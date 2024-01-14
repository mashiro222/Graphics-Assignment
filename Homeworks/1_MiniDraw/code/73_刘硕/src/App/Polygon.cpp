#include "Polygon.h"

MPolygon::MPolygon(QPen pen)
{
	pen_ = pen;
}

MPolygon::~MPolygon()
{
	delete trans;
}

void MPolygon::Draw(QPainter& painter) 
{
	setPolygon(QPolygon(points_));
	QStyleOptionGraphicsItem* option = new QStyleOptionGraphicsItem();
	setPen(pen_);
	painter.translate(*trans);
	paint(&painter, option);
	painter.resetTransform();
	delete option;
}

void MPolygon::Draw(QPainter& painter,bool ndone)
{
	painter.setPen(pen_);
	painter.drawPolyline(points_);
}
bool MPolygon::check_in(QPoint f)
{
	return shape().contains(f - *trans);
}
