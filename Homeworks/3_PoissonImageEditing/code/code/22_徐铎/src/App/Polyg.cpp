#include "Polyg.h"

Polyg::Polyg()
{
}

Polyg::~Polyg()
{

}

void Polyg::Draw(QPainter& painter)
{
	for (size_t i = 0; i < points.size() - 1; i++) {
		painter.drawLine(points[i], points[i + 1]);
	}
	painter.drawLine(points[points.size() - 1], end);
}
