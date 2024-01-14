#pragma once
#include"Shape.h"

class FreeHand :public Shape,public QGraphicsPathItem
{
public:
	FreeHand(QPen pen);
	~FreeHand();

	void Draw(QPainter &painter);
	void Draw(QPainter &painter, bool done);

	bool check_in(QPoint f);
};

