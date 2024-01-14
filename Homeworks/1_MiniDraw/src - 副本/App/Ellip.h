#pragma once

#include "Shape.h"

class Ellip : public Shape {
public:
	Ellip();
	~Ellip();

	void Draw(QPainter& painter);
};