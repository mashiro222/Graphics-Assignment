#pragma once

#include"Shape.h"

class Rect:public Shape,public QGraphicsRectItem
{
public:
	Rect(QPen pen);
	~Rect();

	void Draw(QPainter &painter);
	void Draw(QPainter &painter, bool done);

	bool check_in(QPoint f);
};

