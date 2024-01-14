#pragma once
#include "shape.h"
#include "BezierCurve.h"
#include "vector"

class BSpline : public Shape
{
public:
	BSpline();
	BSpline(QPointF s);
	~BSpline();

	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);

	//to do
	double N(int i,int k, double t);//Îª»ùº¯Êý
	std::vector<double> knots;
	void setknots(int n, int k);

	void SetPoints(QPolygonF points);
	void AppendPoint(QPointF s) { Append(s); };

	bool CheckSelect(QPointF a);

	QPointF GetPoint(int index) { return points[index]; };
	QPolygonF GetPoints() { return points; };

	int SizeofPoints() { return points.size(); }

	void FillParallel(QPainter& paint) {}
	void FillFermat(QPainter& paint) {}
	void FillZigzag(QPainter& paint) {}
};

