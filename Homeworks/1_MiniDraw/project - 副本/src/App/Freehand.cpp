#include "Freehand.h"

Freehand::Freehand(QPoint point)
{
	path.moveTo(point);
}

Freehand::~Freehand()
{
}

void Freehand::Draw(QPainter& painter)
{
    painter.drawPath(path);
}

void Freehand::update(QPoint point) {
	if (path.elementCount() == 0)//若路径中无点，则将点添加到路径的起始位置
	{
		path.moveTo(point);
	}
	else
	{
		path.lineTo(point); 
	}
}