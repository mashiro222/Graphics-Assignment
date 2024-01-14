#pragma once

#include "shape.h"

class Freehand : public Shape
{
public:

	Freehand(QPointF s);
	~Freehand();

	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);
	void AppendPoint(QPointF s) { points << s; };

	bool CheckSelect(QPointF a);

	void FillParallel(QPainter& paint) {}
	void FillFermat(QPainter& paint) {}
	void FillZigzag(QPainter& paint) {}
};