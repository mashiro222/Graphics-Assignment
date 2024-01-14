#pragma once
#include "Warping.h"

class WarpingRBF :
	public Warping
{
private:
	Matrix2d M;
	Vector2d b;
	Matrix<double, Dynamic, 2> a;
	QVector<double> r;
	int u = 1;

public:
	WarpingRBF(QVector<QLine*>& data_pair_);
	~WarpingRBF();
	void set_u(int new_u);

private:
	double basis(int i, double d) const;
	void Init_r();
	void Init_affine_transformation();
	void Init_a();
	Vector2d warp_func(const QPoint& p) const;							
};

