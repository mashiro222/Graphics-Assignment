#include "Ellip.h"

Ellip::Ellip()
{
}

Ellip::~Ellip()
{
}

void Ellip::Draw(QPainter& painter)
{
    painter.drawEllipse(start.rx(), start.ry(), end.rx() - start.rx(), end.ry() - start.ry());
}