#pragma once
#include "shape.h"
#include "SciMathx.h"

class CBezierCurve :
	public CFigure
{
public:
	CBezierCurve();
	CBezierCurve(QPointF s);
	~CBezierCurve();

	void Draw(QPainter &painter);
	void DrawSelected(QPainter& painter);

	void SetPoints(QPolygonF points);
	void AppendPoint(QPointF s) { Append(s); };

	bool CheckSelect(QPointF a);

	QPointF GetPoint(int index) { return points[index]; };
	QPolygonF GetPoints() { return points; };

	int SizeofPoints() { return points.size(); }
};

