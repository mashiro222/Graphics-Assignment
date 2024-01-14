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

//ʵ�ֻ�ͼ����
class ViewWidget : public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget* parent = 0);
	~ViewWidget();

private:
	Ui::ViewWidget ui;

private:
	bool draw_status_; // ��ǰ����״̬��true Ϊ���Ƶ�ǰ����϶���ͼԪ��false Ϊ������
	bool translate_status_;
	QPoint start_point_; // ��ǰͼԪ����ʼ��
	QPoint end_point_; // ��ǰͼԪ����ֹ��
	QPoint p1_translate_;
	QPoint p2_translate_;
	Shape::Type type_; // ͼԪ�����ͣ�
	Shape* shape_; // ͼԪ����״��
	std::vector<Shape*> shape_list_; // ����ͼԪ�����飿
	QPen *pen_;
	// QVector<QPoint> points_;


public:
	void mousePressEvent(QMouseEvent* event); // ��������Ӧ���������Ҽ�����˫����
	void mouseMoveEvent(QMouseEvent* event); // ����ƶ���Ӧ����
	void mouseReleaseEvent(QMouseEvent* event); // ����ͷ���Ӧ���������Ҽ�����˫����
	void mouseDoubleClickEvent(QMouseEvent* event);

public:
	void paintEvent(QPaintEvent*); // Qt ���еĻ��ƶ�ֻ���ڴ˺��������
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
