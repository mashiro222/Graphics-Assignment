#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <math.h>
#include <iostream>
#include <Eigen\Dense>
#include <vector>
#include "ChildWindow.h"
#include "PoissonSolver.h"
#include "ImportGrad.h"
#include "MixGrad.h"
#include "NormalCopy.h"

using namespace Eigen;
using namespace std;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	image_ = new QImage();
	image_backup_ = new QImage();

	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;
	time_count_ = 0;

	shape_ = kRect;
	mode_ = kNormal;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	point_buffer_ = vector<Vector2i>();

	source_window_ = NULL;
}

ImageWidget::~ImageWidget(void)
{
}

int ImageWidget::ImageWidth()
{
	return image_->width();
}

int ImageWidget::ImageHeight()
{
	return image_->height();
}

void ImageWidget::set_draw_status_to_choose()
{
	draw_status_ = kChoose;	
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = kPaste;
}

void ImageWidget::set_shape_to(int index)
{
	switch (index)
	{
	case 0:
		shape_ = kRect;
		break;
	case 1:
		shape_ = kEllipse;
		break;
	case 2:
		shape_ = kPolygon;
		break;
	case 3:
		shape_ = kFreehand;
		break;
	default:
		break;
	}
}

void ImageWidget::set_mode_to(int index)
{
	switch (index)
	{
	case 0:
		mode_ = kNormal;
		break;
	case 1:
		mode_ = kSeamlessCloning;
		break;
	case 2:
		mode_ = kMixingGradient;
		break;
	default:
		break;
	}
}

QImage* ImageWidget::image()
{
	return image_;
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
	QRect rect = QRect(0, 0, image_->width(), image_->height());
	painter.drawImage(rect, *image_);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);

	switch (shape_)
	{
	case kRect:
		painter.drawRect(point_start_.x(), point_start_.y(),
			point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());
		break;
	case kEllipse:
		painter.drawEllipse(point_start_.x(), point_start_.y(),
			point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());
		break;
	case kPolygon:
	case kFreehand:
		if (point_buffer_.size() != 0)
		{
			for (int i = 0; i < point_buffer_.size() - 1; i++)
			{
				painter.drawLine(point_buffer_[i](0), point_buffer_[i](1), point_buffer_[i + 1](0), point_buffer_[i + 1](1));
			}

			if (is_choosing_)
			{
				painter.drawLine(point_buffer_[point_buffer_.size() - 1](0), point_buffer_[point_buffer_.size() - 1](1),
					point_end_.x(), point_end_.y());
			}
			else
			{
				painter.drawLine(point_buffer_[point_buffer_.size() - 1](0), point_buffer_[point_buffer_.size() - 1](1),
					point_buffer_[0](0), point_buffer_[0](1));
			}
		}
		break;
	default:
		break;
	}

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case kChoose:
			if (!is_choosing_)
			{
				point_start_ = point_end_ = mouseevent->pos();
				switch (shape_)
				{
				case kPolygon:
					point_buffer_.clear();
					point_buffer_.push_back(Vector2i(mouseevent->pos().x(), mouseevent->pos().y()));
					setMouseTracking(true);
					is_choosing_ = true;
					break;
				case kFreehand:
					point_buffer_.clear();
					point_buffer_.push_back(Vector2i(mouseevent->pos().x(), mouseevent->pos().y()));
					is_choosing_ = true;
					break;
				default:
					is_choosing_ = true;
					break;
				}
			}
			break;

		case kPaste:
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			vector<Vector2i> vertices = vector<Vector2i>();
			vector<Vector2i> point_buffer = source_window_->imagewidget_->point_buffer_;
			switch (shape_)
			{
			case kRect:
				vertices.push_back(Vector2i(0, 0));
				vertices.push_back(Vector2i(w - 1, 0));
				vertices.push_back(Vector2i(w - 1, h - 1));
				vertices.push_back(Vector2i(0, h - 1));
				break;
			case kEllipse:
				for (int i = 0; i < 40; i++)
				{
					int x = (w - 1) * (0.5 + 0.5 * cos(2 * 3.1415926 * i / 40));
					int y = (h - 1) * (0.5 + 0.5 * sin(2 * 3.1415926 * i / 40));
					vertices.push_back(Vector2i(x, y));
				}
				break;
			case kPolygon:
			case kFreehand:
				if (!point_buffer.empty())
				{
					int min_x = point_buffer[0](0);
					int min_y = point_buffer[0](1);
					int max_x = point_buffer[0](0);
					int max_y = point_buffer[0](1);
					for (int i = 0; i < point_buffer.size(); i++)
					{
						if (point_buffer[i](0) < min_x)
						{
							min_x = point_buffer[i](0);
						}
						else if (point_buffer[i](0) > max_x)
						{
							max_x = point_buffer[i](0);
						}

						if (point_buffer[i](1) < min_y)
						{
							min_y = point_buffer[i](1);
						}
						else if (point_buffer[i](1) > max_y)
						{
							max_y = point_buffer[i](1);
						}
					}

					cout << min_x << ", " << min_y << ", " << max_x << ", " << max_y << ";\n";

					xsourcepos = min_x;
					ysourcepos = min_y;
					w = max_x - min_x + 1;
					h = max_y - min_y + 1;
					Vector2i subtraction = Vector2i(min_x, min_y);
					for (int i = 0; i < point_buffer.size(); i++)
					{
						vertices.push_back(point_buffer[i] - subtraction);
					}
				}
				break;
			default:
				break;
			}

			cout << "shape" << shape_ << endl;
			cout << "w: " << w << "h: " << h << endl;

			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				QProgressDialog progress_dialog("Processing", "Cancel", 0, 100);
				MatrixXd source_red(h, w);
				MatrixXd source_green(h, w);
				MatrixXd source_blue(h, w);
				MatrixXd target_red(h, w);
				MatrixXd target_green(h, w);
				MatrixXd target_blue(h, w);
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{

						QRgb source_pixel = source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j);
						source_red(j, i) = qRed(source_pixel);
						source_green(j, i) = qGreen(source_pixel);
						source_blue(j, i) = qBlue(source_pixel);

						QRgb target_pixel = image_->pixel(xpos + i, ypos + j);
						target_red(j, i) = qRed(target_pixel);
						target_green(j, i) = qGreen(target_pixel);
						target_blue(j, i) = qBlue(target_pixel);
					}

				}

				PoissonSolver solver_red;
				PoissonSolver solver_green;
				PoissonSolver solver_blue;

				switch (mode_)
				{
				case kNormal:
					progress_dialog.show();
					progress_dialog.setValue(33);
					QApplication::processEvents();
					solver_red = NormalCopy(w, h, vertices, source_red, target_red);
					progress_dialog.setValue(66);
					QApplication::processEvents();
					solver_green = NormalCopy(w, h, vertices, source_green, target_green);
					progress_dialog.setValue(99);
					QApplication::processEvents();
					solver_blue = NormalCopy(w, h, vertices, source_blue, target_blue);
					cout << "mission accomplished" << endl;
					break;
				case kSeamlessCloning:
					progress_dialog.show();
					progress_dialog.setValue(33);
					QApplication::processEvents();
					solver_red = ImportGrad(w, h, vertices, source_red, target_red);
					progress_dialog.setValue(66);
					QApplication::processEvents();
					solver_green = ImportGrad(w, h, vertices, source_green, target_green);
					progress_dialog.setValue(99);
					QApplication::processEvents();
					solver_blue = ImportGrad(w, h, vertices, source_blue, target_blue);
					cout << "mission accomplished" << endl;
					break;
				case kMixingGradient:
					progress_dialog.show();
					progress_dialog.setValue(33);
					QApplication::processEvents();
					solver_red = MixGrad(w, h, vertices, source_red, target_red);
					progress_dialog.setValue(66);
					QApplication::processEvents();
					solver_green = MixGrad(w, h, vertices, source_green, target_green);
					progress_dialog.setValue(99);
					QApplication::processEvents();
					solver_blue = MixGrad(w, h, vertices, source_blue, target_blue);
					cout << "mission accomplished" << endl;
					break;
				default:
					break;
				}

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						int red = max(0, min((int)solver_red.SolutionAt(i, j), 255));
						int green = max(0, min((int)solver_green.SolutionAt(i, j), 255));
						int blue = max(0, min((int)solver_blue.SolutionAt(i, j), 255));
						image_->setPixel(xpos + i, ypos + j, qRgb(red, green, blue));
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
	else if ((Qt::RightButton == mouseevent->button()) && (shape_ == kPolygon) && is_choosing_)
	{
		point_buffer_.push_back(Vector2i(mouseevent->pos().x(), mouseevent->pos().y()));
		setMouseTracking(false);
		is_choosing_ = false;
		draw_status_ = kNone;
	}

}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		// Store point position for rectangle region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();

			if (shape_ == kFreehand)
			{
				time_count_++;
				if (time_count_ >= 10)
				{
					point_buffer_.push_back(Vector2i(mouseevent->pos().x(), mouseevent->pos().y()));
					time_count_ = 0;
				}
			}
		}
		break;
	default:
		break;
	}
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (shape_ == kPolygon)
		{
			point_buffer_.push_back(Vector2i(point_end_.x(), point_end_.y()));
		}
		else if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			is_choosing_ = false;
			draw_status_ = kNone;
		}

	case kPaste:
		if (is_pasting_)
		{
			is_pasting_ = false;
			draw_status_ = kNone;
		}

	default:
		break;
	}

	cout << "point buffer:	" << point_buffer_.size() << endl;
	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		image_->load(filename);
		*(image_backup_) = *(image_);
	}

	//	setFixedSize(image_->width(), image_->height());
	//	relate_window_->setWindowFlags(Qt::Dialog);
	//	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
	//	relate_window_->setWindowFlags(Qt::SubWindow);

		//image_->invertPixels(QImage::InvertRgb);
		//*(image_) = image_->mirrored(true, true);
		//*(image_) = image_->rgbSwapped();
	cout << "image size: " << image_->width() << ' ' << image_->height() << endl;
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

	image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}

	// equivalent member function of class QImage
	// image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(image_));
	int width = image_->width();
	int height = image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
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
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(image_) = image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(image_) = *(image_backup_);
	point_start_ = point_end_ = QPoint(0, 0);
	point_buffer_.clear();
	update();
}
