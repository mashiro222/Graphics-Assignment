#include "Line.h"

Line::Line(QPen pen)
{
	pen_ = pen;
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}

Line::~Line()
{
	delete trans;
}

void Line::Draw(QPainter& painter)
{
	// painter.setPen(pen_);
	// painter.drawLine(start, end);
	setLine(start.rx(), start.ry(), end.rx(), end.ry());
	QStyleOptionGraphicsItem* option = new QStyleOptionGraphicsItem();
	setPen(pen_);
	painter.translate(*trans);
	paint(&painter, option);
	painter.resetTransform();
	delete option;
}

void Line::Draw(QPainter &painter, bool done){}

bool Line::check_in(QPoint f)
{
	return shape().contains(f - *trans);
}