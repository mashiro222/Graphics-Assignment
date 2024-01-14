#include "Ellipse.h"

MEllipse::MEllipse(QPen pen)
{
	pen_ = pen;
}

MEllipse::~MEllipse()
{
	delete trans;
}

void MEllipse::Draw(QPainter& painter)
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

void MEllipse::Draw(QPainter &painter, bool done)
{
}

bool MEllipse::check_in(QPoint f)
{
	return shape().contains(f - *trans);
}