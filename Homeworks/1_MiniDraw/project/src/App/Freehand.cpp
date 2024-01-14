#include "Freehand.h"

Freehand::Freehand(QPointF s)
{
	type = kFreehand;
	points.push_back(s);
}

Freehand::~Freehand()
{

}

void Freehand::Draw(QPainter& painter)
{
	painter.setPen(pen);
	painter.drawPolyline(points);

}
void Freehand::DrawSelected(QPainter& painter)
{
	Draw(painter);

	painter.setPen(drag_line_pen);
	painter.drawPolyline(points);

	painter.setPen(drag_point_pen);
	painter.drawEllipse(points[0], 3., 3.);
	painter.drawEllipse(points[points.size() - 1], 3., 3.);
}
bool Freehand::CheckSelect(QPointF a)
{
	QPointF v;
	for (int i = 0; i < points.size(); i++)
	{
		v = a - points[i];
		if (QPointF::dotProduct(v, v) < pen.widthF() * pen.widthF() / 4.0)
		{
			return true;
		}
	}
	return false;
}