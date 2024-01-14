#pragma once

#include "Shape.h"
#include <QPolygon>
// #include <QGraphicsPolygonItem>

class MPolygon :public Shape,public QGraphicsPolygonItem
{
public:
	MPolygon(QPen pen);
	~MPolygon();

	void Draw(QPainter &painter,bool done);
	void Draw(QPainter &painter);

	bool check_in(QPoint f);
};
