#include "Polygon.h"

CPolygon::CPolygon()
{
	type = kPolygon;
}


CPolygon::~CPolygon()
{

}

void CPolygon::Draw(QPainter& painter)
{
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawPolygon(points);
}
void CPolygon::DrawSelected(QPainter& painter)
{
	Draw(painter);

	painter.setPen(drag_line_pen);
	painter.setBrush(QBrush(Qt::NoBrush));
	painter.drawPolygon(points);

	DrawDragPoints(painter);
}

void CPolygon::SetPoints(QPolygonF apoints)
{
	points = apoints;
}

bool CPolygon::CheckSelect(QPointF a)
{
	for (int i = 1; i < points.size(); i++)
	{
		if (CheckontoLine(a, points[i], points[i - 1], pen.widthF()))
			return true;
	}
	if (CheckontoLine(a, points[0], points[points.size() - 1], pen.widthF()))
		return true;
	else
		return false;
}