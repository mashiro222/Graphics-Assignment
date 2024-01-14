#include "Line.h"


CLine::CLine()
{
	type = kLine;
	points.push_back(QPointF(0., 0.));
	points.push_back(QPointF(0., 0.));
}

CLine::CLine(QPointF a_start, QPointF a_end)
{
	type = kLine;
	points.push_back(a_start);
	points.push_back(a_end);
}

CLine::~CLine()
{
}

void CLine::Draw(QPainter& painter)
{
	painter.setPen(pen);
	painter.drawLine(points[0], points[1]);
}
void CLine::DrawSelected(QPainter& painter)
{
	Draw(painter);

	painter.setPen(drag_line_pen);
	painter.drawLine(points[0], points[1]);

	DrawDragPoints(painter);
}

void CLine::SetStartPoint(QPointF s)
{
	points[0] = s;
}

void CLine::SetEndPoint(QPointF e)
{
	points[1] = e;
}

bool CLine::CheckSelect(QPointF a)
{
	QPointF s_a = a - points[0];
	QPointF e_a = a - points[1];
	QPointF s_e = points[1] - points[0];
	double s_a_dot_s_e = QPointF::dotProduct(s_a, s_e);
	if ( s_a_dot_s_e > 0
		&& QPointF::dotProduct(e_a, s_e) < 0
		&& (QPointF::dotProduct(s_a, s_a) - s_a_dot_s_e * s_a_dot_s_e / QPointF::dotProduct(s_e, s_e) <= 4. + pen.widthF() * pen.widthF() / 4.0)
		)
	{
		return true;
	}
	else
		return false; 
}