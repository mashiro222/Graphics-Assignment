#include "viewwidget.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false; // 设置初始绘制状态为 – 不绘制
	translate_status_ = false; 
	shape_ = NULL; // 设置初始绘制形状为 – NULL
	type_ = Shape::kDefault;
	pen_ = new QPen();
	setMouseTracking(true);
}

// 好像这里和hello程序不一样，原因是这里没有初始化表达式，即没有给ui实例化？？？
ViewWidget::~ViewWidget()
{
	for (int i = 0; i < shape_list_.size(); i++) 
	{
		shape_list_[i]->~Shape();
	}
	if (shape_ != NULL)
	{
		delete shape_;
		shape_ = NULL;
	}
	delete pen_;
}

void ViewWidget::setLine()
{
	type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
	type_ = Shape::kRect;
}

void ViewWidget::setEllipse() 
{
	type_ = Shape::kEllipse;
}

void ViewWidget::setPolygon()
{
	type_ = Shape::kPolygon;
}

void ViewWidget::setFreehand()
{
	type_ = Shape::kFreehand;
}

void ViewWidget::setUndo()
{
	if (shape_list_.empty())
		return;
	shape_list_.back()->~Shape();
	shape_list_.pop_back();
}

void ViewWidget::setPenWidth(int index)
{
	// int size = QDial::notchSize();
	pen_->setWidth(index);
}

void ViewWidget::setColor()
{
	QColor color = QColorDialog::getColor();
	pen_->setColor(color);
}

void ViewWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	// 如果单击左键
	if (Qt::LeftButton == event->button())
	{
		if (shape_ == NULL)
		{
			for (int i = 0; i < shape_list_.size(); i++)
			{
				if (shape_list_[i]->check_in(event->pos()))
				{
					shape_ = shape_list_[i];
					translate_status_ = true;
					p1_translate_ = event->pos();
					return;
				}
			}
			switch (type_)
			{
				case Shape::kLine:
					shape_ = new Line(*pen_);
					break;

				case Shape::kDefault:
					break;

				case Shape::kRect:
					shape_ = new Rect(*pen_);
					break;

				case Shape::kEllipse:
					shape_ = new MEllipse(*pen_);
					break;

				case Shape::kPolygon:
					shape_ = new MPolygon(*pen_);
					break;

				case Shape::kFreehand:
					shape_ = new FreeHand(*pen_);
					break;
			}

			if (shape_ != NULL)
			{
				draw_status_ = true;
				start_point_ = end_point_ = event->pos();
				shape_->set_start(start_point_);
				shape_->set_end(end_point_);
				if (type_ == Shape::kPolygon)
				{
					shape_->append_point(end_point_);
					shape_->append_point(end_point_);
				}
			}
		}
	}
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	setCursor(Qt::ArrowCursor);
	for (int i = 0; i < shape_list_.size(); i++)
	{
		if (shape_list_[i]->check_in(event->pos()))
		{
			setCursor(Qt::PointingHandCursor);
		}
	}
	if (draw_status_ && shape_ != NULL)
	{
		end_point_ = event->pos();
		shape_->set_end(end_point_);
		if (type_ == Shape::kFreehand)
		{
			shape_->append_path(end_point_);
		}
		if (type_ == Shape::kPolygon)
		{
			shape_->move_point(end_point_);
		}
	}
	if (translate_status_ && shape_ != NULL)
	{
		p2_translate_ = event->pos();
		//  p2_translate_ - p1_translate_ = 本次鼠标位置-上次鼠标位置
		shape_->set_trans(p2_translate_ - p1_translate_);
		p1_translate_ = p2_translate_;
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	// 如果是右键松开并且
	if (draw_status_)
	{
		if (type_ == Shape::kPolygon && shape_ != NULL)
		{
			if (Qt::RightButton == event->button())
			{
				end_point_ = event->pos();
				shape_->move_point(end_point_);
				draw_status_ = false;
				shape_list_.push_back(shape_);
				shape_ = NULL;
			}
			else
			{
				start_point_ = end_point_ = event->pos();
				shape_->append_point(end_point_);
			}
		}
		if (type_ != Shape::kPolygon && shape_ != NULL)
		{
			draw_status_ = false;
			shape_list_.push_back(shape_);
			shape_ = NULL;
		}
	}
	if (translate_status_)
	{
		translate_status_ = false;
		shape_ = NULL;
	}
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setPen(*pen_);
	for (int i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}
	if (shape_ != NULL && translate_status_==false) {
		if (type_ == Shape::kPolygon)
		{
			shape_->Draw(painter, true);
		}
		else
		{
			shape_->Draw(painter);
		}
	}
	update();
}