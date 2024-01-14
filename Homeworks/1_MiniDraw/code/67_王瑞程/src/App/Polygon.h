#pragma once

#include "shape.h"
#include <vector>

class CPolygon :public CFigure
{
public:
	CPolygon();
	~CPolygon();

	

	void Draw(QPainter &painter);
	void DrawSelected(QPainter& painter);

	void SetPoints(QPolygonF points);
	void AppendPoint(QPointF s) { Append(s); };

	bool CheckSelect(QPointF a);

	QPointF GetPoint(int index) { return points[index]; };
	QPolygonF GetPoints() { return points; };

	int SizeofPoints() { return points.size(); }

};
