#pragma once

#include "Shape.h"

class Freehand : public Shape {
public:
	Freehand(QPoint point);
	~Freehand();

	void Draw(QPainter& painter);
	void update(QPoint point);

private:
	QPainterPath path;
};