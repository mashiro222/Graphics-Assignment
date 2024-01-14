#include "BezierCurve.h"

using namespace SciMathx;
CMathx Mathx;

CBezierCurve::CBezierCurve()
{
	type = kBezierCurve;
}
CBezierCurve::CBezierCurve(QPointF s)
{
	type = kBezierCurve;
	points.append(s);
}

CBezierCurve::~CBezierCurve()
{
}

void CBezierCurve::Draw(QPainter& painter)
{
	painter.setPen(pen);
	QPolygonF path;
	int n = points.size() - 1;
	for (double t = 0; t <= 1.0; t += 0.01)
	{
		QPointF p(0., 0.);
		for (int i = 0; i <= n; i++)
		{
			p += Mathx.Comb(n, i)*pow(t, i)*pow(1. - t, n - i)*points[i];
		}
		path.append(p);
	}
	painter.drawPolyline(path);
}
void CBezierCurve::DrawSelected(QPainter& painter)
{
	Draw(painter);

	painter.setPen(drag_line_pen);
	painter.drawPolyline(points);

	DrawDragPoints(painter);
}

void CBezierCurve::SetPoints(QPolygonF apoints)
{
	points = apoints;
}

bool CBezierCurve::CheckSelect(QPointF a)
{
	QPolygonF path;
	int n = points.size() - 1;
	for (double t = 0; t <= 1.0; t += 0.01)
	{
		QPointF p(0., 0.);
		for (int i = 0; i <= n; i++)
		{
			p += Mathx.Comb(n, i)*pow(t, i)*pow(1. - t, n - i)*points[i];
		}
		path.append(p);
	}
	for (int i = 1; i < path.size(); i++)
	{
		if (CheckontoLine(a, path[i], path[i - 1], pen.widthF()))
			return true;
	}
	return false;
}