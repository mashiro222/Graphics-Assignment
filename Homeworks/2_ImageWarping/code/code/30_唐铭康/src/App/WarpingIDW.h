#pragma once
#include "Warping.h"

class WarpingIDW :
	public Warping
{
private:
	int u = 2;
	QVector<Matrix2d> T;	// local approximation matrix
							// fi(p) = qi + Ti(p - pi)

public:
	WarpingIDW(QVector<QLine*>& data_pair_);
	~WarpingIDW();
	void set_u(int new_u);

private:
	double inverse_dist(int i, const QPoint& p) const;					// di := 1 / d( . , pi)
	double w_cof(int i, const QPoint& p) const;	// wi 
	Vector2d local_approximation_func(int i, const QPoint& p) const;	// fi
	Vector2d warp_func(const QPoint& p) const;							// f = Sum(wi * fi)
};

