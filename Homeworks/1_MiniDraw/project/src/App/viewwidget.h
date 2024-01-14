#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include<qevent.h>
#include<qpainter.h>
#include "ui_viewwidget.h"
#include "shape.h"
#include "Line.h"
#include "Rect.h"
#include "Ellip.h"
#include "Poly.h"
#include "Freehand.h"
#include "BezierCurve.h"
#include "BSpline.h"
#include "BiBspline.h"

#include<vector>
#include<list>

class ViewWidget : public QWidget
{
	Q_OBJECT


public:
	ViewWidget(QWidget* parent = 0);
	~ViewWidget();

private:
	Ui::ViewWidget ui;

	enum EEditingMode
	{
		kDrawing = 0,
		kSelecting = 1,
		kSelected = 2,
	};

	enum Filltype
	{
		kParallel = 0,
		kFermat = 1,
		kZigzag = 2,
	};

	std::vector<Filltype>			fill_mode;
	EEditingMode		editing_mode;		//当前的编辑模式
	bool				draw_status_;
	bool				mouse_pos_visible;
	bool				shape_filled;

	int					selected_shape_index;
	int					drag_point_index;

	QPixmap				p_canvas;			//画布缓冲区
	QPainter			painter;
	QPoint				start_point_;		//鼠标按下的起点
	QPoint				last_point_;		//上次的鼠标位置
	QPoint				end_point_;			//鼠标按下的终点
	Shape::Type		type_;				//当前绘制类型
	Shape* shape_;			//正在绘制的图形
	std::vector<Shape*> shape_list_;
	
	std::vector<Shape*> shape_filled_;		//要填充的shape

	QPen				pen;
	QBrush				brush;

	QImage* ptr_image;


private:
	void AddShapetoList();					//把正在绘制的新shape_加入shap_list中

public:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void Undo();
	void Redo();

public:
	void paintEvent(QPaintEvent*);
	void setPenWidth(int a_pen_width);
	void setPenColor(QColor a_color);
	void setFilled(QBrush abrush = QBrush(Qt::NoBrush));

signals:

public slots:
	void setModeSelect();
	void DoDelete();
	void setModeLine();
	void setModeRect();
	void setModeEllipse();
	void setModePolygon();
	void setModeFreehand();
	void setModeBezier();
	void setModeBSpline();
	void setModeBiBspline();
	void setUndo();
	
	void DoParallel();
	void DoFermat();
	void DoZigzag();

	void Open();
	void DeletePtr();
};

#endif // VIEWWIDGET_H
