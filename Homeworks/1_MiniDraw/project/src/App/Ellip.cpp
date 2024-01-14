
#include "Ellip.h"
#include <iostream>

Ellip::Ellip()
{
	type = kEllipse;
	points.push_back(QPointF(0., 0.));
	points.push_back(QPointF(0., 0.));
}

Ellip::Ellip(QPointF s, QPointF e)
{
	type = kEllipse;
	points.push_back(s);
	points.push_back(e);
}

Ellip::~Ellip()
{

}

void Ellip::Draw(QPainter& painter)
{
	painter.setPen(pen);
	painter.setBrush(brush);

	painter.drawEllipse(QRectF(points[0], points[1]));
}

void Ellip::SetEllipse(QPointF s, QPointF e)
{
	points[0] = s;
	points[1] = e;
}

bool Ellip::CheckSelect(QPointF a)
{
	double width = std::fabs(points[0].x() - points[1].x());
	double height = std::fabs(points[0].y() - points[1].y());

	double x = (points[0].x() + points[1].x()) / 2.0;
	double y = (points[0].y() + points[1].y()) / 2.0;

	double dist = ((a.x() - x) / width) * ((a.x() - x) / width) + ((a.y() - y) / height) * ((a.y() - y) / height);
	double rng = pen.widthF() / std::min(width, height);

	return brush == QBrush(Qt::NoBrush) ? (fabs(4.0 * dist - 1.0) <= rng) : (4.0 * dist <= 1. + rng);
}

void Ellip::DrawSelected(QPainter& painter)
{
	Draw(painter);

	painter.setPen(drag_line_pen);
	painter.setBrush(QBrush(Qt::NoBrush));
	painter.drawRect(QRectF(points[0], points[1]));

	DrawDragPoints(painter);
}