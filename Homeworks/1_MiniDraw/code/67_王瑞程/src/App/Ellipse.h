#pragma once

#include "shape.h"

class CEllipse :public CFigure
{
public:
	CEllipse();
	CEllipse(QPointF s, QPointF e);
	~CEllipse();

	void SetEllipse(QPointF s, QPointF e);
	void Draw(QPainter &painter);
	void DrawSelected(QPainter& painter);

	bool CheckSelect(QPointF a);

};
