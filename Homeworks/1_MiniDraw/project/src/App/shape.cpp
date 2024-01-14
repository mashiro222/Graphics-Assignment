#include "shape.h"


Shape::Shape()
{
	drag_point_pen = QPen(QColor(255, 140, 40), 9.0);
	drag_line_pen = QPen(QColor(30, 250, 30, 170), 3.0);
	brush = QBrush(Qt::NoBrush);
}


Shape::~Shape()
{

}

void Shape::Append(QPointF s)
{
	points << s;
}

bool Shape::CheckontoLine(QPointF a, QPointF start_p, QPointF end_p, double widthf)
{
	QPointF s_a = a - start_p;
	QPointF e_a = a - end_p;
	QPointF s_e = end_p - start_p;
	double s_a_dot_s_e = QPointF::dotProduct(s_a, s_e);
	if (s_a_dot_s_e > 0
		&& QPointF::dotProduct(e_a, s_e) < 0
		&& (QPointF::dotProduct(s_a, s_a) - s_a_dot_s_e * s_a_dot_s_e / QPointF::dotProduct(s_e, s_e) <= widthf * widthf / 4.0)
		)
	{
		return true;
	}
	else
		return false;
}

int Shape::CheckDragPoint(QPointF a)
{
	if (type != kFreehand)
	{
		QPointF v;
		for (int i = 0; i < points.size(); i++)
		{
			v = a - points[i];
			if (QPointF::dotProduct(v, v) < 64.)
				return i;
		}
	}
	return -1;
}

void Shape::SetPoint(int a_index, QPointF a)
{
	points[a_index] = a;
}

void Shape::MoveBy(QPointF a_direction)
{
	for (int i = 0; i < points.size(); i++)
	{
		points[i] += a_direction;
	}
}

void Shape::DrawDragPoints(QPainter& painter)
{
	painter.setPen(drag_point_pen);
	for (int i = 0; i < points.size(); i++)
	{
		painter.drawEllipse(points[i], drag_point_r, drag_point_r);
	}
}