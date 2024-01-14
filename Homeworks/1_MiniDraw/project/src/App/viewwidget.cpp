#include "viewwidget.h"
#include <sstream>
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include <vector>
#include <map>
#include "clipper2/clipper.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false;
	shape_ = nullptr;
	mouse_pos_visible = false;
	type_ = Shape::kDefault;
	pen = QPen(QColor(0, 0, 0), 1);
	p_canvas = QPixmap(1920, 1080);

	ptr_image = new QImage();

}

ViewWidget::~ViewWidget()
{
	for (int i = 0; i < shape_list_.size(); i++)
	{
		delete shape_list_[i];
	}
	//for (int i = 0; i < shape_filled_.size(); i++)
	//{
	//	delete shape_filled_[i];
	//}

}

void ViewWidget::setModeLine()
{
	editing_mode = kDrawing;
	type_ = Shape::kLine;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModeRect()
{
	editing_mode = kDrawing;
	type_ = Shape::kRect;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModeEllipse()
{
	editing_mode = kDrawing;
	type_ = Shape::kEllipse;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModePolygon()
{
	editing_mode = kDrawing;
	type_ = Shape::kPolygon;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModeFreehand()
{
	editing_mode = kDrawing;
	type_ = Shape::kFreehand;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}
void ViewWidget::setModeBezier()
{
	editing_mode = kDrawing;
	type_ = Shape::kBezierCurve;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}
void ViewWidget::setModeBSpline()
{
	editing_mode = kDrawing;
	type_ = Shape::kBSpline;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}
void ViewWidget::setModeBiBspline()
{
	editing_mode = kDrawing;
	type_ = Shape::kBiBspline;
	if (shape_ != nullptr && draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}
void ViewWidget::setFilled(QBrush abrush)
{
	brush = abrush;
	if (shape_ != nullptr)
	{
		shape_->brush = brush;
		repaint();
	}
}

void ViewWidget::AddShapetoList()
{
	draw_status_ = false;
	shape_list_.push_back(shape_);
	shape_ = nullptr;
}

void ViewWidget::setUndo()
{
	//删除最新绘制的图形
	if (shape_list_.size() > 0) {
		delete shape_list_.back();
		shape_list_.pop_back();
	}
	if (shape_filled_.size() > 0) {
		//delete shape_filled_.back();
		shape_filled_.pop_back();
		fill_mode.pop_back();
	}
	update();
}

void ViewWidget::DoDelete()
{
	switch (editing_mode)
	{
	case kDrawing:
		draw_status_ = false;
		delete shape_;
		shape_ = nullptr;
		break;
	case kSelecting:

		break;
	case kSelected:
		shape_list_.erase(shape_list_.begin() + selected_shape_index);
		shape_ = nullptr;
		editing_mode = kSelecting;
		selected_shape_index = -1;
		break;
	default:
		break;
	}
	repaint();
}

void ViewWidget::setModeSelect()
{
	if (editing_mode == kDrawing && draw_status_)
	{
		AddShapetoList();
	}
	editing_mode = kSelecting;
	selected_shape_index = -1;
	repaint();
}

void ViewWidget::setPenWidth(int a_pen_width)
{
	pen.setWidth(a_pen_width);
	if (shape_ != nullptr)
	{
		shape_->pen.setWidth(a_pen_width);
	}
	repaint();
}
void ViewWidget::setPenColor(QColor a_color)
{
	pen.setColor(a_color);
	if (shape_ != nullptr)
	{
		shape_->pen.setColor(a_color);
	}
	repaint();
}
void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	start_point_ = last_point_ = end_point_ = event->pos();
	switch (editing_mode)
	{
		//绘制新图形模式
	case kDrawing:
		switch (type_)
		{
			//创建一个新的图形
		case Shape::kLine:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new Line(start_point_, end_point_ + QPoint(1, 1));
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case Shape::kRect:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new Rect(start_point_, end_point_ + QPoint(1, 1));
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case Shape::kEllipse:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new Ellip(start_point_, end_point_ + QPoint(1, 1));
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case Shape::kFreehand:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new Freehand(start_point_);
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case Shape::kPolygon:
		{
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new Poly();
				Poly* ppolygon = (Poly*)shape_;
				ppolygon->AppendPoint(start_point_);
				ppolygon->AppendPoint(end_point_);
				shape_->pen = pen;
				draw_status_ = true;
			}
			else if (draw_status_)
			{
				if (Qt::LeftButton == event->button())
				{
					Poly* ppolygon = (Poly*)shape_;
					ppolygon->AppendPoint(start_point_);
				}
				else if (Qt::RightButton == event->button())
				{
					AddShapetoList();
				}
			}
			break;
		}
		case Shape::kBezierCurve:
		{
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new BezierCurve();
				BezierCurve* pbezier = (BezierCurve*)shape_;
				pbezier->AppendPoint(start_point_);
				pbezier->AppendPoint(end_point_);
				shape_->pen = pen;
				draw_status_ = true;
			}
			else if (draw_status_)
			{
				if (Qt::LeftButton == event->button())
				{
					BezierCurve* pbezier = (BezierCurve*)shape_;
					pbezier->AppendPoint(start_point_);
				}
				else if (Qt::RightButton == event->button())
				{
					AddShapetoList();
				}
			}
			break;
		}
		case Shape::kBSpline:
		{
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new BSpline();
				BSpline* pbspline = (BSpline*)shape_;
				pbspline->AppendPoint(start_point_);
				pbspline->AppendPoint(end_point_);
				shape_->pen = pen;
				draw_status_ = true;
			}
			else if (draw_status_)
			{
				if (Qt::LeftButton == event->button())
				{
					BSpline* pbspline = (BSpline*)shape_;
					pbspline->AppendPoint(start_point_);
				}
				else if (Qt::RightButton == event->button())
				{
					AddShapetoList();
				}
			}
			break;
		}
		case Shape::kBiBspline:
		{
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new BiBspline();
				BiBspline* pbspline = (BiBspline*)shape_;
				pbspline->AppendPoint(start_point_);
				pbspline->AppendPoint(end_point_);
				shape_->pen = pen;
				draw_status_ = true;
			}
			else if (draw_status_)
			{
				if (Qt::LeftButton == event->button())
				{
					BiBspline* pbspline = (BiBspline*)shape_;
					pbspline->AppendPoint(start_point_);
				}
				else if (Qt::RightButton == event->button())
				{
					AddShapetoList();
				}
			}
			break;
		}
		}
		break;
		//鼠标选择模式
	case kSelecting:

		break;

		//选中模式
	case kSelected:
		drag_point_index = shape_->CheckDragPoint(start_point_);
		if (drag_point_index == -1)
		{
			if (shape_->CheckSelect(start_point_))
			{//设置整体移动模式
				drag_point_index = -2;
			}
			else
			{//点击其他地方，失去选中
				editing_mode = kSelecting;
				shape_ = nullptr;
				selected_shape_index = -1;
			}
		}
		break;
	}
	mouse_pos_visible = true;

	//shape_filled = shape_;//选中时加入填充shape

	repaint();
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{

	end_point_ = event->pos();

	//分三种模式
	switch (editing_mode)
	{
		//绘制新图形模式
	case kDrawing:
		if (draw_status_ && shape_ != nullptr)
		{
			//分不同图形
			switch (shape_->type)
			{
			case Shape::kLine:
			{
				Line* pline = (Line*)shape_;
				pline->SetStartPoint(start_point_);
				pline->SetEndPoint(end_point_);
				break;
			}
			case Shape::kRect:
			{
				Rect* prect = (Rect*)shape_;
				prect->SetRect(start_point_, end_point_);
				break;
			}
			case Shape::kEllipse:
			{
				Ellip* pellipse = (Ellip*)shape_;
				pellipse->SetEllipse(start_point_, end_point_);
				break;
			}
			case Shape::kPolygon:
			{
				Poly* ppolygon = (Poly*)shape_;
				ppolygon->SetPoint(ppolygon->SizeofPoints() - 1, end_point_);
				break;
			}
			case Shape::kBezierCurve:
			{
				BezierCurve* pbezier = (BezierCurve*)shape_;
				pbezier->SetPoint(pbezier->SizeofPoints() - 1, end_point_);
				break;
			}
			case Shape::kBSpline:
			{
				BSpline* pbspline = (BSpline*)shape_;
				pbspline->SetPoint(pbspline->SizeofPoints() - 1, end_point_);
				break;
			}
			case Shape::kBiBspline:
			{
				BiBspline* pbspline = (BiBspline*)shape_;
				pbspline->SetPoint(pbspline->SizeofPoints() - 1, end_point_);
				break;
			}
			case Shape::kFreehand:
			{
				Freehand* pfreehand = (Freehand*)shape_;
				pfreehand->AppendPoint(end_point_);
				break;
			}
			default:

				break;
			}
		}
		break;
		//鼠标选择模式
	case kSelecting:

		break;
		//选中模式
	case kSelected:
		if (selected_shape_index >= 0)
		{
			if (drag_point_index >= 0)
			{
				shape_->SetPoint(drag_point_index, end_point_);
			}
			else if (drag_point_index == -2)
			{//整体拖动
				shape_->MoveBy(end_point_ - last_point_);
			}
		}
		break;
	default:
		break;
	}
	last_point_ = end_point_;

	repaint();
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	end_point_ = event->pos();
	switch (editing_mode)
	{
	case kDrawing:
		if (draw_status_ && shape_ != nullptr)
		{
			switch (shape_->type)
			{
			case Shape::kPolygon:
				if (Qt::LeftButton == event->button())
				{
					Poly* ppolygon = (Poly*)shape_;
					ppolygon->SetPoint(ppolygon->SizeofPoints() - 1, end_point_);
				}
				break;
			case Shape::kBezierCurve:
				if (Qt::LeftButton == event->button())
				{
					BezierCurve* pbezier = (BezierCurve*)shape_;
					pbezier->SetPoint(pbezier->SizeofPoints() - 1, end_point_);
				}
				break;
			case Shape::kBSpline:
				if (Qt::LeftButton == event->button())
				{
					BSpline* pbspline = (BSpline*)shape_;
					pbspline->SetPoint(pbspline->SizeofPoints() - 1, end_point_);
				}
				break;
			case Shape::kBiBspline:
				if (Qt::LeftButton == event->button())
				{
					BiBspline* pbspline = (BiBspline*)shape_;
					pbspline->SetPoint(pbspline->SizeofPoints() - 1, end_point_);
				}
				break;

			default:
				AddShapetoList();
				break;
			}

		}
		break;
	case kSelecting:
		for (int i = 0; i < shape_list_.size(); i++)
		{
			if (shape_list_[i]->CheckSelect(start_point_) && shape_list_[i]->CheckSelect(end_point_))
			{
				selected_shape_index = i;
				shape_ = shape_list_[selected_shape_index];
				editing_mode = kSelected;
				break;
			}
		}

		//shape_filled_ = shape_;

		break;
	case kSelected:

		break;
	default:
		break;
	}
	mouse_pos_visible = false;

	repaint();
}

void ViewWidget::DoParallel() {
	if (!shape_list_.empty()) {
		//默认最后一个是BiBspline类型的shape
		shape_filled_.push_back(shape_list_.back());
		std::cout << "success!" << std::endl;
	}
	else {
		std::cout << "the list is empty!" << std::endl;
	}
	fill_mode.push_back(kParallel);
}

void ViewWidget::DoFermat() {
	if (!shape_list_.empty()) {
		//默认最后一个是BiBspline类型的shape
		shape_filled_.push_back(shape_list_.back());
		std::cout << "success!" << std::endl;
	}
	else {
		std::cout << "the list is empty!" << std::endl;
	}
	fill_mode.push_back(kFermat);
}

void ViewWidget::DoZigzag() {
	if (!shape_list_.empty()) {
		//默认最后一个是BiBspline类型的shape
		shape_filled_.push_back(shape_list_.back());
		std::cout << "success!" << std::endl;
	}
	else {
		std::cout << "the list is empty!" << std::endl;
	}
	fill_mode.push_back(kZigzag);
}


void ViewWidget::paintEvent(QPaintEvent*)
{
	painter.begin(&p_canvas);
	painter.setFont(QFont("consolas"));
	painter.setRenderHint(QPainter::Antialiasing, true);
	p_canvas.fill();

	QRect rect = QRect(0, 0, ptr_image->width(), ptr_image->height());
	painter.drawImage(rect, *ptr_image);

	for (int i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}
	for (int i = 0; i < shape_filled_.size(); i++)
	{
		if (fill_mode[i] == kParallel) {
			shape_filled_[i]->FillParallel(painter);
		}
		else if (fill_mode[i] == kFermat) {
			shape_filled_[i]->FillFermat(painter);
		}
		else if (fill_mode[i] == kZigzag) {
			shape_filled_[i]->FillZigzag(painter);
		}
	
	}


	
	//绘制选中的图形
	if (shape_ != nullptr) {
		shape_->DrawSelected(painter);
	}


	//绘制提示鼠标坐标
	if (mouse_pos_visible)
	{
		QRect rec = QRect(end_point_.x() + 30, end_point_.y() + 10, 90, 22);
		painter.setPen(QPen(QColor(20, 20, 140), 2.));
		painter.setBrush(QBrush(Qt::NoBrush));
		painter.drawRect(rec);
		painter.fillRect(rec, Qt::GlobalColor::blue);
		painter.setPen(QPen(QColor(255, 255, 255), 3.));
		painter.drawText(rec, QString::number(end_point_.x()) + "," + QString::number(end_point_.y()));
	}
	painter.end();
	painter.begin(this);
	painter.drawPixmap(0, 0, p_canvas);
	painter.end();
}

void ViewWidget::Open()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	if (!fileName.isEmpty())
	{
		ptr_image->load(fileName);
	}

	std::cout << "image size: " << ptr_image->width() << std::endl;
	update();
}

void ViewWidget::DeletePtr()
{
	ptr_image = new QImage();
	update();
}

