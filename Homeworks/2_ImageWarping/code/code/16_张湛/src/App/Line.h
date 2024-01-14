#pragma once
#if !defined _LINE_H_
#define _LINE_H_

#include <QPainter>

class Line
{
public:
	Line(void) {

	}
	Line(int start_point_x, int start_point_y, int end_point_x, int end_point_y)
	{
		start_point_x_ = start_point_x;
		start_point_y_ = start_point_y;
		end_point_x_ = end_point_x;
		end_point_y_ = end_point_y;
	}
	~Line(void) {

	}

public:
	void Draw(QPainter &paint, int dw = 0, int dh =0)
	{
		paint.drawLine(start_point_x_ + dw, start_point_y_ + dh, end_point_x_ + dw, end_point_y_ + dh);
	}
	int get_start_point_x_() {
		return start_point_x_;
	}
	int get_start_point_y_() {
		return start_point_y_;
	}
	int get_end_point_x_() {
		return end_point_x_;
	}
	int get_end_point_y_() {
		return end_point_y_;
	}

private:
	int	start_point_x_, start_point_y_, end_point_x_, end_point_y_;
};

#endif