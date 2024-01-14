#include "viewwidget.h"
	
//构造函数，进行一些初始化设置
ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false;
	shape_ = NULL;
	type_ = Shape::kDefault;
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::setLine()
{
	type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
	type_ = Shape::kRect;
}

void ViewWidget::setEllip()
{
	type_ = Shape::kEllip;
}

void ViewWidget::setPoly()
{
	type_ = Shape::kPoly;
}

void ViewWidget::setFreehand()
{
	type_ = Shape::kFreehand;
}

void ViewWidget::setUndo() {
	//删除最新绘制的图形
	if (shape_list_.size() > 0) {
		delete shape_list_.back();
		shape_list_.pop_back();
	}
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{

	if (Qt::LeftButton == event->button())//判断是否鼠标左击
	{

		start_point_ = end_point_ = event->pos();

		switch (type_)
		{
		case Shape::kLine:
			shape_ = new Line();
			break;
		case Shape::kDefault:
			break;

		case Shape::kRect:
			shape_ = new Rect();
			break;

		case Shape::kEllip:
			shape_ = new Ellip();
			break;

		case Shape::kFreehand:
			shape_ = new Freehand(start_point_);//Freehand和Poly初始化记录start point
			break;

		case Shape::kPoly:
			if (shape_ == NULL)
			{
				shape_ = new Poly(start_point_);
				setMouseTracking(true);//该状态始终运行mousemoveevent
			}
			break;
		default:
			break;
		}
		if (shape_ != NULL)
		{
			draw_status_ = true;
			shape_->set_start(start_point_);
			shape_->set_end(end_point_);
		}
	}
	update();
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status_ && shape_ != NULL)
	{
		end_point_ = event->pos();
		shape_->set_end(end_point_);
		shape_->AddPoint(end_point_);//poly的
		shape_->update(end_point_);//freehand的
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (type_ != Shape::kPoly)
	{
		draw_status_ = false;
		if (shape_ != NULL) {
			shape_list_.push_back(shape_);
			shape_ = NULL;
		}
	}
	else {
		if (Qt::LeftButton == event->button()) {
			shape_->AddPoint(1);
		}
		//右键结束
		if (Qt::RightButton == event->button()) {
			draw_status_ = false;
			shape_->AddPoint(0);
			shape_list_.push_back(shape_);
			shape_ = NULL;
			setMouseTracking(false);
		}
	}
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	for (int i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}

	if (shape_ != NULL) {
		shape_->Draw(painter);
	}

	update();
}