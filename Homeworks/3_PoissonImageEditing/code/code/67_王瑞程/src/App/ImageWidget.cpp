#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ScanLine.h"

using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	draw_status_ = kNone;
	is_choosing_ = false;
	is_choosing_polyedge = false;
	is_pasting_ = false;
	is_pasting_holding = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = relatewindow;

	poisson_editing.SetMixedGrad(true);
}

ImageWidget::~ImageWidget(void)
{

}

int ImageWidget::ImageWidth()
{
	return image_mat.size().width;
}

int ImageWidget::ImageHeight()
{
	return image_mat.size().height;
}

void ImageWidget::set_draw_status(EnumDrawStatus astatus)
{
	switch (astatus)
	{
	case kChoosePolygon:
		draw_status_ = astatus;
		break;
	case kChooseFreehand:
		draw_status_ = astatus;
		break;
	case kPasteClone:
		BeginPasteClone();
		break;
	case kPasteSeamless:
		BeginPasteSeamless();
		break;
	case kNone:
		draw_status_ = astatus;
		break;
	default:
		break;
	}
}

cv::Mat ImageWidget::image()
{
	return image_mat;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

void ImageWidget::set_mixed(bool amixed)
{
	poisson_editing.SetMixedGrad(amixed);
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::NoBrush);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect(0, 0, image_mat.size().width, image_mat.size().height);
	
	QImage image_show;

	switch (draw_status_)
	{
	case kChoosePolygon:
		image_show = QImage((unsigned char *)(image_mat.data), image_mat.cols, image_mat.rows, image_mat.step, QImage::Format_RGB888);
		break;
	case kChooseFreehand:
		image_show = QImage((unsigned char *)(image_mat.data), image_mat.cols, image_mat.rows, image_mat.step, QImage::Format_RGB888);
		break;
	case kPasteClone:
		image_show = QImage((unsigned char *)(image_mat_active.data), image_mat.cols, image_mat.rows, image_mat.step, QImage::Format_RGB888);
		break;
	case kPasteSeamless:
		image_show = QImage((unsigned char *)(image_mat_active.data), image_mat.cols, image_mat.rows, image_mat.step, QImage::Format_RGB888);
		break;
	case kNone:
		image_show = QImage((unsigned char *)(image_mat.data), image_mat.cols, image_mat.rows, image_mat.step, QImage::Format_RGB888);
		break;
	default:
		break;
	}
	painter.drawImage(rect, image_show);
	
	// Draw choose region
	switch (draw_status_)
	{
	case kChoosePolygon:
		if (is_choosing_)
			painter.setPen(QPen(QColor(25, 25, 255), 3));
		else
			painter.setPen(QPen(QColor(0, 0, 0), 3));
		painter.drawPolygon(choosed_polygon);
		break;
	case kChooseFreehand:
		if (is_choosing_)
			painter.setPen(QPen(QColor(25, 25, 255), 3));
		else
			painter.setPen(QPen(QColor(0, 0, 0), 3));
		painter.drawPolygon(choosed_polygon);
		break;
	case kPasteClone:

		break;
	case kPasteSeamless:

		break;
	case kNone:

		break;
	default:
		break;
	}


	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoosePolygon:
		mousePressChoosePolygon(mouseevent);
		break;
	case kChooseFreehand:
		mousePressChooseFreehand(mouseevent);
		break;
	case kPasteClone:
		mousePressPasteClone(mouseevent);
		break;
	case kPasteSeamless:
		mousePressPasteSeamless(mouseevent);
		break;
	default:
		break;
	}
	update();
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoosePolygon:
		mouseMoveChoosePolygon(mouseevent);
		break;
	case kChooseFreehand:
		mouseMoveChooseFreehand(mouseevent);
		break;
	case kPasteClone:
		mouseMovePasteClone(mouseevent);
		break;
	case kPasteSeamless:
		mouseMovePasteSeamless(mouseevent);
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
	case kChoosePolygon:
		mouseReleaseChoosePolygon(mouseevent);
		break;
	case kChooseFreehand:
		mouseReleaseChooseFreehand(mouseevent);
		break;
	case kPasteClone:
		mouseReleasePasteClone(mouseevent);
		break;
	case kPasteSeamless:
		mouseReleasePasteSeamless(mouseevent);
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
		image_mat = cv::imread(filename.toLocal8Bit().toStdString());
		cv::cvtColor(image_mat, image_mat, cv::COLOR_BGR2RGB);
		image_mat_backup = image_mat.clone();
	}

	//cout << "image size: " << image_mat.size().width << ' ' << image_mat.size().height << endl;
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
	cv::cvtColor(image_mat, image_save, cv::COLOR_BGR2RGB);
	cv::imwrite(filename.toLatin1().data(), image_save);
}

void ImageWidget::Invert()
{
	if (image_mat.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat.begin<cv::Vec3b>(), iterend = image_mat.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		(*iter)[0] = 255 - (*iter)[0];
		(*iter)[1] = 255 - (*iter)[1];
		(*iter)[2] = 255 - (*iter)[2];
	}

	// equivalent member function of class QImage
	// image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if (image_mat.empty())
		return;
	int width = image_mat.cols;
	int height = image_mat.rows;

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat.at<cv::Vec3b>(j, i) = image_mat_backup.at<cv::Vec3b>(height - 1 - j, width - 1 - i);
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat.at<cv::Vec3b>(j, i) = image_mat_backup.at<cv::Vec3b>(j, width - 1 - i);
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
					image_mat.at<cv::Vec3b>(j, i) = image_mat_backup.at<cv::Vec3b>(height - 1 - j, i);
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
	if (image_mat.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat.begin<cv::Vec3b>(), iterend = image_mat.end<cv::Vec3b>(); iter != iterend; ++iter)
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
	image_mat = image_mat_backup.clone();
	point_start_ = point_end_ = QPoint(0, 0);
	update();
}

//Mouse Press
void ImageWidget::mousePressChoosePolygon(QMouseEvent* mouseevent)
{
	if (is_choosing_)
	{
		if (Qt::LeftButton == mouseevent->button())
		{
			choosed_polygon.append(mouseevent->pos());
			is_choosing_polyedge = true;
		}
		else if(Qt::RightButton == mouseevent->button())
		{
			is_choosing_ = false;
		}
	}
	else
	{
		if (Qt::LeftButton == mouseevent->button())
		{
			choosed_polygon.clear();
			choosed_polygon.append(mouseevent->pos());
			choosed_polygon.append(mouseevent->pos());
			is_choosing_ = true;
			is_choosing_polyedge = true;
		}
	}
}

void ImageWidget::mousePressChooseFreehand(QMouseEvent* mouseevent)
{
	choosed_polygon.clear();
	choosed_polygon.append(mouseevent->pos());
	is_choosing_ = true;
}

void ImageWidget::mousePressPasteClone(QMouseEvent* mouseevent)
{
	if (is_pasting_)
	{
		if (Qt::LeftButton == mouseevent->button())			//×ó¼üÍÏ¶¯
		{
			cv::Point inner_pos(mouseevent->pos().x() - paste_pos.x(), mouseevent->pos().y() - paste_pos.y());
			if (*(CImageGrad::AddrMatAt(source_window_->shared_clipboard->clip_mask, inner_pos)) > 0)
			{
				is_pasting_holding = true;
				point_last = mouseevent->pos();
			}
		}
		else if(Qt::RightButton ==  mouseevent->button())	//ÓÒ¼üÈ·¶¨²¢½áÊøÕ³Ìù
		{
			image_mat = image_mat_active.clone();
			draw_status_ = kNone;
			is_pasting_ = false;
			is_pasting_holding = false;
		}
	}
	
}
void ImageWidget::mousePressPasteSeamless(QMouseEvent* mouseevent)
{
	if (is_pasting_)
	{
		if (Qt::LeftButton == mouseevent->button())			//×ó¼üÍÏ¶¯
		{
			cv::Point inner_pos(mouseevent->pos().x() - paste_pos.x(), mouseevent->pos().y() - paste_pos.y());
			if (*(CImageGrad::AddrMatAt(source_window_->shared_clipboard->clip_mask, inner_pos)) > 0)
			{
				is_pasting_holding = true;
				point_last = mouseevent->pos();
			}
		}
		else if (Qt::RightButton == mouseevent->button())	//ÓÒ¼üÈ·¶¨²¢½áÊøÕ³Ìù
		{
			image_mat = image_mat_active.clone();
			draw_status_ = kNone;
			is_pasting_ = false;
			is_pasting_holding = false;
		}
	}
}

//Mouse Move
void ImageWidget::mouseMoveChoosePolygon(QMouseEvent* mouseevent)
{
	if (is_choosing_&&is_choosing_polyedge)
	{
		choosed_polygon[choosed_polygon.size() - 1] = mouseevent->pos();
	}
}
void ImageWidget::mouseMoveChooseFreehand(QMouseEvent* mouseevent)
{
	if (is_choosing_)
	{
		if (choosed_polygon[choosed_polygon.size() - 1] != mouseevent->pos())
		{
			choosed_polygon.append(mouseevent->pos());
		}
	}
}
void ImageWidget::mouseMovePasteClone(QMouseEvent* mouseevent)
{
	if (is_pasting_&&is_pasting_holding)
	{
		paste_pos += mouseevent->pos() - point_last;
		point_last = mouseevent->pos();
		DrawPasteClone();
	}
}
void ImageWidget::mouseMovePasteSeamless(QMouseEvent* mouseevent)
{
	if (is_pasting_&&is_pasting_holding)
	{
		paste_pos += mouseevent->pos() - point_last;
		point_last = mouseevent->pos();
		DrawPasteSeamless();
	}
}

//Mouse Release
void ImageWidget::mouseReleaseChoosePolygon(QMouseEvent* mouseevent)
{
	is_choosing_polyedge = false;
}
void ImageWidget::mouseReleaseChooseFreehand(QMouseEvent* mouseevent)
{
	is_choosing_ = false;
}
void ImageWidget::mouseReleasePasteClone(QMouseEvent* mouseevent)
{
	if (is_pasting_&&is_pasting_holding)
	{
		is_pasting_holding = false;
	}
}
void ImageWidget::mouseReleasePasteSeamless(QMouseEvent* mouseevent)
{
	if (is_pasting_&&is_pasting_holding)
	{
		is_pasting_holding = false;
	}
}

void ImageWidget::DrawPasteClone()
{
	image_mat_active = image_mat.clone();
	int feasible_width = std::min(source_window_->shared_clipboard->clip_image.size().width, image_mat_active.size().width - paste_pos.x());
	int feasible_height = std::min(source_window_->shared_clipboard->clip_image.size().height, image_mat_active.size().height - paste_pos.y());
	for (int i = std::max(0, -paste_pos.x()); i < feasible_width; i++)
	{
		for (int j = std::max(0, -paste_pos.y()); j < feasible_height; j++)
		{
			if (source_window_->shared_clipboard->clip_mask.at<cv::uint8_t>(j, i) > 0)
			{
				image_mat_active.at<cv::Vec3b>(j + paste_pos.y(), i + paste_pos.x()) = source_window_->shared_clipboard->clip_image.at<cv::Vec3b>(j, i);
			}
		}
	}
}

void ImageWidget::DrawPasteSeamless()
{
	poisson_editing.SetShiftPos(cv::Point(paste_pos.x(),paste_pos.y()));
	poisson_editing.SetShiftPos(cv::Point(paste_pos.x(),paste_pos.y()));
	image_mat_active = poisson_editing.GetFusion();
}

void ImageWidget::CopytoClipboard()
{
	if (choosed_polygon.size() > 2)
	{
		cv::Rect frect = CScanLine::CalcRectRegion(choosed_polygon, image_mat.size().width, image_mat.size().height);

		source_window_->shared_clipboard->isEmpty = false;
		source_window_->shared_clipboard->clip_image = image_mat(frect);

		source_window_->shared_clipboard->clip_mask = cv::Mat::zeros(frect.size().height, frect.size().width, CV_8UC1);
		CScanLine scanline;
		scanline.SetPolygon(CScanLine::ShiftPolygon(choosed_polygon, QPoint(-frect.x, -frect.y)));
		scanline.FillPolygon(source_window_->shared_clipboard->clip_mask, (uchar)255);
	}
}

void ImageWidget::BeginPasteClone()
{
	if (!source_window_->shared_clipboard->isEmpty)
	{
		paste_pos = QPoint(10, 10);		//ÉèÖÃÕ³ÌùÆðÊ¼Î»ÖÃ
		draw_status_ = kPasteClone;		//ÉèÖÃ»æÍ¼×´Ì¬
		is_pasting_ = true;
		DrawPasteClone();				
		update();
	}	
}

void ImageWidget::BeginPasteSeamless()
{
	if (!source_window_->shared_clipboard->isEmpty)
	{
		paste_pos = QPoint(10, 10);		//ÉèÖÃÕ³ÌùÆðÊ¼Î»ÖÃ
		draw_status_ = kPasteSeamless;		//ÉèÖÃ»æÍ¼×´Ì¬
		is_pasting_ = true;
		poisson_editing.SetSourceMask(source_window_->shared_clipboard->clip_image, source_window_->shared_clipboard->clip_mask);
		poisson_editing.SetTarget(image_mat);
		DrawPasteSeamless();				
		update();
	}
}
