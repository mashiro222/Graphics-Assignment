#include "Warping.h"
#define MAX4(a,b,c,d) a>b?(a>c?(a>d?a:d):(c>d?c:d)):(b>c?(b>d?b:d):(c>d?c:d));
#define MIN4(a,b,c,d) a<b?(a<c?(a<d?a:d):(c<d?c:d)):(b<c?(b<d?b:d):(c<d?c:d));

void Warping::InitPoints(QVector<QLine*>& data_pair_)
{
	for (int i = 0; i < data_pair_.size(); i++)
	{
		// Guaranteeing that there are not same two points in p_
		bool f = true;
		for (int j = 0; j < i; j++)
			if (p_[j] == data_pair_[i]->p1())
			{
				f = false;
				break;
			}

		if (f)
		{
			p_.push_back(data_pair_[i]->p1());
			q_.push_back(data_pair_[i]->p2());
		}
	}
	ptr_control_points_ = &data_pair_;
	//ReassignPoints();
}

Warping::Warping()
{
}

Warping::~Warping()
{
}

void Warping::Warp(QImage* image_, int setgap, bool warpPoints)
{
	if (image_->isNull())
	{
		return;
	}

	QImage image_tmp(*(image_));
	image_->fill(Qt::white);


	// Initiate squares
	int gap;
	if (setgap <= 0)
	{
		int min = image_tmp.width();
		if (image_tmp.height() < min)
		{
			min = image_tmp.height();
		}
		gap = 1 + min / 20;
	}
	else
	{
		gap = setgap;
	}
	int count_width = 1 + (image_tmp.width() - 1) / gap + (image_tmp.width() % gap != 0);
	int count_height = 1 + (image_tmp.height() - 1) / gap + (image_tmp.height() % gap != 0);
	QVector<QVector<QPoint>> vertices_old(count_width, QVector<QPoint>(count_height , QPoint(0, 0)));
	QVector<QVector<Vector2d>> vertices_new(count_width, QVector<Vector2d>(count_height, Vector2d(0.0, 0.0)));

	for (int i = 0; i < count_width - 1; i++)
	{
		for (int j = 0; j < count_height - 1; j++)
		{
			vertices_old[i][j] = QPoint(i * gap, j * gap);
		}
		vertices_old[i][count_height - 1] = QPoint(i * gap, image_tmp.height() - 1);
	}
	for (int j = 0; j < count_height - 1; j++)
	{
		vertices_old[count_width - 1][j] = QPoint(image_tmp.width() - 1, j * gap);
	}
	vertices_old[count_width - 1][count_height - 1] = QPoint(image_tmp.width() - 1, image_tmp.height() - 1);


	// Do warping to the vertices
	for (int i = 0; i < count_width; i++)
	{
		for (int j = 0; j < count_height; j++)
		{
			vertices_new[i][j] = warp_func(vertices_old[i][j]);
			if (0 <= int(vertices_new[i][j][0]) && int(vertices_new[i][j][0]) < image_tmp.width()
				&& 0 <= int(vertices_new[i][j][1]) && int(vertices_new[i][j][1]) < image_tmp.height())
				image_->setPixel(vertices_new[i][j][0], vertices_new[i][j][1],
					image_tmp.pixel(vertices_old[i][j].x(), vertices_old[i][j].y()));
		}
	}

	// Do warping to each square
	for (int i = 1; i < count_width; i++)
	{
		for (int j = 1; j < count_height; j++)
		{
			// get vertices
			Vector2d A = vertices_new[i - 1][j - 1];
			Vector2d B = vertices_new[i - 1][j];
			Vector2d C = vertices_new[i][j - 1];
			Vector2d D = vertices_new[i][j];
			QPoint A0 = vertices_old[i - 1][j - 1];
			QPoint B0 = vertices_old[i - 1][j];
			QPoint C0 = vertices_old[i][j - 1];
			QPoint D0 = vertices_old[i][j];

			// estimate range of new vertices
			int top = MIN4(A.x(), B.x(), C.x(), D.x());
			top = top >= 0 ? top : 0;
			int bottom = MAX4(A.x(), B.x(), C.x(), D.x());
			bottom = bottom < image_tmp.width() ? bottom : image_tmp.width() - 1;
			int left = MIN4(A.y(), B.y(), C.y(), D.y());
			left = left >= 0 ? left : 0;
			int right = MAX4(A.y(), B.y(), C.y(), D.y());;
			right = right < image_tmp.height() ? right : image_tmp.height() - 1;

			// find inner points of the square and estimate their source points
			for (int row = top; row <= bottom; row++)
			{
				for (int col = left; col <= right; col++)
				{
					if (isInTriangle(A, B, C, row, col))
					{
						Vector2d co = cooridinates(A, B, C, row, col);
						QPoint old = A0 + co[0] * (B0 - A0) + co[1] * (C0 - A0);
						if (old.x() >= 0 && old.y() >= 0
							&& old.x() < image_tmp.width() && old.y() < image_tmp.height())
							image_->setPixel(row, col, image_tmp.pixel(old));
					}
					else if (isInTriangle(D, B, C, row, col))
					{
						Vector2d co = cooridinates(D, B, C, row, col);
						QPoint old = D0 + co[0] * (B0 - D0) + co[1] * (C0 - D0);
						if (old.x() >= 0 && old.y() >= 0
							&& old.x() < image_tmp.width() && old.y() < image_tmp.height())
							image_->setPixel(row, col, image_tmp.pixel(old));
					}
				}
			}
		}
	}

	if (warpPoints)
	{
		for (int i = 0; i < q_.size(); i++)
		{
			Vector2d tmp = warp_func(q_[i]);
			bool anchor = p_[i] == q_[i];
			p_[i] = q_[i];
			q_[i] = QPoint(tmp[0], tmp[1]);
			if (anchor)
			{
				p_[i] = q_[i];
			}
		}
		ReassignPoints();
	}
}

double Warping::dist(const QPoint& p1, const QPoint& p2) const
{
	return sqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));	
}

int Warping::isInTriangle(Vector2d A, Vector2d B, Vector2d C, int x, int y) const
{
	double signOfAB = (B[0] - A[0]) * (y - A[1]) - (B[1] - A[1]) * (x - A[0]);
	double signOfCA = (A[0] - C[0]) * (y - C[1]) - (A[1] - C[1]) * (x - C[0]);
	double signOfBC = (C[0] - B[0]) * (y - B[1]) - (C[1] - B[1]) * (x - B[0]);
	if ((signOfAB > -W_EPSILON && signOfCA > -W_EPSILON && signOfBC > -W_EPSILON)
		|| (signOfAB < W_EPSILON && signOfCA < W_EPSILON&& signOfBC < W_EPSILON)) 
	{
		return 1;
	}

	return 0;
}

Vector2d Warping::cooridinates(Vector2d O, Vector2d A, Vector2d B, int x, int y) const
{
	Vector2d OA = A - O;
	Vector2d OB = B - O;
	Vector2d OX(x - O[0], y - O[1]);
	Matrix2d cof;
	cof << OA[0], OB[0],
		OA[1], OB[1];
	Vector2d sol = cof.lu().solve(OX);
	return sol;
}

Vector2d Warping::warp_func(const QPoint& p) const
{
	return Vector2d();
}

void Warping::ReassignPoints()
{
	ptr_control_points_->resize(p_.size());
	for (int i = 0; i < p_.size(); i++)
	{
		ptr_control_points_->at(i)->setLine(p_.at(i).x(), p_.at(i).y(), q_.at(i).x(), q_.at(i).y());
	}
}
