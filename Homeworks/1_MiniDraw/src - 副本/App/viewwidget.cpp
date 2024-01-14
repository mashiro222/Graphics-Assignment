#include "viewwidget.h"
	
//���캯��������һЩ��ʼ������
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
	//ɾ�����»��Ƶ�ͼ��
	if (shape_list_.size() > 0) {
		delete shape_list_.back();
		shape_list_.pop_back();
	}
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{

	if (Qt::LeftButton == event->button())//�ж��Ƿ�������
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
			shape_ = new Freehand(start_point_);//Freehand��Poly��ʼ����¼start point
			break;

		case Shape::kPoly:
			if (shape_ == NULL)
			{
				shape_ = new Poly(start_point_);
				setMouseTracking(true);//��״̬ʼ������mousemoveevent
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
		shape_->AddPoint(end_point_);//poly��
		shape_->update(end_point_);//freehand��
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
		//�Ҽ�����
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