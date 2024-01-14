#pragma once

#include"shape.h"

class CLine :public CFigure
{
public:
	CLine();
	CLine(QPointF a_start, QPointF a_end);
	~CLine();

	void SetStartPoint(QPointF s);
	void SetEndPoint(QPointF e);

	void Draw(QPainter& painter);
	bool CheckSelect(QPointF s);

	void DrawSelected(QPainter& painter);

};

