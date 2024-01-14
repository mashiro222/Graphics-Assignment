#include "Freehand.h"

CFreehand::CFreehand(QPointF s)
{
	type = kFreehand;
	points.push_back(s);
}

CFreehand::~CFreehand()
{

}

void CFreehand::Draw(QPainter& painter)
{
	painter.setPen(pen);
	painter.drawPolyline(points);
	
}
void CFreehand::DrawSelected(QPainter& painter)
{
	Draw(painter);

	painter.setPen(drag_line_pen);
	painter.drawPolyline(points);

	painter.setPen(drag_point_pen);
	painter.drawEllipse(points[0],3. , 3.);
	painter.drawEllipse(points[points.size() - 1], 3., 3.);
}
bool CFreehand::CheckSelect(QPointF a)
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