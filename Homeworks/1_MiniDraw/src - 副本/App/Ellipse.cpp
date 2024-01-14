#include "Ellipse.h"

Ellipse::Ellipse()
{
}

Ellipse::~Ellipse()
{
}

void Ellipse::Draw(QPainter& painter)
{
    painter.drawEllipse(start.rx(), start.ry(), end.rx() - start.rx(), end.ry() - start.ry());
}