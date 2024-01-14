#pragma once
#include <QVector>
#include <QLine>
#include <QImage>
#include <Eigen/Dense>
#define W_EPSILON 1e-16

using namespace Eigen;

QT_BEGIN_NAMESPACE
	class	QImage;
QT_END_NAMESPACE

class Warping
{
protected:
	QVector<QPoint>	p_, q_;	// initial points and target points, respectively
	QVector<QLine*>* ptr_control_points_ = Q_NULLPTR; // store the pointer of the control points
	void InitPoints(QVector<QLine*>& data_pair_);	 // make sure there are not two same initial points

public:
	Warping();
	virtual ~Warping();
	void Warp(QImage* image_, int setgap = 0, bool warpPoints = false);	// warp the image
	virtual Vector2d warp_func(const QPoint& p) const;	// warping function, such that f(p_) == q_
	void ReassignPoints();								// assign p_, q_ to *ptr_control_points_

protected:
	double dist(const QPoint& p1, const QPoint& p2) const;	// distance function

private:
	int isInTriangle(Vector2d A, Vector2d B, Vector2d C, int x, int y) const;	// judge if the point (x, y) is in the square ABC
	Vector2d cooridinates(Vector2d O, Vector2d A, Vector2d B, int x, int y) const; // calculate the coordinates of (x, y) in Span{OA, OB}
};

