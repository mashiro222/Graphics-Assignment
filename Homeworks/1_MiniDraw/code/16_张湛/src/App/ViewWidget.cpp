#include "ViewWidget.h"
#include <QMouseEvent>
#include <QColorDialog>
#include <QImage>

ViewWidget::ViewWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	chose_mod_ = false;
	figure_type_ = kDefault;
	shift_state_ = SReleased;
	draw_status_ = false;
	current_color.setRgb(0x00, 0xFF, 0x00);
	current_size = 1;
}

ViewWidget::~ViewWidget()
{
	for (size_t i = 0; i < figure_array_.size(); i++)
	{
		if (figure_array_[i])
		{
			delete figure_array_[i];
			figure_array_[i] = NULL;
		}
	}
}

void ViewWidget::mousePressEvent(QMouseEvent * event)
{
	setFocusPolicy(Qt::StrongFocus);
	if (Qt::LeftButton == event->button()||(figure_type_ != kPolygon && figure_type_ != kFreehand && !chose_mod_ && Qt::RightButton == event->button())){
		chosed_figure_array_.clear();
		if(Qt::RightButton == event->button()) shift_state_ = SPressed;
		draw_status_ = true;
		start_point_ = end_point_ = event->pos();
		point_array_.PushBack(start_point_);
		if(point_array_.GetSize() == 1) point_array_.PushBack(end_point_);
	}
	if (Qt::RightButton == event->button() && chose_mod_) {
		draw_status_ = false;
		start_point_ = end_point_ = event->pos();
		point_array_.PushBack(start_point_);
		if (point_array_.GetSize() == 1) point_array_.PushBack(end_point_);
	}
}

void ViewWidget::mouseMoveEvent(QMouseEvent * event)
{
	if(figure_type_ != kFreehand && !chose_mod_) point_array_.PopBack();
	if (draw_status_){
		if (shift_state_ == SPressed) {
			int dy = (event->pos().ry() - start_point_.ry());
			int dx = (event->pos().rx() - start_point_.rx());
			if (dx * dy > 0) {
				if (dx * dx < dy * dy) end_point_ = QPoint(event->pos().rx(), start_point_.ry() + dx);
				else end_point_ = QPoint(start_point_.rx() + dy, event->pos().ry());
			}
			else {
				if (dx * dx < dy * dy) end_point_ = QPoint(event->pos().rx(), start_point_.ry() - dx);
				else end_point_ = QPoint(start_point_.rx() - dy, event->pos().ry());
			}
		}
		else end_point_ = event->pos();
		point_array_.PushBack(end_point_);
	}
	if (chose_mod_) {
		end_point_ = event->pos();
		if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton) {
			chosed_figure_array_.clear();
			for (int i = 0; i < figure_array_.size(); i++) {
				if (figure_array_[i]->Intersected(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry())) chosed_figure_array_.push_back(i);
			}
		}
		if ((event->buttons() & Qt::RightButton) == Qt::RightButton) {
			int num = chosed_figure_array_.size();
			for (size_t i = 0; i < num; i++) {
				figure_array_[chosed_figure_array_[i]]->Move(end_point_.rx() - point_array_[point_array_.GetSize() - 1].rx(), end_point_.ry() - point_array_[point_array_.GetSize() - 1].ry());
			}
		}
		point_array_.PushBack(end_point_);
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent * event)
{
	Figure* current_figure_ = NULL;
	if (Qt::RightButton == event->button()) shift_state_ = SReleased;
	switch (figure_type_)
	{
	case kDefault:
		break;
	case kLine:
		current_figure_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		current_figure_->SetColor(current_color);
		current_figure_->SetSize(current_size);
		figure_array_.push_back(current_figure_);
		//chosed_figure_array_.push_back(current_figure_);
		draw_status_ = false;
		point_array_.SetSize(0);
		break;
	case kRectangle:
		current_figure_ = new Rectangle(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		current_figure_->SetColor(current_color);
		current_figure_->SetSize(current_size);
		figure_array_.push_back(current_figure_);
		//chosed_figure_array_.push_back(current_figure_);
		draw_status_ = false;
		point_array_.SetSize(0);
		break;
	case kEllipse:
		current_figure_ = new Ellipse(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		current_figure_->SetColor(current_color);
		current_figure_->SetSize(current_size);
		figure_array_.push_back(current_figure_);
		//chosed_figure_array_.push_back(current_figure_);
		draw_status_ = false;
		point_array_.SetSize(0);
		break;
	case kPolygon:
		if (Qt::RightButton == event->button()) {
			draw_status_ = false;
			current_figure_ = new Polygon(point_array_);
			current_figure_->SetColor(current_color);
			current_figure_->SetSize(current_size);
			figure_array_.push_back(current_figure_);
			//chosed_figure_array_.push_back(current_figure_);
			point_array_.SetSize(0);
		}
		break;
	case kFreehand:
		current_figure_ = new Freehand(point_array_);
		current_figure_->SetColor(current_color);
		current_figure_->SetSize(current_size);
		figure_array_.push_back(current_figure_);
		//chosed_figure_array_.push_back(current_figure_);
		draw_status_ = false;
		point_array_.SetSize(0);
		break;
	default:
		break;
	}
	if (chose_mod_) {
		if (Qt::LeftButton == event->button()) {
			chosed_figure_array_.clear();
			for (int i = 0; i < figure_array_.size(); i++) {
				if (figure_array_[i]->Intersected(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry())) chosed_figure_array_.push_back(i);
			}
		}
		draw_status_ = false;
		point_array_.SetSize(0);
	}
	current_figure_ = NULL;

}

void ViewWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.begin(this);
	int num = figure_array_.size(); 
	for (size_t i = 0; i < num; i++){
		figure_array_[i]->Draw(painter);
	}
	num = chosed_figure_array_.size();
	for (size_t i = 0; i < num; i++) {
		figure_array_[chosed_figure_array_[i]]->Draw(painter, true);
	}

	QPen pen;  // creates a default pen
	pen.setWidth(current_size);
	pen.setBrush(current_color);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	if (draw_status_){
		switch (figure_type_){
		case kDefault:
			break;
		case kLine:
			painter.drawLine(start_point_, end_point_);
			break;
		case kRectangle:
			painter.drawRect(start_point_.rx(), start_point_.ry(), end_point_.rx() - start_point_.rx(), end_point_.ry() - start_point_.ry());
			break;
		case kEllipse:
			painter.drawEllipse(start_point_.rx(), start_point_.ry(), end_point_.rx() - start_point_.rx(), end_point_.ry() - start_point_.ry());
			break;
		case kPolygon:
			painter.drawPolygon(point_array_.GetFPoint(), point_array_.GetSize());
			break;
		case kFreehand:
			int num_1 = point_array_.GetSize() - 1;
			for (int i = 0; i < num_1; i++) {
				painter.drawLine(point_array_.GetAt(i), point_array_.GetAt(i+1));
			}
			break;
		}
		if (chose_mod_) {
			pen.setWidth(1);
			pen.setBrush(Qt::black);
			pen.setCapStyle(Qt::RoundCap);
			pen.setStyle(Qt::DashLine);
			pen.setJoinStyle(Qt::RoundJoin);
			painter.setPen(pen);
			QBrush brush(QColor(160, 200, 240, 30));
			painter.setBrush(brush);
			painter.drawRect(start_point_.rx(), start_point_.ry(), end_point_.rx() - start_point_.rx(), end_point_.ry() - start_point_.ry());
		}
	}

	update();
}

QColor ViewWidget::getColor()
{
	return current_color;
}

void ViewWidget::set_figure_type_to_line()
{
	chosed_figure_array_.clear();
	chose_mod_ = false;
	figure_type_ = kLine;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(false);
}

void ViewWidget::set_figure_type_to_rectangle()
{
	chosed_figure_array_.clear();
	chose_mod_ = false;
	figure_type_ = kRectangle;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(false);
}

void ViewWidget::set_figure_type_to_ellipse()
{
	chosed_figure_array_.clear();
	chose_mod_ = false;
	figure_type_ = kEllipse;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(false);
}

void ViewWidget::set_figure_type_to_polygon()
{
	chosed_figure_array_.clear();
	chose_mod_ = false;
	figure_type_ = kPolygon;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(true);
}

void ViewWidget::set_figure_type_to_freehand()
{
	chosed_figure_array_.clear();
	chose_mod_ = false;
	figure_type_ = kFreehand;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(false);
}

void ViewWidget::undo_current_figure()
{
	chosed_figure_array_.clear();
	chose_mod_ = false;
	figure_type_ = kDefault;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(false);
	if (figure_array_.size() > 0) {
		Figure* p = figure_array_.back();
		figure_array_.back() = NULL;
		figure_array_.pop_back();
		delete p;
	}

}

void ViewWidget::delete_chosed_figure()
{
	chose_mod_ = true;
	figure_type_ = kDefault;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(false);
	while (chosed_figure_array_.size() > 0) {
		int idx = chosed_figure_array_.back();
		Figure* p = figure_array_[idx];
		figure_array_[idx] = NULL;
		delete p;
		figure_array_.erase(figure_array_.begin() + idx);
		chosed_figure_array_.pop_back();
	}
	chosed_figure_array_.clear();
}

void ViewWidget::activate_chose_mod()
{
	chosed_figure_array_.clear();
	chose_mod_ = true;
	figure_type_ = kDefault;
	draw_status_ = false;
	point_array_.SetSize(0);
	setMouseTracking(false);
}

void ViewWidget::showColor()
{
	QColor c = QColorDialog::getColor(current_color);
	current_color = c;
	int num = chosed_figure_array_.size();
	for (size_t i = 0; i < num; i++) {
		figure_array_[chosed_figure_array_[i]]->SetColor(current_color);
	}
}

void ViewWidget::setSize(int s)
{
	current_size = s;
	int num = chosed_figure_array_.size();
	for (size_t i = 0; i < num; i++) {
		figure_array_[chosed_figure_array_[i]]->SetSize(current_size);
	}
}
