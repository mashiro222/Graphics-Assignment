#include "WarpingRBF.h"

WarpingRBF::WarpingRBF(QVector<QLine*>& data_pair_)
{
	InitPoints(data_pair_);
	Init_r();
	Init_affine_transformation();
	Init_a();
}

WarpingRBF::~WarpingRBF()
{
}

void WarpingRBF::set_u(int new_u)
{
	u = new_u;
}

Vector2d WarpingRBF::warp_func(const QPoint& p) const
{
	Vector2d x{ p.x(),p.y() };
	Vector2d sol = M * x + b;
	for (int i = 0; i < p_.size(); i++)
	{
		sol = sol + Vector2d(a(i, 0), a(i, 1)) * basis(i, dist(p, p_[i]));
	}
	return sol;
}

double WarpingRBF::basis(int i, double d) const
{
	return pow(sqrt(pow(d, 2) + pow(r[i], 2)), u);
}

void WarpingRBF::Init_r()
{
	r = QVector<double>(p_.size(), 0);
	if (p_.size() == 1)
	{
		r[0] = 1;	// default r
	}
	else
	{
		for (int i = 0; i < r.size(); i++)
		{
			r[i] = i != 0 ? dist(p_[i], p_[0]) : dist(p_[i], p_[1]);
			for (int j = 1; j < r.size(); j++)
			{
				if (j != i)
				{
					double tmp = dist(p_[i], p_[j]);
					r[i] = tmp < r[i] ? tmp : r[i];
				}
			}
		}
	}
	return;
}

void WarpingRBF::Init_affine_transformation()
{
	Matrix<double, Dynamic, 3> X;
	Matrix<double, Dynamic, 2> Y;
	X.resize(p_.size(), 3);
	Y.resize(p_.size(), 2);

	// X = (1, p_.x, p_.y), Y1 = (q_.x, q_.y)
	for (int i = 0; i < p_.size(); i++)
	{
		X(i, 0) = 1;
		X(i, 1) = p_[i].x();
		X(i, 2) = p_[i].y();
		Y(i, 0) = q_[i].x();
		Y(i, 1) = q_[i].y();
	}
	
	switch (p_.size())
	{
	case 1:								// given one anchor point, make a translation, i.e. M = I
		M(0, 0) = 1;
		M(1, 0) = 0;
		M(0, 1) = 0;
		M(1, 1) = 1;
		b[0] = q_[0].x() - p_[0].x();
		b[1] = q_[0].y() - p_[0].y();
		break;

	case 2:								// given two anchor points, make a translation and scaling, i.e. M = Diag(d11, d22)
		M(1, 0) = 0;
		M(0, 1) = 0;
		M(0, 0) = (p_[1] - p_[0]).x() == 0 ? 1 : double((q_[1] - q_[0]).x()) / (p_[1] - p_[0]).x();
		M(1, 1) = (p_[1] - p_[0]).y() == 0 ? 1 : double((q_[1] - q_[0]).y()) / (p_[1] - p_[0]).y();
		b[0] = q_[0].x() - M(0, 0) * p_[0].x();
		b[1] = q_[1].y() - M(1, 1) * p_[1].y();
		break;

	default:
		Matrix<double, 3, 2> parameters;	// (theta_x, theta_y), theta_x = (b(0), M(0, 0), M(0, 1))^T, ...
		parameters.fill(0.0);
		parameters = (X.transpose() * X).inverse() * X.transpose() * Y;
		b[0] = parameters(0, 0);
		b[1] = parameters(0, 1);
		M(0, 0) = parameters(1, 0);
		M(1, 0) = parameters(1, 1);
		M(0, 1) = parameters(2, 0);
		M(1, 1) = parameters(2, 1);
		break;
	}	
	
}

void WarpingRBF::Init_a()
{
	Matrix<double, Dynamic, Dynamic> G;
	Matrix<double, Dynamic, 2> f;
	G.resize(p_.size(), p_.size());
	a.resize(p_.size(), 2);
	f.resize(p_.size(), 2);

	Vector2d tmp;

	for (int i = 0; i < p_.size(); i++)
	{
		for (int j = 0; j < p_.size(); j++)
		{
			G(i, j) = basis(j, dist(p_[i], p_[j]));
		}
		tmp = Vector2d(q_[i].x(), q_[i].y()) - M * Vector2d(p_[i].x(), p_[i].y()) - b;
		f(i, 0) = tmp[0];
		f(i, 1) = tmp[1];
	}

	a = G.colPivHouseholderQr().solve(f);
}
