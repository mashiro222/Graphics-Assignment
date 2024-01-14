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
	if (path.elementCount() == 0)//��·�����޵㣬�򽫵���ӵ�·������ʼλ��
	{
		path.moveTo(point);
	}
	else
	{
		path.lineTo(point); 
	}
}