#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"
#include <vector>

#include "Poisson.h"
#include "Rect.h"
#include "Polyg.h"
#include "Freehand.h"

#include "Line_Scanner.h"
#include <Eigen\Sparse>
#include <Eigen\Dense>
using namespace Eigen;
typedef Eigen::SparseMatrix<double> SparseMatrixType;
typedef Eigen::Triplet<double> T;
typedef Eigen::SimplicialCholesky<SparseMatrixType> Solver;

using namespace std;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	//image_ = new QImage();
	//image_backup_ = new QImage();

	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;
	copied = false;
	mixed = false;
	polygoning = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);
	shape_ = NULL;
	source_window_ = NULL;
	solver = NULL;
}

ImageWidget::~ImageWidget(void)
{
	if (shape_)
	{
		delete shape_;
		shape_ = NULL;
	}
	if (solver) 
	{
		delete solver;
		solver = NULL;
	}
}

int ImageWidget::ImageWidth()
{
	return image_mat_.cols;
}

int ImageWidget::ImageHeight()
{
	return image_mat_.rows;
}

void ImageWidget::set_draw_status_to_choose_Rect()
{
	if (polygoning)
	{
		QMessageBox::about(this, tr("Error"), tr("Finish your Polygon !!!"));
		return;
	}
	draw_status_ = kRect;	
	if (shape_ != NULL)
	{
		delete shape_;
		shape_ = NULL;
	}
}

void ImageWidget::set_draw_status_to_choose_Polygon()
{
	if (polygoning)
	{
		QMessageBox::about(this, tr("Error"), tr("Finish your Polygon !!!"));
		return;
	}

	draw_status_ = kPolygon;
	if (shape_ != NULL)
	{
		delete shape_;
		shape_ = NULL;
	}
}

void ImageWidget::set_draw_status_to_choose_Freehand()
{
	if (polygoning)
	{
		QMessageBox::about(this, tr("Error"), tr("Finish your Polygon !!!"));
		return;
	}
	draw_status_ = kFreehand;
	if (shape_ != NULL)
	{
		delete shape_;
		shape_ = NULL;
	}
}

void ImageWidget::set_draw_status_to_paste()
{
	if (polygoning)
	{
		QMessageBox::about(this, tr("Error"), tr("Finish your Polygon !!!"));
		return;
	}
	image_mat_undo_ = image_mat_.clone();
	draw_status_ = kPaste;
	setMouseTracking(true);
	mixed = false;
}

void ImageWidget::set_draw_status_to_paste_mix()
{
	if (polygoning)
	{
		QMessageBox::about(this, tr("Error"), tr("Finish your Polygon !!!"));
		return;
	}
	image_mat_undo_ = image_mat_.clone();			// for dragging
	draw_status_ = kPaste;
	setMouseTracking(true);
	mixed = true;
}

void ImageWidget::set_Copy()
{
	if (polygoning)
	{
		QMessageBox::about(this, tr("Error"), tr("Finish your Polygon !!!"));
		return;
	}
	if (shape_ == NULL)
	{
		QMessageBox::about(this, tr("Error"), tr("Choose first !!!"));
		return;
	}
	if (solver)
	{
		delete solver;
	}
	int x1, x2, y1, y2;
	if (shape_->points.empty())	
	{
		// get the rectangle boundary for choosing
		x1 = point_start_.rx();
		x2 = point_end_.rx();
		y1 = point_start_.ry();
		y2 = point_end_.ry();
		if (x1 > x2)
		{
			int tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		if (y1 > y2)
		{
			int tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		// set the selected
		selected = MatrixXd::Zero(y2 - y1 + 1, x2 - x1 + 1); 
		for (int i = 1; i < y2 - y1; i++) {
			for (int j = 1; j < x2 - x1; j++) {
				selected(i, j) = 1;
			}
		}
	}
	else
	{
		Line_Scanner line_scanner;
		x1 = x2 = shape_->points[0].rx();
		y1 = y2 = shape_->points[0].ry();
		vector<int>x_point;
		vector<int>y_point;

		// get the rectangle boundary for choosing 
		for (int k = 0; k < shape_->points.size(); k++)
		{
			if (shape_->points[k].rx() < x1)x1 = shape_->points[k].rx();
			if (shape_->points[k].rx() > x2)x2 = shape_->points[k].rx();
			if (shape_->points[k].ry() < y1)y1 = shape_->points[k].ry();
			if (shape_->points[k].ry() > y2)y2 = shape_->points[k].ry();
		}
		for (int k = 0; k < shape_->points.size(); k++)
		{
			x_point.push_back(shape_->points[k].rx() - x1);
			y_point.push_back(shape_->points[k].ry() - y1);
		}
		// set the selected (by line scanner)
		selected = line_scanner.Scanner2Mat(y2 - y1 + 1, x2 - x1 + 1, x_point, y_point);
	}
	cout <<"select : "<< y2 - y1 + 1 << "      " << x2 - x1 + 1 << endl;
	pos_x = x1;
	pos_y = y1;
	width_select = x2 - x1 + 1;
	height_select = y2 - y1 + 1;
	Poisson poisson;
	// get the solver for copy
	solver = poisson.Get_Poisson_solver(selected);
	copied = true;
}

cv::Mat ImageWidget::image()
{
	return image_mat_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QImage image_show = QImage((unsigned char*)(image_mat_.data), (int)image_mat_.cols, (int)image_mat_.rows, image_mat_.step, QImage::Format_RGB888);
	QRect rect = QRect(0, 0, image_show.width(), image_show.height());
	painter.drawImage(rect, image_show);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	if (shape_ != NULL)
	{
		shape_->Draw(painter);
	}

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case kRect:
			if (shape_ != NULL)
			{
				delete shape_;
			}
			is_choosing_ = true;
			shape_ = new Rect();
			point_start_ = point_end_ = mouseevent->pos();
			shape_->set_start(point_start_);
			break;

		case kPolygon:
			if (shape_ == NULL) 
			{
				point_start_ = point_end_ = mouseevent->pos();
				shape_ = new Polyg();
				polygoning = true;
				is_choosing_ = true;
				shape_->set_start(point_start_);
				shape_->set_end(point_end_);
				shape_->points.push_back(point_start_);
			}
			else
			{
				point_end_ = mouseevent->pos();
				shape_->set_start(point_start_);
				shape_->set_end(point_end_);
				shape_->points.push_back(point_end_);
			}
			setMouseTracking(true);
			break;

		case kFreehand:
			if (shape_ != NULL)
			{
				delete shape_;
			}
			is_choosing_ = true; 
			point_start_ = point_end_ = mouseevent->pos();
			shape_ = new Freehand();
			shape_->points.push_back(point_start_);
			break;

		case kPaste:
		{
			// Paste region to object image
			is_pasting_ = true;

			if (source_window_ == NULL || !source_window_->imagewidget_->copied) {
				QMessageBox::about(this, tr("Error"), tr("Copy first !!!"));
				setMouseTracking(false);
				return;
			}

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->pos_x;
			int ysourcepos = source_window_->imagewidget_->pos_y;
			cout << source_window_->imagewidget_->height() << " " << source_window_->imagewidget_->width() << endl;

			// Width and Height of region
			int w = source_window_->imagewidget_->width_select;
			int h = source_window_->imagewidget_->height_select;

			if (xpos + w / 2 >= width())xpos = width() - w / 2 - 1;
			if (ypos + h / 2 >= height())ypos = height() - h / 2 - 1;
			if (xpos - w + w / 2 < 0)xpos = w - w / 2;
			if (ypos - h + h / 2 < 0)ypos = h - h / 2;

			if (xpos + w / 2 >= width() || ypos + h / 2 >= height() || xpos - w + w / 2 < 0 || ypos - h + h / 2 < 0)
			{
				QMessageBox::about(this, tr("Error"), tr("Cannot put it here !!!"));
				return;
			}

			MatrixXd sourse_select = source_window_->imagewidget_->selected;

			Poisson poisson;
			// get solver 
			Solver* sourse_solver = source_window_->imagewidget_->solver;
			MatrixXd Data_f, Data_g;
			// Paste
			for (int k = 0; k < 3; k++) {
				Data_f = MatrixXd::Zero(h, w);
				Data_g = MatrixXd::Zero(h, w);
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						Data_f(i, j) = image_mat_.at<cv::Vec3b>(i + ypos - h + h / 2, j + xpos - w + w / 2)[k];
						Data_g(i, j) = source_window_->imagewidget_->image().at<cv::Vec3b>(i + ysourcepos, j + xsourcepos)[k];
					}
				}

				// get the solve from solver
				//VectorXd solve = poisson.Poisson_solve(Data_f, Data_g, sourse_select);
				VectorXd solve = poisson.Use_Poisson_solver(Data_f, Data_g, sourse_select, sourse_solver, mixed);
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						double tmp = solve(i * w + j);
						if (tmp > 255) tmp = 255;
						if (tmp < 0)tmp = 0;
						image_mat_.at<cv::Vec3b>(i + ypos - h + h / 2, j + xpos - w + w / 2)[k] = floor(tmp);
					}
				}
			}
		}

		update();
		break;

		default:
			break;
		}
	}
	if (Qt::RightButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case kRect:
			break;

		case kPolygon:
			shape_->points.push_back(point_end_);
			shape_->points.push_back(point_start_);
			polygoning = false;
			is_choosing_ = false;
			draw_status_ = kNone;
			break;

		default:
			break;
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kRect:
		// Store point position for rectangle region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->set_end(point_end_);
		}
		break;
	case kPolygon:
		// Store point position for polygon region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->set_end(point_end_);
		}
		break;
	case kFreehand:
		// Store point position for freehand region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->points.push_back(point_end_);
		}
		break;

	case kPaste:
		// Paste region to object image
		if (is_pasting_)
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->pos_x;
			int ysourcepos = source_window_->imagewidget_->pos_y;

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->width_select;
			int h = source_window_->imagewidget_->height_select;

			if (xpos + w / 2 >= width())xpos = width() - w / 2 - 1;
			if (ypos + h / 2 >= height())ypos = height() - h / 2 - 1;
			if (xpos - w + w / 2 < 0)xpos = w - w / 2;
			if (ypos - h + h / 2 < 0)ypos = h - h / 2;
			if (xpos + w / 2 >= width() || ypos + h / 2 >= height() || xpos - w + w / 2 < 0 || ypos - h + h / 2 < 0)
			{
				QMessageBox::about(this, tr("Error"), tr("Cannot put it here !!!"));
				return;
			}

			image_mat_ = image_mat_undo_.clone();
			MatrixXd sourse_select = source_window_->imagewidget_->selected;


			Poisson poisson;

			// get solver 
			Solver* sourse_solver = source_window_->imagewidget_->solver;
			MatrixXd Data_f, Data_g;
			// Paste
			for (int k = 0; k < 3; k++) {
				Data_f = MatrixXd::Zero(h, w);
				Data_g = MatrixXd::Zero(h, w);
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						Data_f(i, j) = image_mat_.at<cv::Vec3b>(i + ypos - h + h / 2, j + xpos - w + w / 2)[k];
						Data_g(i, j) = source_window_->imagewidget_->image().at<cv::Vec3b>(i + ysourcepos, j + xsourcepos)[k];
					}
				}


				// get the solve from solver
				//VectorXd solve = poisson.Poisson_solve(Data_f, Data_g, sourse_select);
				VectorXd solve = poisson.Use_Poisson_solver(Data_f, Data_g, sourse_select, sourse_solver, mixed);
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						double tmp = solve(i * w + j);
						if (tmp > 255) tmp = 255;
						if (tmp < 0)tmp = 0;
						image_mat_.at<cv::Vec3b>(i + ypos - h + h / 2, j + xpos - w + w / 2)[k] = floor(tmp);
					}
				}
			}
		}

	default:
		break;
	}

	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kRect:
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->set_end(point_end_);
			is_choosing_ = false;
			draw_status_ = kNone;
		}
		break;
	case kPolygon:
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->set_end(point_end_);
		}
		break;
	case kFreehand:
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->points.push_back(point_end_);
			shape_->points.push_back(point_start_);
			is_choosing_ = false;
			draw_status_ = kNone;
		}
		break;


	case kPaste:
		if (is_pasting_)
		{
			delete source_window_->imagewidget_->shape_;
			source_window_->imagewidget_->shape_ = NULL;
			is_pasting_ = false;
			draw_status_ = kNone;
		}
		break;

	default:
		break;
	}

	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		//only for bmp?
		image_mat_ = cv::imread(filename.toLatin1().data());
		cv::cvtColor(image_mat_, image_mat_, cv::COLOR_BGR2RGB);
		image_mat_backup_ = image_mat_.clone(); 
		//image_->load(filename);
		//*(image_backup_) = *(image_);
	}
	cout << "image size: " << image_mat_.rows << ' ' << image_mat_.cols << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}
	cv::Mat image_save;
	cv::cvtColor(image_mat_, image_save, cv::COLOR_BGR2RGB);
	cv::imwrite(filename.toLatin1().data(), image_save);
	//image_->save(filename);
}


void ImageWidget::Invert()
{
	if (image_mat_.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat_.begin<cv::Vec3b>(), iterend = image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		(*iter)[0] = 255 - (*iter)[0];
		(*iter)[1] = 255 - (*iter)[1];
		(*iter)[2] = 255 - (*iter)[2];
	}

	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if (image_mat_.empty())
		return;
	int width = image_mat_.cols;
	int height = image_mat_.rows;

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height - 1 - j, width - 1 - i);
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(j, width - 1 - i);
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height - 1 - j, i);
				}
			}
		}
	}

	update();
}

void ImageWidget::TurnGray()
{
	if (image_mat_.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat_.begin<cv::Vec3b>(), iterend = image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		int itmp = ((*iter)[0] + (*iter)[1] + (*iter)[2]) / 3;
		(*iter)[0] = itmp;
		(*iter)[1] = itmp;
		(*iter)[2] = itmp;
	}

	update();
}

void ImageWidget::Restore()
{
	image_mat_ = image_mat_backup_.clone();
	//*(image_) = *(image_backup_);
	//point_start_ = point_end_ = QPoint(0, 0);
	update();
}
