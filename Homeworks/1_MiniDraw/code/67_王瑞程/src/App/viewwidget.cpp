#include "viewwidget.h"
#include <sstream>

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{

	ui.setupUi(this);
	draw_status_ = false;
	shape_ = nullptr;
	mouse_pos_visible = false;
	type_ = CFigure::kDefault;
	pen = QPen(QColor(0, 0, 0), 1);
	p_canvas = QPixmap(1920, 1080);


}

ViewWidget::~ViewWidget()
{
	for (int i = 0; i < shape_list_.size(); i++) 
	{
		delete shape_list_[i];
	}
}

void ViewWidget::setModeLine()
{
	editing_mode = kDrawing;
	type_ = CFigure::kLine;
	if (shape_ != nullptr&&draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModeRect()
{
	editing_mode = kDrawing;
	type_ = CFigure::kRect;
	if (shape_ != nullptr&&draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModeEllipse()
{
	editing_mode = kDrawing;
	type_ = CFigure::kEllipse;
	if (shape_ != nullptr&&draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModePolygon()
{
	editing_mode = kDrawing;
	type_ = CFigure::kPolygon;
	if (shape_ != nullptr&&draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}

void ViewWidget::setModeFreehand()
{
	editing_mode = kDrawing;
	type_ = CFigure::kFreehand;
	if (shape_ != nullptr&&draw_status_)
	{
		AddShapetoList();
	}
	shape_ = nullptr;
}
void ViewWidget::setModeBezier()
{
	editing_mode = kDrawing;
	type_ = CFigure::kBezierCurve;
	if (shape_ != nullptr&&draw_status_)
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

	operations_undo.push_back(new COpAdd);
	for (auto it = operations_redo.begin(); it != operations_redo.end(); it++)
	{
		delete (*it);
	}
	operations_redo.clear();
	shape_ = nullptr;
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
		operations_undo.push_back(new COpRemove(selected_shape_index, shape_));
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
	start_point_ = last_point_= end_point_ = event->pos();
	switch (editing_mode)
	{
	//绘制新图形模式
	case kDrawing:
		switch (type_)
		{
			//创建一个新的图形
		case CFigure::kLine:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new CLine(start_point_, end_point_ + QPoint(1, 1));
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case CFigure::kRect:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new CRect(start_point_, end_point_ + QPoint(1, 1));
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case CFigure::kEllipse:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new CEllipse(start_point_, end_point_ + QPoint(1, 1));
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case CFigure::kFreehand:
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new CFreehand(start_point_);
				shape_->pen = pen;
				shape_->brush = brush;
				draw_status_ = true;
			}
			break;
		case CFigure::kPolygon:
		{
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new CPolygon();
				CPolygon* ppolygon = (CPolygon*)shape_;
				ppolygon->AppendPoint(start_point_);
				ppolygon->AppendPoint(end_point_);
				shape_->pen = pen;
				draw_status_ = true;
			}
			else if (draw_status_)
			{
				if (Qt::LeftButton == event->button())
				{
					CPolygon* ppolygon = (CPolygon*)shape_;
					ppolygon->AppendPoint(start_point_);
				}
				else if (Qt::RightButton == event->button())
				{
					AddShapetoList();
				}
			}
			break;
		}
		case CFigure::kBezierCurve:
		{
			if (!draw_status_ && Qt::LeftButton == event->button())
			{
				shape_ = new CBezierCurve();
				CBezierCurve* pbezier = (CBezierCurve*)shape_;
				pbezier->AppendPoint(start_point_);
				pbezier->AppendPoint(end_point_);
				shape_->pen = pen;
				draw_status_ = true;
			}
			else if (draw_status_)
			{
				if (Qt::LeftButton == event->button())
				{
					CBezierCurve* pbezier = (CBezierCurve*)shape_;
					pbezier->AppendPoint(start_point_);
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
				drag_point_index = - 2;			
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
			case CFigure::kLine:
			{
				CLine* pline = (CLine*)shape_;
				pline->SetStartPoint(start_point_);
				pline->SetEndPoint(end_point_);
				break;
			}
			case CFigure::kRect:
			{
				CRect* prect = (CRect*)shape_;
				prect->SetRect(start_point_, end_point_);
				break;
			}
			case CFigure::kEllipse:
			{
				CEllipse* pellipse = (CEllipse*)shape_;
				pellipse->SetEllipse(start_point_, end_point_);
				break;
			}
			case CFigure::kPolygon:
			{
				CPolygon* ppolygon = (CPolygon*)shape_;
				ppolygon->SetPoint(ppolygon->SizeofPoints() - 1, end_point_);
				break;
			}
			case CFigure::kBezierCurve:
			{
				CBezierCurve* pbezier = (CBezierCurve*)shape_;
				pbezier->SetPoint(pbezier->SizeofPoints() - 1, end_point_);
				break;
			}
			case CFigure::kFreehand:
			{
				CFreehand* pfreehand = (CFreehand*)shape_;
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
			case CFigure::kPolygon:
				if (Qt::LeftButton == event->button())
				{
					CPolygon* ppolygon = (CPolygon*)shape_;
					ppolygon->SetPoint(ppolygon->SizeofPoints() - 1, end_point_);
				}
				break;
			case CFigure::kBezierCurve:
				if (Qt::LeftButton == event->button())
				{
					CBezierCurve* pbezier = (CBezierCurve*)shape_;
					pbezier->SetPoint(pbezier->SizeofPoints() - 1, end_point_);
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
		break;
	case kSelected:

		break;
	default:
		break;
	}
	mouse_pos_visible = false;

	repaint();
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	painter.begin(&p_canvas);
	painter.setFont(QFont("consolas"));
	painter.setRenderHint(QPainter::Antialiasing, true);
	p_canvas.fill();
	for (int i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
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
		painter.fillRect(rec,Qt::GlobalColor::blue);
		painter.setPen(QPen(QColor(255, 255, 255), 3.));
		painter.drawText(rec, QString::number(end_point_.x())+","+QString::number(end_point_.y()));
	}
	painter.end();
	painter.begin(this);
	painter.drawPixmap(0, 0, p_canvas);
	painter.end();
	//update();
}

void ViewWidget::Undo()
{
	if (shape_ != nullptr)
	{
		delete shape_;
		draw_status_ = false;
	}
	else
	{
		if (!operations_undo.empty())
		{
			operations_undo.back()->Undo(shape_list_);
			operations_redo.push_front(operations_undo.back());
			operations_undo.pop_back();
		}
	}
}

void ViewWidget::Redo()
{
	if (!operations_redo.empty())
	{
		operations_redo.front()->Redo(shape_list_);
		operations_undo.push_back(operations_redo.front());
		operations_undo.pop_front();
	}
}

ViewWidget::COpAdd::COpAdd()
{
	cache = nullptr;
}

ViewWidget::COpAdd::~COpAdd()
{
	delete cache;
}

void ViewWidget::COpAdd::Undo(std::vector<CFigure*>& a_sh_list)
{
	cache = a_sh_list[a_sh_list.size() - 1];
	a_sh_list.pop_back();
}

void ViewWidget::COpAdd::Redo(std::vector<CFigure*>& a_sh_list)
{
	a_sh_list.push_back(cache);
}

ViewWidget::COpRemove::COpRemove(int a_index, CFigure* a_figure)
{
	index = a_index;
	cache = a_figure;
}

ViewWidget::COpRemove::~COpRemove()
{
	delete cache;
}

void ViewWidget::COpRemove::Undo(std::vector<CFigure*>& a_sh_list)
{
	a_sh_list.insert(a_sh_list.begin() + index, cache);
}

void ViewWidget::COpRemove::Redo(std::vector<CFigure*>& a_sh_list)
{
	a_sh_list.erase(a_sh_list.begin() + index);
}

ViewWidget::COpChange::COpChange(int a_index, CFigure* a_figure_bef, CFigure* a_figure_aft)
{
	index = a_index;
	cache_bef = a_figure_bef;
	cache_aft = a_figure_aft;
}

ViewWidget::COpChange::~COpChange()
{
	delete cache_bef;
	delete cache_aft;
}

void ViewWidget::COpChange::Undo(std::vector<CFigure*>& a_sh_list)
{
	a_sh_list[index] = cache_bef;
}

void ViewWidget::COpChange::Redo(std::vector<CFigure*>& a_sh_list)
{
	a_sh_list[index] = cache_aft;
}
