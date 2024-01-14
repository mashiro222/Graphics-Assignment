#include "shape.h"

Shape::Shape()
{
}

Shape::~Shape()
{
}

void Shape::set_start(QPoint s)
{
	start = s;
	path_.moveTo(s);
}

void Shape::set_end(QPoint e)
{
	end = e;
}

void Shape::set_trans(QPoint p)
{
	*trans += p;
}

void Shape::append_point(QPoint v)
{
	points_.append(v);
}

void Shape::append_path(QPoint v)
{
	path_.lineTo(v);
}

void Shape::move_point(QPoint w)
{
	// points_[points_.size() - 1] = w;
	points_.back() = w;
}
