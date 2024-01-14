#include "Figure.h"

Figure::Figure()
{
}

Figure::~Figure()
{
}

void Figure::Draw(QPainter & paint, bool chosed)
{
}

void Figure::Move(int dx, int dy)
{
}

void Figure::SetColor(QColor c)
{
	sideline.SetColor(c);
}

void Figure::SetSize(int s)
{
	sideline.SetSize(s);
}


bool Figure::IntervalOverlap(double x1, double x2, double x3, double x4)
{
	double t;
	if (x3 > x4)
	{
		t = x3;
		x3 = x4;
		x4 = t;
	}

	if (x3 > x2 || x4 < x1)
		return false;
	else
		return true;
}

bool Figure::Intersected(int start_point_x, int start_point_y, int end_point_x, int end_point_y)
{
	return false;
}

Line::Line()
{
}

Line::Line(int start_point_x, int start_point_y, int end_point_x, int end_point_y)
{
	start_point_x_ = start_point_x;
	start_point_y_ = start_point_y;
	end_point_x_ = end_point_x;
	end_point_y_ = end_point_y;
}

Line::~Line()
{
}

void Line::Draw(QPainter & paint, bool chosed)
{
	QPen pen;  // creates a default pen
	if (chosed) {
		pen.setWidth(sideline.GetSize() + 2);
		pen.setBrush(QColor(160, 200, 240, 100));
		QBrush brush(QColor(160, 200, 240, 50));
		paint.setBrush(brush);
	}
	else {
		pen.setWidth(sideline.GetSize());
		pen.setBrush(sideline.GetColor());
	}
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	paint.setPen(pen);
	paint.drawLine(start_point_x_, start_point_y_, end_point_x_, end_point_y_);
}

void Line::Move(int dx, int dy)
{
	start_point_x_ += dx;
	end_point_x_ += dx;
	start_point_y_ += dy;
	end_point_y_ += dy;
}

bool Line::Intersected(int start_x, int start_y, int end_x, int end_y)
{
	int rx_min, rx_max, ry_min, ry_max;
	if (start_x < end_x) { rx_min = start_x; rx_max = end_x; }
	else { rx_max = start_x; rx_min = end_x; }
	if (start_y < end_y) { ry_min = start_y; ry_max = end_y; }
	else { ry_max = start_y; ry_min = end_y; }

	int a_x, a_y, b_x, b_y;
	if (start_point_y_ > end_point_y_) { a_y = end_point_y_; a_x = end_point_x_; b_y = start_point_y_; b_x = start_point_x_; }
	else { b_y = end_point_y_; b_x = end_point_x_; a_y = start_point_y_; a_x = start_point_x_; }
	if (a_y == b_y)	// 线段平行于x轴
	{
		if (a_y <= ry_max && a_y >= ry_min)
		{
			return IntervalOverlap(rx_min, rx_max, a_x, b_x);
		}
		else
		{
			return false;
		}
	}
	// 在线段AB上确定点C和D
	// 两点确定一条直线: (x-x1)/(x2-x1)=(y-y1)/(y2-y1)
	double k = 1.0 * (b_x - a_x) / (b_y - a_y);
	int c_x, c_y, d_x, d_y;
	if (a_y < ry_min)
	{
		c_y = ry_min;
		c_x = k * (c_y - a_y) + a_x;
	}
	else {
		c_x = a_x;
		c_y = a_y;
	}

	if (b_y > ry_max)
	{
		d_y = ry_max;
		d_x = k * (d_y - a_y) + a_x;
	}
	else {
		d_x = b_x;
		d_y = b_y;
	}

	if (d_y >= c_y)	// y维上有交集
	{
		return IntervalOverlap(rx_min, rx_max, d_x, c_x);
	}
	else
	{
		return false;
	}
}


Rectangle::Rectangle()
{
}

Rectangle::Rectangle(int start_point_x, int start_point_y, int end_point_x, int end_point_y)
{
	start_point_x_ = start_point_x;
	start_point_y_ = start_point_y;
	end_point_x_ = end_point_x;
	end_point_y_ = end_point_y;
}

Rectangle::~Rectangle()
{
}

void Rectangle::Draw(QPainter & paint, bool chosed)
{
	QPen pen;  // creates a default pen
	if (chosed) {
		pen.setWidth(sideline.GetSize() + 2);
		pen.setBrush(QColor(160, 200, 240, 100));
		QBrush brush(QColor(160, 200, 240, 50));
		paint.setBrush(brush);
	}
	else {
		pen.setWidth(sideline.GetSize());
		pen.setBrush(sideline.GetColor());
	}
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	paint.setPen(pen);

	paint.drawRect(start_point_x_, start_point_y_, end_point_x_ - start_point_x_, end_point_y_ - start_point_y_);
}

void Rectangle::Move(int dx, int dy)
{
	start_point_x_ += dx;
	end_point_x_ += dx;
	start_point_y_ += dy;
	end_point_y_ += dy;
}

bool Rectangle::Intersected(int start_x, int start_y, int end_x, int end_y)
{
	int rx_min, rx_max, ry_min, ry_max;
	if (start_x < end_x) { rx_min = start_x; rx_max = end_x; }
	else { rx_max = start_x; rx_min = end_x; }
	if (start_y < end_y) { ry_min = start_y; ry_max = end_y; }
	else { ry_max = start_y; ry_min = end_y; }

	return IntervalOverlap(rx_min, rx_max, start_point_x_, end_point_x_) && IntervalOverlap(ry_min, ry_max, start_point_y_, end_point_y_);
}


Ellipse::Ellipse()
{
}

Ellipse::Ellipse(int start_point_x, int start_point_y, int end_point_x, int end_point_y)
{
	start_point_x_ = start_point_x;
	start_point_y_ = start_point_y;
	end_point_x_ = end_point_x;
	end_point_y_ = end_point_y;
}

Ellipse::~Ellipse()
{
}

void Ellipse::Draw(QPainter & paint, bool chosed)
{
	QPen pen;  // creates a default pen
	if (chosed) {
		pen.setWidth(sideline.GetSize() + 2);
		pen.setBrush(QColor(160, 200, 240, 100));
		QBrush brush(QColor(160, 200, 240, 50));
		paint.setBrush(brush);
	}
	else {
		pen.setWidth(sideline.GetSize());
		pen.setBrush(sideline.GetColor());
	}
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	paint.setPen(pen);

	paint.drawEllipse(start_point_x_, start_point_y_, end_point_x_ - start_point_x_, end_point_y_ - start_point_y_);
}

void Ellipse::Move(int dx, int dy)
{
	start_point_x_ += dx;
	end_point_x_ += dx;
	start_point_y_ += dy;
	end_point_y_ += dy;
}

bool Ellipse::Intersected(int start_x, int start_y, int end_x, int end_y)
{
	int rx_min, rx_max, ry_min, ry_max;
	if (start_x < end_x) { rx_min = start_x; rx_max = end_x; }
	else { rx_max = start_x; rx_min = end_x; }
	if (start_y < end_y) { ry_min = start_y; ry_max = end_y; }
	else { ry_max = start_y; ry_min = end_y; }

	int x_min, x_max, y_min, y_max;
	if (start_point_x_ < end_point_x_) { x_min = start_point_x_; x_max = end_point_x_; }
	else { x_max = start_point_x_; x_min = end_point_x_; }
	if (start_point_y_ < end_point_y_) { y_min = start_point_y_; y_max = end_point_y_; }
	else { y_max = start_point_y_; y_min = end_point_y_; }

	double x_cen, y_cen;
	x_cen = 1.0 * (x_max + x_min) / 2;
	y_cen = 1.0 * (y_min + y_max) / 2;
	if (IntervalOverlap(rx_min, rx_max, x_max, x_min) && IntervalOverlap(ry_min, ry_max, y_min, y_max)) {
		int c_x, c_y;
		if (IntervalOverlap(rx_min, rx_max, x_cen, x_cen)) {
			if (IntervalOverlap(ry_min, ry_max, y_cen, y_cen)) return true;
			if (abs(ry_max - y_cen) < abs(ry_min - y_cen)) c_y = ry_max;
			else c_y = ry_min;
			if (2 * abs(c_y - y_cen) < y_max - y_min) return true;
		}
		else if (IntervalOverlap(ry_min, ry_max, y_cen, y_cen)) {
			if (IntervalOverlap(rx_min, rx_max, x_cen, x_cen)) return true;
			if (abs(rx_max - x_cen) < abs(rx_min - x_cen)) c_x = rx_max;
			else c_x = rx_min;
			if (2 * abs(c_x - x_cen) < x_max - x_min) return true;
		}
		else {
			if (abs(ry_max - y_cen) < abs(ry_min - y_cen)) c_y = ry_max;
			else c_y = ry_min;
			if (abs(rx_max - x_cen) < abs(rx_min - x_cen)) c_x = rx_max;
			else c_x = rx_min;

			if (((c_x - x_cen) * (c_x - x_cen)) / ((x_max - x_cen)*(x_max - x_cen)) + ((c_y - y_cen) * (c_y - y_cen)) / ((y_max - y_cen)*(y_max - y_cen)) < 1) {
				return true;
			}
		}
	}
	return false;
}

Polygon::Polygon()
{
}

Polygon::Polygon(CDArray<QPoint> pointarray)
{
	points = pointarray;
}

Polygon::~Polygon()
{
}

void Polygon::Draw(QPainter & paint, bool chosed)
{
	QPen pen;  // creates a default pen
	if (chosed) {
		pen.setWidth(sideline.GetSize() + 2);
		pen.setBrush(QColor(160, 200, 240, 100));
		QBrush brush(QColor(160, 200, 240, 50));
		paint.setBrush(brush);
	}
	else {
		pen.setWidth(sideline.GetSize());
		pen.setBrush(sideline.GetColor());
	}
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	paint.setPen(pen);

	paint.drawPolygon(points.GetFPoint(),points.GetSize());
}

void Polygon::Move(int dx, int dy)
{
	CDArray<QPoint> pointarray;
	for (int i = 0; i < points.GetSize(); i++) {
		int x = points[i].rx();
		int y = points[i].ry();
		pointarray.PushBack(QPoint(x + dx, y + dy));
	}
	points.~CDArray();
	points = pointarray;
}

bool Polygon::Intersected(int start_x, int start_y, int end_x, int end_y)
{
	int num = points.GetSize();

	int rx_min, rx_max, ry_min, ry_max;
	if (start_x < end_x) { rx_min = start_x; rx_max = end_x; }
	else { rx_max = start_x; rx_min = end_x; }
	if (start_y < end_y) { ry_min = start_y; ry_max = end_y; }
	else { ry_max = start_y; ry_min = end_y; }

	bool ss = true;
	double rx_cen, ry_cen, s;
	rx_cen = 1.0 * (rx_min + rx_max) / 2;
	ry_cen = 1.0 * (ry_min + ry_max) / 2;
	s = (points[1].ry() - points[0].ry()) * (rx_cen - points[0].rx()) - (points[1].rx() - points[0].rx()) * (ry_cen - points[0].ry());
	for (int i = 0; i < num; i++) {
		int a_x, a_y, b_x, b_y;
		int j = (i + 1) % num;
		if (points[i].ry() > points[j].ry()) { a_y = points[j].ry(); a_x = points[j].rx(); b_y = points[i].ry(); b_x = points[i].rx(); }
		else { b_y = points[j].ry(); b_x = points[j].rx(); a_y = points[i].ry(); a_x = points[i].rx(); }

		if (s * ((points[j].ry() - points[i].ry()) * (rx_cen - points[i].rx()) - (points[j].rx() - points[i].rx()) * (ry_cen - points[i].ry())) < 0) ss = false;

		if (a_y == b_y) {
			if (a_y <= ry_max && a_y >= ry_min) {
				if (IntervalOverlap(rx_min, rx_max, a_x, b_x)) return true;
				else continue;
			}
			else continue;
		}
		// 在线段AB上确定点C和D
		// 两点确定一条直线: (x-x1)/(x2-x1)=(y-y1)/(y2-y1)
		double k = 1.0 * (b_x - a_x) / (b_y - a_y);
		int c_x, c_y, d_x, d_y;
		if (a_y < ry_min) {
			c_y = ry_min;
			c_x = k * (c_y - a_y) + a_x;
		}
		else {
			c_x = a_x;
			c_y = a_y;
		}

		if (b_y > ry_max) {
			d_y = ry_max;
			d_x = k * (d_y - a_y) + a_x;
		}
		else {
			d_x = b_x;
			d_y = b_y;
		}

		if (d_y >= c_y)	{
			if (IntervalOverlap(rx_min, rx_max, d_x, c_x)) return true;
			else continue;
		}
		else continue;
	}
	if (num > 2 && ss) return true;
	return false;
}

Freehand::Freehand()
{
}

Freehand::Freehand(CDArray<QPoint> pointarray)
{
	points = pointarray;
}

Freehand::~Freehand()
{
}

void Freehand::Draw(QPainter & paint, bool chosed)
{
	QPen pen;  // creates a default pen
	if (chosed) {
		pen.setWidth(sideline.GetSize() + 2);
		pen.setBrush(QColor(160, 200, 240, 100));
		QBrush brush(QColor(160, 200, 240, 50));
		paint.setBrush(brush);
	}
	else {
		pen.setWidth(sideline.GetSize());
		pen.setBrush(sideline.GetColor());
	}
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	paint.setPen(pen);

	int num_1 = points.GetSize() - 1;
	for (int i = 0; i < num_1; i++) {
		paint.drawLine(points.GetAt(i).rx(), points.GetAt(i).ry(), points.GetAt(i + 1).rx(), points.GetAt(i + 1).ry());
	}
}

void Freehand::Move(int dx, int dy)
{
	CDArray<QPoint> pointarray;
	for (int i = 0; i < points.GetSize(); i++) {
		int x = points[i].rx();
		int y = points[i].ry();
		pointarray.PushBack(QPoint(x + dx, y + dy));
	}
	points.~CDArray();
	points = pointarray;
}

bool Freehand::Intersected(int start_x, int start_y, int end_x, int end_y)
{
	int rx_min, rx_max, ry_min, ry_max;
	if (start_x < end_x) { rx_min = start_x; rx_max = end_x; }
	else { rx_max = start_x; rx_min = end_x; }
	if (start_y < end_y) { ry_min = start_y; ry_max = end_y; }
	else { ry_max = start_y; ry_min = end_y; }

	for (int i = 0; i < points.GetSize(); i++) {
		int x = points[i].rx();
		int y = points[i].ry();
		if (IntervalOverlap(rx_min, rx_max, x, x) && IntervalOverlap(ry_min, ry_max, y, y)) return true;
	}
	return false;
}
