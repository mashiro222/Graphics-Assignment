#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"

using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	image_ = new cv::Mat();
	image_backup_ = new cv::Mat();
	domain_ = nullptr;
	clone_ = nullptr;

	draw_status_ = kNone;
	clone_status_ = kDefault;
	choose_status_ = kRect;
	is_choosing_ = false;
	is_pasting_ = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = NULL;
	setMouseTracking(true);
}

ImageWidget::~ImageWidget(void)
{
	if (nullptr != image_)
		delete image_;
	if (nullptr != image_backup_)
		delete image_backup_;
	if (nullptr != domain_)
		delete domain_;
}

int ImageWidget::ImageWidth()
{
	return image_->cols;
}

int ImageWidget::ImageHeight()
{
	return image_->rows;
}

void ImageWidget::set_draw_status_to_choose()
{
	InitChoose();
	draw_status_ = kChoose;	
}

void ImageWidget::set_draw_status_to_paste()
{
	InitChoose();
	draw_status_ = kPaste;
}

void ImageWidget::set_clone_status_to_default()
{
	InitChoose();
	clone_status_ = kDefault;
}

void ImageWidget::set_clone_status_to_seamless()
{
	InitChoose();
	clone_status_ = kSeamless;
}

void ImageWidget::set_clone_status_to_mixed()
{
	InitChoose();
	clone_status_ = kMixed;
}

void ImageWidget::set_choose_status_to_freehand()
{
	InitChoose();
	choose_status_ = kFreehand;
}

void ImageWidget::set_choose_status_to_rect()
{
	InitChoose();
	choose_status_ = kRect;
}

void ImageWidget::set_choose_status_to_polygon()
{
	InitChoose();
	choose_status_ = kPolygon;
}

cv::Mat* ImageWidget::image()
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
	QImage image_show = QImage((unsigned char*)(image_->data), image_->cols, image_->rows, image_->step, QImage::Format_RGB888);
	QRect rect = QRect(0, 0, image_show.width(), image_show.height());
	painter.drawImage(rect, image_show);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	switch (choose_status_)
	{
	case kRect:
		painter.drawRect(point_start_.x(), point_start_.y(),
			point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());
		break;
	case kFreehand:
	case kPolygon:
		if (vertice_list_.empty())
			break;
		for (size_t i = 0; i < vertice_list_.size() - 1; i++)
		{
			painter.drawLine(vertice_list_[i].x, vertice_list_[i].y, vertice_list_[i + 1].x, vertice_list_[i + 1].y);
		}
		if (is_choosing_)
		{
			painter.drawLine(point_start_.x(), point_start_.y(), point_end_.x(), point_end_.y());
		}
		else
		{
			painter.drawLine(vertice_list_[vertice_list_.size() - 1].x, vertice_list_[vertice_list_.size() - 1].y, vertice_list_[0].x, vertice_list_[0].y);
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
			switch (choose_status_)
			{
			case kRect:
				is_choosing_ = true;
				point_start_ = point_end_ = mouseevent->pos();
				break;
			case kFreehand:
			case kPolygon:
				point_start_ = point_end_ = mouseevent->pos();
				vertice_list_.push_back(cv::Point(mouseevent->pos().x(), mouseevent->pos().y()));
				is_choosing_ = true;
				break;
			default:
				break;
			}
			break;

		case kPaste:
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			switch (source_window_->imagewidget_->choose_status_)
			{
			case kRect:
				domain_ = new RectDomain(cv::Point(source_window_->imagewidget_->point_start_.rx(), source_window_->imagewidget_->point_start_.ry()), 
					cv::Point(source_window_->imagewidget_->point_end_.rx(), source_window_->imagewidget_->point_end_.ry()));
				break;
			case kFreehand:
			case kPolygon:
				domain_ = new PolyDomain(source_window_->imagewidget_->vertice_list_);
				break;
			default:
				break;
			}
			
			switch (clone_status_)
			{
			case kDefault:
				clone_ = new NormalCloning();
				clone_->Clone(domain_, cv::Point(xpos, ypos), source_window_->imagewidget_->image(), image_);
				break;
			case kSeamless:
				clone_ = new SeamlessCloning(domain_);
				clone_->Clone(domain_, cv::Point(xpos, ypos), source_window_->imagewidget_->image(), image_);
				break;
			case kMixed:
				clone_ = new MixedCloning(domain_);
				clone_->Clone(domain_, cv::Point(xpos, ypos), source_window_->imagewidget_->image(), image_);
				break;
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
		// End polygon choosing
		if (is_choosing_ && kPolygon == choose_status_)
		{
			point_end_ = mouseevent->pos();
			is_choosing_ = false;
			draw_status_ = kNone;
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			if (choose_status_ == kFreehand)
			{
				point_start_ = point_end_ = mouseevent->pos();
				vertice_list_.push_back(cv::Point(mouseevent->pos().x(), mouseevent->pos().y()));
			}
		}
		break;

	case kPaste:
		// Update while moving
		if (is_pasting_)
		{
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			*image_ = image_backup_->clone();
			clone_->Clone(domain_, cv::Point(xpos, ypos), source_window_->imagewidget_->image(), image_);
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
	case kChoose:
		if (is_choosing_)
		{
			switch (choose_status_)
			{
			case kRect:
				point_end_ = mouseevent->pos();
				is_choosing_ = false;
				draw_status_ = kNone;
				break;
			case kPolygon:
				point_end_ = mouseevent->pos();
				break;
			case kFreehand:
				point_end_ = mouseevent->pos();
				is_choosing_ = false;
				draw_status_ = kNone;
				break;
			default:
				break;
			}
		}

	case kPaste:
		if (is_pasting_)
		{
			if (nullptr != domain_)
				delete domain_;
			if (nullptr != clone_)
				delete clone_;
			domain_ = nullptr;
			clone_ = nullptr;
			is_pasting_ = false;
			draw_status_ = kNone;
		}

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
		*image_ = cv::imread(filename.toLatin1().data());
		cv::cvtColor(*image_, *image_, cv::COLOR_BGR2RGB);
		*image_backup_ = image_->clone();
	}

	//	setFixedSize(image_->width(), image_->height());
	//	relate_window_->setWindowFlags(Qt::Dialog);
	//	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
	//	relate_window_->setWindowFlags(Qt::SubWindow);

		//image_->invertPixels(QImage::InvertRgb);
		//*(image_) = image_->mirrored(true, true);
		//*(image_) = image_->rgbSwapped();
	cout << "image size: " << image_->cols << ' ' << image_->rows << endl;
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
	cv::cvtColor(*image_, image_save, cv::COLOR_BGR2RGB);
	cv::imwrite(filename.toLatin1().data(), image_save);
}

void ImageWidget::Invert()
{
	if (image_->empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_->begin<cv::Vec3b>(), iterend = image_->end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		(*iter)[0] = 255 - (*iter)[0];
		(*iter)[1] = 255 - (*iter)[1];
		(*iter)[2] = 255 - (*iter)[2];
	}

	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if (image_->empty())
		return;
	int width = image_->cols;
	int height = image_->rows;

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->at<cv::Vec3b>(j, i) = image_backup_->at<cv::Vec3b>(height - 1 - j, width - 1 - i);
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->at<cv::Vec3b>(j, i) = image_backup_->at<cv::Vec3b>(j, width - 1 - i);
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
					image_->at<cv::Vec3b>(j, i) = image_backup_->at<cv::Vec3b>(height - 1 - j, i);
				}
			}
		}
	}

	update();
}

void ImageWidget::TurnGray()
{
	if (image_->empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_->begin<cv::Vec3b>(), iterend = image_->end<cv::Vec3b>(); iter != iterend; ++iter)
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
	InitChoose();
	*image_ = image_backup_->clone();
	update();
}

void ImageWidget::InitChoose()
{
	is_choosing_ = false;
	point_start_ = point_end_ = QPoint(0, 0);
	vertice_list_.clear();
}