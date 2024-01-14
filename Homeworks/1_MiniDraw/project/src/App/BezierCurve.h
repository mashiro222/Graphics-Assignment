#pragma once
#include "shape.h"
#include "SciMathx.h"

class BezierCurve : public Shape
{
public:
	BezierCurve();
	BezierCurve(QPointF s);
	~BezierCurve();

	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);

	void SetPoints(QPolygonF points);
	void AppendPoint(QPointF s) { Append(s); };

	bool CheckSelect(QPointF a);

	QPointF GetPoint(int index) { return points[index]; };
	QPolygonF GetPoints() { return points; };

	int SizeofPoints() { return points.size(); }

	void FillParallel(QPainter& paint){}
	void FillFermat(QPainter& paint) {}
	void FillZigzag(QPainter& paint) {}
};

