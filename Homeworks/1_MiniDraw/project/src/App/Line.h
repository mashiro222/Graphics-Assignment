#pragma once

#include"shape.h"

class Line :public Shape
{
public:
	Line();
	Line(QPointF a_start, QPointF a_end);
	~Line();

	void SetStartPoint(QPointF s);
	void SetEndPoint(QPointF e);

	void Draw(QPainter& painter);
	bool CheckSelect(QPointF s);

	void DrawSelected(QPainter& painter);

	void FillParallel(QPainter& paint) {}
	void FillFermat(QPainter& paint) {}
	void FillZigzag(QPainter& paint) {}
};

