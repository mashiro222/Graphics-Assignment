#pragma once

#include"shape.h"
class CRect:public CFigure
{
public:
	CRect();
	CRect(QPointF s, QPointF e);
	~CRect();

	void SetRect(QPointF s, QPointF e);

	void Draw(QPainter &painter);
	void DrawSelected(QPainter& painter);

	bool CheckSelect(QPointF a);
	
};

