#ifndef FIGURE_H
#define FIGURE_H

#include <QPainter.h>
#include "CDArray.h"
#include "SideLine.h"
#include <vector>

using namespace std;

class Figure
{
public:
	Figure();
	virtual ~Figure();
	virtual void Draw(QPainter &paint, bool chosed = false);
	virtual void Move(int dx, int dy);
	virtual void SetColor(QColor c);
	virtual void SetSize(int s);
	virtual bool IntervalOverlap(double x1, double x2, double x3, double x4);
	virtual bool Intersected(int start_point_x, int start_point_y, int end_point_x, int end_point_y);
protected:
	SideLine sideline;
};

class Line : public Figure
{
public:
	Line();
	Line(int start_point_x, int start_point_y, int end_point_x, int end_point_y);
	~Line();
	void Draw(QPainter &paint, bool chosed = false);
	void Move(int dx, int dy);
	bool Intersected(int start_point_x, int start_point_y, int end_point_x, int end_point_y);

private:
	int	start_point_x_, start_point_y_, end_point_x_, end_point_y_;
};

class Rectangle : public Figure
{
public:
	Rectangle();
	Rectangle(int start_point_x, int start_point_y, int end_point_x, int end_point_y);
	~Rectangle();
	void Draw(QPainter &paint, bool chosed = false);
	void Move(int dx, int dy);
	bool Intersected(int start_point_x, int start_point_y, int end_point_x, int end_point_y);

private:
	int	start_point_x_, start_point_y_, end_point_x_, end_point_y_;
};


class Ellipse : public Figure
{
public:
	Ellipse();
	Ellipse(int start_point_x, int start_point_y, int end_point_x, int end_point_y);
	~Ellipse();
	void Draw(QPainter &paint, bool chosed = false);
	void Move(int dx, int dy);
	bool Intersected(int start_point_x, int start_point_y, int end_point_x, int end_point_y);

private:
	int	start_point_x_, start_point_y_, end_point_x_, end_point_y_;
};

class Polygon : public Figure
{
public:
	Polygon();
	Polygon(CDArray<QPoint> pointarray);
	~Polygon();
	void Draw(QPainter &paint, bool chosed = false);
	void Move(int dx, int dy);
	bool Intersected(int start_point_x, int start_point_y, int end_point_x, int end_point_y);

private:
	CDArray<QPoint> points;
};

class Freehand : public Figure
{
public:
	Freehand();
	Freehand(CDArray<QPoint> pointarray);
	~Freehand();
	void Draw(QPainter &paint, bool chosed = false);
	void Move(int dx, int dy);
	bool Intersected(int start_point_x, int start_point_y, int end_point_x, int end_point_y);

private:
	CDArray<QPoint> points;
};

#endif // !FIGURE_H