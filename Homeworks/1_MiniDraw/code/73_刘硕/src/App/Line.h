#pragma once

#include"shape.h"

class Line :public Shape, public QGraphicsLineItem
{
public:
	Line(QPen pen);
	~Line();

	void Draw(QPainter& painter);
	void Draw(QPainter &painter, bool done);

	bool check_in(QPoint f);
};

