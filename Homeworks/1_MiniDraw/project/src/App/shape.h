#pragma once

#include <iostream>
#include <QtGui>

class Shape
{
public:
	Shape();
	virtual ~Shape();

public:
	enum Type
	{
		kDefault = 0,
		kLine = 1,
		kRect = 2,
		kEllipse = 3,
		kPolygon = 4,
		kFreehand = 5,
		kBezierCurve = 6,
		kBSpline = 7,
		kBiBspline = 8,
	};

protected:
	QPolygonF points;
	QPen drag_point_pen;
	QPen drag_line_pen;
	const double drag_point_r = 4.0;

	void Append(QPointF s);
	void DrawDragPoints(QPainter& painter);
public:
	Type	type;
	QPen	pen;
	QBrush	brush;
	bool	filled;

	virtual void Draw(QPainter& paint) = 0;
	virtual void DrawSelected(QPainter& painter) = 0;
	virtual bool CheckSelect(QPointF a) = 0;
	virtual void FillParallel(QPainter& paint) = 0;
	virtual void FillFermat(QPainter& paint) = 0;
	virtual void FillZigzag(QPainter& paint) = 0;

	int CheckDragPoint(QPointF a);

	void SetPoint(int a_index, QPointF a);
	void MoveBy(QPointF a_direction);

	static bool CheckontoLine(QPointF a, QPointF start_p, QPointF end_p, double widthf);
};

