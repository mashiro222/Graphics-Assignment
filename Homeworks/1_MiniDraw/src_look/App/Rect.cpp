#include "Rect.h"


CRect::CRect()
{
	type = kRect;
	points.push_back(QPointF(0., 0.));
	points.push_back(QPointF(0., 0.));
}

CRect::CRect(QPointF s, QPointF e)
{
	type = kRect;
	points.push_back(s);
	points.push_back(e);
}

CRect::~CRect()
{
}

void CRect::SetRect(QPointF s, QPointF e)
{
	points[0] = s;
	points[1] = e;
}


void CRect::Draw(QPainter& painter)
{
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(QRectF(points[0],points[1]));
}

bool CRect::CheckSelect(QPointF a)
{
	double x = std::min(points[0].x(), points[1].x());
	double y = std::min(points[0].y(), points[1].y());
	double width = std::fabs(points[0].x() - points[1].x());
	double height = std::fabs(points[0].y() - points[1].y());
	return brush == QBrush(Qt::NoBrush) ?
		((std::abs(a.y() - y) < pen.widthF() / 2. && (x - pen.widthF() / 2.) < a.x() && a.x() < (x + width + pen.widthF() / 2.))
		|| (std::abs(a.y() - y - height) < pen.widthF() / 2. && (x - pen.widthF() / 2.) < a.x() && a.x() < (x + width + pen.widthF() / 2.))
		|| (std::abs(a.x() - x) < pen.widthF() / 2. && (y - pen.widthF() / 2.) < a.y() && a.y() < (y + height + pen.widthF() / 2.))
		|| (std::abs(a.x() - x - width) < pen.widthF() / 2. && (y - pen.widthF() / 2.) < a.y() && a.y() < (y + height + pen.widthF() / 2.))
		)
		:
		x <= a.x() && a.x() - x <= width && y <= a.y() && a.y() - y <= height;
}

void CRect::DrawSelected(QPainter& painter)
{
	Draw(painter);

	painter.setPen(drag_line_pen);
	painter.setBrush(QBrush(Qt::NoBrush));
	painter.drawRect(QRectF(points[0],points[1]));

	DrawDragPoints(painter);
}