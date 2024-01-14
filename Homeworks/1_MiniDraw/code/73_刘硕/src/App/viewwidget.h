#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "ui_viewwidget.h"

#include "shape.h"
#include "Line.h"
#include "Rect.h"
#include "Ellipse.h"
#include "Polygon.h"
#include "FreeHand.h"

#include <qevent.h>
#include <qpainter.h>
#include <QWidget>
#include <QDial>
#include <QColorDialog>

#include <vector>

//实现绘图功能
class ViewWidget : public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget* parent = 0);
	~ViewWidget();

private:
	Ui::ViewWidget ui;

private:
	bool draw_status_; // 当前绘制状态，true 为绘制当前鼠标拖动的图元，false 为不绘制
	bool translate_status_;
	QPoint start_point_; // 当前图元的起始点
	QPoint end_point_; // 当前图元的终止点
	QPoint p1_translate_;
	QPoint p2_translate_;
	Shape::Type type_; // 图元的类型？
	Shape* shape_; // 图元的形状？
	std::vector<Shape*> shape_list_; // 绘制图元的数组？
	QPen *pen_;
	// QVector<QPoint> points_;


public:
	void mousePressEvent(QMouseEvent* event); // 鼠标击发响应函数（左右键，单双击）
	void mouseMoveEvent(QMouseEvent* event); // 鼠标移动响应函数
	void mouseReleaseEvent(QMouseEvent* event); // 鼠标释放响应函数（左右键，单双击）
	void mouseDoubleClickEvent(QMouseEvent* event);

public:
	void paintEvent(QPaintEvent*); // Qt 所有的绘制都只能在此函数中完成
signals:
public slots:
	void setLine();
	void setRect();
	void setEllipse();
	void setPolygon();
	void setFreehand();
	void setUndo();
	void setPenWidth(int index);
	void setColor();
};

#endif // VIEWWIDGET_H
