#pragma once

#include "shape.h"

class CFreehand : public CFigure
{
public:

	CFreehand(QPointF s);
	~CFreehand();

	void Draw(QPainter& painter);
	void DrawSelected(QPainter& painter);
	void AppendPoint(QPointF s) { points << s; };

	bool CheckSelect(QPointF a);

};