#include "SideLine.h"

SideLine::SideLine()
{
	size = 1;
	color = QColor(255, 255, 255);
}

SideLine::~SideLine()
{
}

void SideLine::SetColor(QColor c)
{
	color = c;
}

QColor SideLine::GetColor()
{
	return color;
}


void SideLine::SetSize(int s)
{
	size = s;
}

int SideLine::GetSize()
{
	return size;
}
