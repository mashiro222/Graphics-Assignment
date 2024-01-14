#include "FreeHand.h"
FreeHand::FreeHand(QPen pen)
{
	pen_ = pen;
}

FreeHand::~FreeHand()
{
	delete trans;
}

void FreeHand::Draw(QPainter& painter)
{
	setPath(path_);
	QStyleOptionGraphicsItem* option = new QStyleOptionGraphicsItem();
	setPen(pen_);
	painter.translate(*trans);
	paint(&painter, option);
	painter.resetTransform();
	delete option;
}
void FreeHand::Draw(QPainter &painter, bool done)
{
}

bool FreeHand::check_in(QPoint f)
{
	return shape().contains(f - *trans);
}