#pragma once

#include"Shape.h"

class MEllipse :public Shape,public QGraphicsEllipseItem
{
public:
	MEllipse(QPen pen);
	~MEllipse();

	void Draw(QPainter &painter);
	void Draw(QPainter &painter, bool done);

	bool check_in(QPoint f);
};
