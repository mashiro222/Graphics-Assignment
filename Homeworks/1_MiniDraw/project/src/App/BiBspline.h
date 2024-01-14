#pragma once
#include "shape.h"
#include "BezierCurve.h"
#include "vector"

class BiBspline : public Shape
{
public:
	BiBspline();
	BiBspline(QPointF s);
	~BiBspline();

	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);

	double N(int i, int k, double t);//Îª»ùº¯Êý
	std::vector<double> knots;
	void setknots(int n, int k);

	int findnearest(QPointF a, std::vector<QPointF> list);

	void FillParallel(QPainter& painter);
	void FillFermat(QPainter& painter);
	void FillZigzag(QPainter& paint);

	void SetPoints(QPolygonF points);
	void AppendPoint(QPointF s) { Append(s); };

	bool CheckSelect(QPointF a);

	QPointF GetPoint(int index) { return points[index]; };
	QPolygonF GetPoints() { return points; };

	int SizeofPoints() { return points.size(); }
};

