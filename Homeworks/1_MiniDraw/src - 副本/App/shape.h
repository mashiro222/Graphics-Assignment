#pragma once

#include <QtGui>

class Shape
{
public:
	Shape();
	virtual ~Shape();
	virtual void Draw(QPainter& paint) = 0;
	void set_start(QPoint s);
	void set_end(QPoint e);
	virtual void AddPoint(QPoint& point) {};
	virtual void AddPoint(int type) {};
	virtual void update(QPoint point) {};

public:
	enum Type
	{
		kDefault = 0,
		kLine = 1,
		kRect = 2,
		kEllip = 3,
		kPoly = 4,
		kFreehand = 5,
	};

protected:
	QPoint start;
	QPoint end;
};

