#pragma once

#include <QtGui>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsItem>
#include <QGraphicsView>

#include <iostream>

class Shape
{
public:
	Shape();
	virtual ~Shape();
	virtual void Draw(QPainter &paint)=0;
	virtual void Draw(QPainter &paint,bool done) = 0;
	void set_start(QPoint s);
	void set_end(QPoint e);
	void set_trans(QPoint p);
	void append_point(QPoint v);
	void append_path(QPoint v);
	void move_point(QPoint w);
	virtual bool check_in(QPoint f) = 0;

public:
	enum Type
	{
		kDefault = 0,
		kLine = 1,
		kRect = 2,
		kEllipse = 3,
		kPolygon = 4,
		kFreehand = 5,
	};
	
protected:
	QPoint start;
	QPoint end;
	QPoint* trans = new QPoint(0,0);
	QPainterPath path_;
	QVector<QPoint> points_;
	QPen pen_;
};

