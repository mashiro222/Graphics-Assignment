#pragma once

#include"shape.h"
class Rect :public Shape
{
public:
	Rect();
	Rect(QPointF s, QPointF e);
	~Rect();

	void SetRect(QPointF s, QPointF e);

	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);

	bool CheckSelect(QPointF a);

	void FillParallel(QPainter& paint) {}
	void FillFermat(QPainter& paint) {}
	void FillZigzag(QPainter& paint) {}
};

