#pragma once

#include "shape.h"

class Ellip :public Shape
{
public:
	Ellip();
	Ellip(QPointF s, QPointF e);
	~Ellip();

	void SetEllipse(QPointF s, QPointF e);
	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);

	bool CheckSelect(QPointF a);

	void FillParallel(QPainter& paint) {}
	void FillFermat(QPainter& paint) {}
	void FillZigzag(QPainter& paint) {}
};