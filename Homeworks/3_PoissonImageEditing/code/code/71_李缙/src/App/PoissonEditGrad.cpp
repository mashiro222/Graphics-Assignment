#include "PoissonEditGrad.h"


CPoissonEditGrad::CPoissonEditGrad(void)
{
}


CPoissonEditGrad::~CPoissonEditGrad(void)
{
}

int CPoissonEditGrad::v(QPoint p, QPoint q, int rgb, QPoint pos)
{
	switch (rgb)
	{
	case 0:
        return qRed(imagewidget_s->image().pixel(p)) - qRed(imagewidget_s->image().pixel(q));
	case 1:
        return qGreen(imagewidget_s->image().pixel(p)) - qGreen(imagewidget_s->image().pixel(q));
	case 2:
        return qBlue(imagewidget_s->image().pixel(p)) - qBlue(imagewidget_s->image().pixel(q));
	default:
		break;
	}
	return 0;
}
