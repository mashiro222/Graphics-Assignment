#pragma once

#include "shape.h"

class Fill : public Shape
{
public:
	enum Type {
		kparallel = 0,
		kzigzag = 1,
		kfermat = 2,
	};
public:
	Fill();
	Fill(QPointF s);
	~Fill();

	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);

	void SetPoints(QPolygonF points);
	void AppendPoint(QPointF s) { Append(s); };

	bool CheckSelect(QPointF a);

	QPointF GetPoint(int index) { return points[index]; };
	QPolygonF GetPoints() { return points; };

	int SizeofPoints() { return points.size(); }
};

