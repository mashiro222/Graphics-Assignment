#include "Rect.h"

Rect::Rect(QPen pen)
{
	pen_ = pen;
	setAcceptHoverEvents(true);
}

Rect::~Rect()
{
	delete trans;
}

void Rect::Draw(QPainter& painter)
{
	setRect(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
	QStyleOptionGraphicsItem* option = new QStyleOptionGraphicsItem();
	setPen(pen_);
	painter.translate(*trans);
	paint(&painter, option);
	painter.resetTransform();
	delete option;
}

void Rect::Draw(QPainter &painter, bool done)
{
}

bool Rect::check_in(QPoint f)
{
	return shape().contains(f - *trans);
}
