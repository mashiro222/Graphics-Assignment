#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets>

ImageWidget::ImageWidget()
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	ptr_image_history_.push_back(ptr_image_backup_);

	draw_status_ = false;
	warp_type_ = WarpType::None;
	current_line_ = new QLine();


}

ImageWidget::~ImageWidget()
{
	for (auto&& p : ptr_image_history_)
		if (p)
		{
			delete p;
		}

	if (ptr_image_)
		delete ptr_image_;
	
	//Since ptr_image_backup_ == ptr_image_history_.front(), do not delete ptr_image_backup again
	assert(ptr_image_backup_ == ptr_image_history_.front());	

	for (auto&& p : pair_array_)
		if (p)
		{
			delete p;
		}

	if (current_line_)
		delete current_line_;
}

void ImageWidget::paintEvent(QPaintEvent*)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect(left(), top(), ptr_image_->width() * image_ratio_, ptr_image_->height() * image_ratio_);
	painter.drawImage(rect, *ptr_image_);

	// Draw lines
	for (auto&& p : pair_array_)
	{
		QLine temp_line_;
		temp_line_.setP1(QPoint(p->x1() * image_ratio_ + left(), p->y1() * image_ratio_ + top()));
		temp_line_.setP2(QPoint(p->x2() * image_ratio_ + left(), p->y2() * image_ratio_ + top()));
		draw_InputLine(temp_line_);
	}
	if (draw_status_)
	{
		draw_InputLine(*current_line_);
	}

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && warp_type_ == WarpType::Setting)
	{
		draw_status_ = true;
		current_line_->setP1(event->pos());
		current_line_->setP2(event->pos());
		update();	
	}		
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status_)
	{
		current_line_->setP2(event->pos());
		update();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && draw_status_)
	{
		draw_status_ = false;		
		current_line_->setP2(event->pos());

		// change the position of widget to the position of image
		QPoint p1((current_line_->x1() - left()) * 1 / image_ratio_, (current_line_->y1() - top()) * 1 / image_ratio_);
		QPoint p2((current_line_->x2() - left()) * 1 / image_ratio_, (current_line_->y2() - top()) * 1 / image_ratio_);
		QLine* current_pair_ = new QLine();
		current_pair_->setP1(p1);
		current_pair_->setP2(p2);
		pair_array_.push_back(current_pair_);

		update();
	}
}

void ImageWidget::draw_InputLine(const QLine& line)
{
	if (!visible)
	{
		return;
	}
	QPainter painter(this);
	QPen pen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter.setPen(pen);
	painter.drawLine(line.p1() - QPoint(3, 3), line.p1() + QPoint(3, 3));
	painter.drawLine(line.p1() - QPoint(3, -3), line.p1() + QPoint(3, -3));
	painter.drawLine(line.p1(), line.p2());
	pen.setColor(Qt::blue);
	painter.setPen(pen);
	painter.drawLine(line.p2() - QPoint(3, 3), line.p2() + QPoint(3, 3));
	painter.drawLine(line.p2() - QPoint(3, -3), line.p2() + QPoint(3, -3));	
}

QImage* ImageWidget::Open()
{		
	// Open file
	QString filename = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!filename.isEmpty())
	{
		if (!file_name_.isNull())	// If users did not close previous file
		{
			for (auto&& p : ptr_image_history_)
				if (p)
				{
					delete p;
				}
			ptr_image_history_.clear();
			ptr_image_backup_ = new QImage;
			ptr_image_history_.push_back(ptr_image_backup_);
		}

		file_name_ = filename;
		ptr_image_->load(file_name_);
		*(ptr_image_backup_) = *(ptr_image_);

		
		return ptr_image_;
	}

	update();

	if (!file_name_.isNull())
		return Q_NULLPTR;
	else
		return ptr_image_;	
}

void ImageWidget::Save()
{
	if (file_name_.isNull())
	{
		return;
	}

	ptr_image_->save(file_name_);
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	if (file_name_.isNull())
	{
		return;
	}

	for (int i = 0; i < ptr_image_->width(); i++)
	{
		for (int j = 0; j < ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}

	SaveHistory();
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if (file_name_.isNull())
	{
		return;
	}

	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	// Mirror image horizontally
	if (ishorizontal)
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				ptr_image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
			}
		}
	}
	
	// Mirror image vertically
	if (isvertical)
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				ptr_image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
			}
		}
	}

	SaveHistory();
	update();
}

void ImageWidget::DoWarp(int setgap, bool warpPoints)
{
	// Check status
	assert(WarpType::Setting != warp_type_);
	if (warp_type_ == WarpType::None)
	{
		return;
	}

	// the case no anchor points is set will be seen as misclick
	if (pair_array_.isEmpty())
	{
		warp_type_ = WarpType::Setting;
		return;
	}

	// Warp the image
	Warping* pwarping_;
	switch (warp_type_)
	{
	case WarpType::IDW:
		pwarping_ = new WarpingIDW(pair_array_);
		break;

	case WarpType::RBF:
		pwarping_ = new WarpingRBF(pair_array_);
		break;

	default:
		break;
	}	
	pwarping_->Warp(ptr_image_, setgap, warpPoints);
	delete pwarping_;

	// Finish warping
	warp_type_ = WarpType::Setting;
	SaveHistory();
	update();
}

void ImageWidget::Mirror_H()
{
	Mirror(true, false);
}

void ImageWidget::Mirror_V()
{
	Mirror(false, true);
}

void ImageWidget::TurnGray()
{
	if (file_name_.isNull())
	{
		return;
	}

	for (int i = 0; i < ptr_image_->width(); i++)
	{
		for (int j = 0; j < ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}

	SaveHistory();
	update();
}

void ImageWidget::Restore()
{
	if (file_name_.isEmpty())
	{
		return;
	}

	*(ptr_image_) = *(ptr_image_backup_);
	SaveHistory();
	update();
}

void ImageWidget::Undo()
{
	if (ptr_image_history_.size() > 1)
	{
		QImage* back = ptr_image_history_.back();
		if (back)
		{
			delete back;
			back = Q_NULLPTR;
		}
		ptr_image_history_.pop_back();
		*(ptr_image_) = *(ptr_image_history_.back());

		update();
	}
}

void ImageWidget::StartSetting()
{
	if (!file_name_.isNull())
	{
		warp_type_ = WarpType::Setting;
	}
}

void ImageWidget::IDWWarping(int setgap, bool warpPoints)
{
	if (warp_type_ != WarpType::None)
	{
		warp_type_ = WarpType::IDW;
		DoWarp(setgap, warpPoints);
	}
}

void ImageWidget::RBFWarping(int setgap, bool warpPoints)
{
	if (warp_type_ != WarpType::None)
	{
		warp_type_ = WarpType::RBF;
		DoWarp(setgap, warpPoints);
	}
}

void ImageWidget::ClearSetting()
{
	for (auto&& p : pair_array_)
		if (p)
		{
			delete p;
		}
	pair_array_.clear();
	warp_type_ = WarpType::None;
	update();
}

int ImageWidget::Change_Visibility()
{
	visible = not visible;
	update();
	return visible;
}

void ImageWidget::set_ratio(double ratio)
{
	image_ratio_ = ratio;
	update();
}

int ImageWidget::left()
{
	if (file_name_.isNull())
	{
		return 0;
	}

	return (width() - ptr_image_->width() * image_ratio_) / 2;
}

int ImageWidget::top()
{
	if (file_name_.isNull())
	{
		return 0;
	}

	return (height() - ptr_image_->height() * image_ratio_) / 2;
}

// Add current image to history
void ImageWidget::SaveHistory()
{
	QImage* image_tmp;
	image_tmp = new QImage;
	*(image_tmp) = *(ptr_image_);
	ptr_image_history_.push_back(image_tmp);
}