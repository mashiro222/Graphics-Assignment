#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"
#include <Eigen\Sparse>
#include <Eigen\Dense>
#include <vector>
#include "PoissonEditing.h"

typedef SparseLU<SparseMatrix<double>> Solver;
using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	//image_ = new QImage();
	//image_backup_ = new QImage();

	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;
	mixed = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = NULL;
	coeff_Matrix = NULL;
	shape_ = NULL;
}

ImageWidget::~ImageWidget(void)
{
	if (shape_)
	{
		delete shape_;
		shape_ = NULL;
	}
	if (coeff_Matrix)
	{
		delete coeff_Matrix;
		coeff_Matrix= NULL;
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

void ImageWidget::set_draw_status_to_chooseRect()
{
	draw_status_ = kRect;
	if (shape_ != NULL)
	{
		delete shape_;
		shape_ = NULL;
	}
}

void ImageWidget::set_draw_status_to_chooseFreehand()
{
	draw_status_ = kFreehand;
	if (shape_ != NULL)
	{
		delete shape_;
		shape_ = NULL;
	}
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = kPaste;
	mixed = false;
}

void ImageWidget::set_draw_status_to_paste_mixed()
{
	draw_status_ = kPaste;
	mixed = true;
}

void ImageWidget::set_Copy()
{
	if (coeff_Matrix) delete coeff_Matrix;
	if (shape_->points.empty()) {
		//at first get the rectangle boundary
		int x1, x2, y1, y2;
		x1 = point_start_.rx();
		x2 = point_end_.rx();
		y1 = point_start_.ry();
		y2 = point_end_.ry();
		//x1 = 93;
		//x2 = 97;
		//y1 = 53;
		//y2 = 57;
		//cout << x1 << " " << x2 << " " << y1 << " " << y2 << std::endl;
		if (x1 > x2) {
			int temp; temp = x1; x1 = x2; x2 = temp;
		}
		if (y1 > y2) {
			int temp; temp = y1; y1 = y2; y2 = temp;
		}
		base_x = x1;
		base_y = y1;
		w_selected = x2 - x1 + 1;
		h_selected = y2 - y1 + 1;
		selected_area = Eigen::MatrixXd::Ones(y2 - y1 + 1, x2 - x1 + 1);
		//pre decomposition
		PoissonEditing p;
		coeff_Matrix = p.Get_coeff_Matrix(selected_area);
		cout << "copy finished!" << std::endl;
		return;
	}
	ScanLine scanline;
	int x1, x2, y1, y2;
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
	selected_area = scanline.Scanner2Mat(y2 - y1 + 1, x2 - x1 + 1, x_point, y_point);
	base_x = x1;
	base_y = y1;
	w_selected = x2 - x1 + 1;
	h_selected = y2 - y1 + 1;
	//pre decomposition
	PoissonEditing p;
	coeff_Matrix = p.Get_coeff_Matrix(selected_area);
	cout << "copy finished!" << std::endl;
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
	QRect rect = QRect(0, 0, image_mat_.cols, image_mat_.rows);
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
			cout << point_start_.rx() << point_start_.ry() << std::endl;
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
			is_pasting_ = true;
			
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();
			//int xpos = 158;
			//int ypos = 226;

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->base_x;
			int ysourcepos = source_window_->imagewidget_->base_y;

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->w_selected;
			int h = source_window_->imagewidget_->h_selected;
			
			//std::cout << xpos << " " << ypos << " " << xsourcepos << " " << ysourcepos << " " << w << " " << h << "\n" << std::endl;

			//最坑的地方，竖着的是y轴，也就是列是y
			//相当于矩阵的i行j列其实是正常认知中的转置
			//这里真的相当坑，由于行列和xy轴的不同，矩阵的赋值要非常小心
	
			// Paste
			if (xpos + w >= width() || ypos + h >= height()) {
				QMessageBox::about(this, tr("Error"), tr("Cannot put it here !!!"));
				return;
			}
			//initialize
			MatrixXd source_selected_area = source_window_->imagewidget_->selected_area;
			MatrixXd f, g;
			Solver* source_coeff_Matrix = source_window_->imagewidget_->coeff_Matrix;
			PoissonEditing p;
			
			//paste
			for (int k = 0; k < 3; k++) {
				f = MatrixXd::Zero(h, w);
				g = MatrixXd::Zero(h, w);
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						f(i, j) = image_mat_.at<cv::Vec3b>(i + ypos, j + xpos)[k];
						g(i, j) = source_window_->imagewidget_->image_mat_.at<cv::Vec3b>(i + ysourcepos, j + xsourcepos)[k];
					}
				}
				cout << "f and g finished!" << std::endl;
				//cout << "f" << f << std::endl;
				//cout << "g" << g << std::endl;

				VectorXd result = p.Solve_Poisson(f, g, source_selected_area, source_coeff_Matrix, mixed);
				//VectorXd result = p.Poisson_solve(f, g, source_selected_area);
				cout << "result finished!" << std::endl;

				//re evaluation
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						double temp = result(i * w + j);
						if (temp > 255) temp = 255;
						if (temp < 0) temp = 0;
						image_mat_.at<cv::Vec3b>(i + ypos, j + xpos)[k] = floor(temp);
					}
				}
				cout << "k = " << k << " finished!" << std::endl;
			}
		}

		update();
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
	case kFreehand:
		// Store point position for freehand region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->points.push_back(point_end_);
		}
		break;

	case kPaste:
		if (is_pasting_) {
			is_pasting_ = true;

			image_mat_ = image_mat_backup_.clone();

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();
			//int xpos = 158;
			//int ypos = 226;

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->base_x;
			int ysourcepos = source_window_->imagewidget_->base_y;

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->w_selected;
			int h = source_window_->imagewidget_->h_selected;

			//std::cout << xpos << " " << ypos << " " << xsourcepos << " " << ysourcepos << " " << w << " " << h << "\n" << std::endl;

			//最坑的地方，竖着的是y轴，也就是列是y
			//相当于矩阵的i行j列其实是正常认知中的转置
			//这里真的相当坑，由于行列和xy轴的不同，矩阵的赋值要非常小心

			// Paste
			if (xpos + w >= width() || ypos + h >= height()) {
				QMessageBox::about(this, tr("Error"), tr("Cannot put it here !!!"));
				return;
			}
			//initialize
			MatrixXd source_selected_area = source_window_->imagewidget_->selected_area;
			MatrixXd f, g;
			Solver* source_coeff_Matrix = source_window_->imagewidget_->coeff_Matrix;
			PoissonEditing p;

			//paste
			for (int k = 0; k < 3; k++) {
				f = MatrixXd::Zero(h, w);
				g = MatrixXd::Zero(h, w);
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						f(i, j) = image_mat_.at<cv::Vec3b>(i + ypos, j + xpos)[k];
						g(i, j) = source_window_->imagewidget_->image_mat_.at<cv::Vec3b>(i + ysourcepos, j + xsourcepos)[k];
					}
				}

				VectorXd result = p.Solve_Poisson(f, g, source_selected_area, source_coeff_Matrix, mixed);

				//re evaluation
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						double temp = result(i * w + j);
						if (temp > 255) temp = 255;
						if (temp < 0) temp = 0;
						image_mat_.at<cv::Vec3b>(i + ypos, j + xpos)[k] = floor(temp);
					}
				}
			}
		}
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
	cout << "image size: " << image_mat_.rows << ' ' << image_mat_.cols << std::endl;
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
	update();
}
