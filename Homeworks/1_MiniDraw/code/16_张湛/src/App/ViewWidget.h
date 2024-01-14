#pragma once

#include <QWidget>
#include "CDArray.h"
#include <QFrame>
#include <QPainter>
#include <vector>
#include <Figure.h>
#include "ui_ViewWidget.h"

using namespace std;

enum FigureType
{
	kDefault = 0,
	kLine = 1,
	kRectangle = 2,
	kEllipse = 3,
	kPolygon = 4,
	kFreehand = 5,
};

enum ShiftState
{
	SReleased = 0,
	SPressed = 1,
};

class ViewWidget : public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget *parent = Q_NULLPTR);
	~ViewWidget();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *);
	QColor getColor();

public slots:
	void set_figure_type_to_line();
	void set_figure_type_to_rectangle();
	void set_figure_type_to_ellipse();
	void set_figure_type_to_polygon();
	void set_figure_type_to_freehand();
	void undo_current_figure();
	void delete_chosed_figure();
	void activate_chose_mod();

	void showColor();
	void setSize(int s);

private:
	Ui::ViewWidget ui;

	bool draw_status_;

	QFrame *colorFrame;
	QColor current_color;
	int current_size;

	CDArray<QPoint> point_array_;
	QPoint start_point_;
	QPoint end_point_;

	vector<Figure* >  figure_array_;
	vector<int >  chosed_figure_array_;

	bool chose_mod_;
	FigureType  figure_type_;
	ShiftState shift_state_;
};

