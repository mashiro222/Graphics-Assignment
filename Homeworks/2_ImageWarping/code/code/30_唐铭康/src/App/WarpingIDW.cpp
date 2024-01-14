#include "WarpingIDW.h"

WarpingIDW::WarpingIDW(QVector<QLine*>& data_pair_)
{
	InitPoints(data_pair_);

	// Calculate T[i]
	T = QVector<Matrix2d>(p_.size());
	for (int i = 0; i < p_.size(); i++)
	{
		Matrix2d A;
		Vector2d b1, b2, t1, t2;
		A.fill(0.0);				// ((A, O), (O, A)) is coefficient matrix
		b1.fill(0.0);				// (b1^T, b2^T)^T is constant term vector
		b2.fill(0.0);

		for (int j = 0; j < p_.size(); j++)
		{	// j!=i
			if (j == i)
			{
				continue;
			}

			int dpx = (p_[j] - p_[i]).x();
			int dpy = (p_[j] - p_[i]).y();
			int dqx = (q_[j] - q_[i]).x();
			int dqy = (q_[j] - q_[i]).y();
			double cof = inverse_dist(i, p_[j]);

			// 0.5 * the 1st equation
			A(0, 0) += dpx * dpx * cof;
			A(0, 1) += dpx * dpy * cof;
			b1[0] += dpx * dqx * cof;

			// 0.5 * the 2nd equation
			A(1, 0) += dpy * dpx * cof;
			A(1, 1) += dpy * dpy * cof;
			b1[1] += dpy * dqx * cof;

			// 0.5 * the 3rd equation
			b2[0] += dpx * dqy * cof;

			// 0.5 * the 4th equation
			b2[1] += dpy * dqy * cof;
		}

		t1 = A.lu().solve(b1);	// (t1^T, t2^T)^T is the solution
		t2 = A.lu().solve(b2);

		T[i](0, 0) = t1[0];
		T[i](0, 1) = t1[1];
		T[i](1, 0) = t2[0];
		T[i](1, 1) = t2[1];
	}
}

WarpingIDW::~WarpingIDW()
{
}

void WarpingIDW::set_u(int new_u)
{
	if (u > 0)			
	{
		u = new_u;
	}
}

double WarpingIDW::inverse_dist(int i, const QPoint& p) const
{
	assert(0 <= i && i < p_.size());

	double d = dist(p_[i], p);
	if (fabs(d) < W_EPSILON)
	{
		return 0.0;
	}
	else
	{
		QPoint pi = p_[i];
		return 1.0 / pow(d, u);
	}
}

double WarpingIDW::w_cof(int i, const QPoint& p) const
{
	assert(0 <= i && i < p_.size());

	double sum = 0.0;
	for (int j = 0; j < p_.size(); j++)
	{
		if (p == p_[j])
		{
			return i == j ? 1.0 : 0.0;
		}
		else
		{
			sum += inverse_dist(j, p);
		}
	}

	assert(fabs(sum) >= W_EPSILON);	// otherwise invert_dist(i,p) <= W_EPSILON, which means p == p_[i] and the function would have returned
	return inverse_dist(i, p) / sum;
}

Vector2d WarpingIDW::local_approximation_func(int i, const QPoint& p) const
{
	assert(0 <= i && i < p_.size());

	QPoint diff = p - p_[i], qi = q_[i];
	Vector2d ans{ 0,0 };
	ans[0] = T[i](0, 0) * diff.x() + T[i](0, 1) * diff.y() + qi.x();
	ans[1] = T[i](1, 0) * diff.x() + T[i](1, 1) * diff.y() + qi.y();
	// fi(p) = qi + Ti(p - pi)

	return ans;
}

Vector2d WarpingIDW::warp_func(const QPoint& p) const
{
	Vector2d ans{ 0,0 };
	for (int i = 0; i < p_.size(); i++)
	{
		// f = Sum(wi * fi)
		ans += w_cof(i, p) * local_approximation_func(i, p);
	}
	return ans;
}