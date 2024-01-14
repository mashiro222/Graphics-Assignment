#pragma once

#include "Shape.h"

class Poly : public Shape {
public:
	Poly(QPoint point);
	~Poly();

	void Draw(QPainter& painter);
	void AddPoint(QPoint& point);
	void AddPoint(int type);


private:
	QPolygon points;
	bool complete;
}; 
