#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include<qevent.h>
#include<qpainter.h>
#include "ui_viewwidget.h"
#include "shape.h"
#include "Line.h"
#include "Rect.h"
#include "Ellipse.h"
#include "Polygon.h"
#include "Freehand.h"
#include "BezierCurve.h"

#include<vector>
#include<list>

class ViewWidget : public QWidget
{
	Q_OBJECT


public:
	ViewWidget(QWidget *parent = 0);
	~ViewWidget();

private:
	Ui::ViewWidget ui;

private:
	//操作基类
	class COperation
	{
	public:
		virtual void Undo(std::vector<CFigure*>&) {}
		virtual void Redo(std::vector<CFigure*>&) {}
	};
	class COpAdd : public COperation
	{
	public:
		CFigure* cache;
		COpAdd();
		~COpAdd();
		void Undo(std::vector<CFigure*>& a_sh_list);
		void Redo(std::vector<CFigure*>& a_sh_list);
	};
	class COpRemove : public COperation
	{
	public:
		int index;
		CFigure* cache;
		COpRemove(int index, CFigure* a_figure);
		~COpRemove();
		void Undo(std::vector<CFigure*>& a_sh_list);
		void Redo(std::vector<CFigure*>& a_sh_list);
	};
	class COpChange : public COperation
	{
	public:
		int index;
		CFigure* cache_bef, *cache_aft;
		COpChange(int a_index, CFigure* a_figure_bef, CFigure* a_figure_aft);
		~COpChange();
		void Undo(std::vector<CFigure*>& a_sh_list);
		void Redo(std::vector<CFigure*>& a_sh_list);
	};

	enum EEditingMode
	{
		kDrawing = 0,
		kSelecting = 1,
		kSelected = 2,
	};
	
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
	CFigure::Type		type_;				//当前绘制类型
	CFigure				*shape_;			//正在绘制的图形
	std::vector<CFigure*> shape_list_;	

	QPen				pen;
	QBrush				brush;

	std::list<COperation*> operations_undo;	//撤销操作list
	std::list<COperation*> operations_redo;	//重做操作list
	
private:
	void AddShapetoList();					//把正在绘制的新shape_加入shap_list中

public:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void Undo();
	void Redo();

public:
	void paintEvent(QPaintEvent *);
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
};

#endif // VIEWWIDGET_H
