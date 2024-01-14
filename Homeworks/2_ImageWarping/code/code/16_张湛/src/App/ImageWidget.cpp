#include "ImageWidget.h"
#include "Warping_IDW.h"
#include "Warping_RBF.h"
#include "Warping.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_1 = new QImage();
	ptr_image_backup_ = new QImage();
	iw = new Warping();
	warp_status_ = None;
	in_affineset_ = false;
	compare_ = false;
}


ImageWidget::~ImageWidget(void)
{
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	int w = ptr_image_->width(), h = ptr_image_->height();
	if (compare_) {
		if (w * 2.5 > width()) {
			h = width() / 2.5 *h / w;
			w = width() / 2.5;
		}
		if (h > height()) {
			w = height()*w / h;
			h = height();
		}
		temp = (*ptr_image_).scaled(w, h, Qt::KeepAspectRatio);
		QImage temp1 = (*ptr_image_1).scaled(w, h, Qt::KeepAspectRatio);
		QRect rect1 = QRect((width() - 2.5 * temp1.width()) / 2, (height() - temp1.height()) / 2, temp1.width(), temp1.height());
		QRect rect2 = QRect((width() + 0.5 * temp.width()) / 2, (height() - temp.height()) / 2, temp.width(), temp.height());
		painter.drawImage(rect1, temp1);
		painter.drawImage(rect2, temp);
	}
	else {
		if (w > width()) {
			h = width()*h / w;
			w = width();
		}
		if (h > height()) {
			w = height()*w / h;
			h = height();
		}
		temp = (*ptr_image_).scaled(w, h, Qt::KeepAspectRatio);
		QRect rect = QRect((width() - temp.width()) / 2, (height() - temp.height()) / 2, temp.width(), temp.height());
		painter.drawImage(rect, temp);
	}

	int dw = (this->width() - temp.width()) / 2;
	int dh = (this->height() - temp.height()) / 2;
	if (warp_status_ != None)
	{
		if (warp_status_ == DefaultW) {
			QColor green(0, 255, 0);
			QColor blue(0, 0, 255);
			QColor greenblue(0, 191, 191, 191);
			QColor red(191, 0, 0, 191);
			QPen pen1(green);
			QPen pen2(blue);
			QPen pen3(greenblue);
			QPen pen4(red);
			pen4.setWidth(2);
			pen3.setWidth(2);
			pen2.setWidth(5);
			pen1.setWidth(5);

			if (in_affineset_) painter.setPen(pen4);
			else painter.setPen(pen3);
			painter.drawLine(start_point_, end_point_);
			painter.setPen(pen2);
			painter.drawPoint(start_point_);
			painter.setPen(pen1);
			painter.drawPoint(end_point_);
		}
		
		iw->DrawCP(painter, dw, dh);
	}

	update();
	painter.end();
}

void ImageWidget::Open()
{
	// Open file
	filename_ = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!filename_.isEmpty())
	{
		ptr_image_->load(filename_);
		*(ptr_image_backup_) = *(ptr_image_);
	}
	else
		return;

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout << "image size: " << ptr_image_->width() << ' ' << ptr_image_->height() << endl;
	compare_ = false;
	warp_status_ = None;
	iw->Init();
	update();
}

void ImageWidget::Save()
{
	if (filename_.isNull())
		SaveAs();
	else {
		if ((*ptr_image_).isNull()) {
			QMessageBox::about(this, tr("Error"), tr("          No photo to save.          "));
			return;
		}
		ptr_image_->save(filename_);
	}
	compare_ = false;
	warp_status_ = None;
	iw->Init();
}

void ImageWidget::SaveAs()
{
	if ((*ptr_image_).isNull()) {
		QMessageBox::about(this, tr("Error"), tr("          No photo to save.          "));
		return;
	}
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}
	if (warp_status_ != None) {
		QPixmap pixmap(temp.width(), temp.height());
		this->render(&pixmap, QPoint(),QRegion((width() - temp.width()) / 2, (height() - temp.height()) / 2, temp.width(), temp.height()));//保存当前窗口
		pixmap.save(filename);
	}
	else if (compare_) {
		QPixmap pixmap(this->size());
		this->render(&pixmap);//保存当前窗口
		pixmap.save(filename);
	}
	else {
		ptr_image_->save(filename);
	}
}

void ImageWidget::mousePressEvent(QMouseEvent * event)
{
	if (warp_status_ != DefaultW && warp_status_ != None) {
		iw = new Warping();
		iw->Init();
		warp_status_ = DefaultW;
	}
	if (Qt::LeftButton == event->button()) {
		start_point_ = end_point_ = event->pos();
	}
	if (Qt::RightButton == event->button() && warp_status_ != None) {
		start_point_ = end_point_ = event->pos();
		in_affineset_ = true;
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (warp_status_ != None) {
		end_point_ = event->pos();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent * event)
{
	int dw = (this->width() - temp.width()) / 2;
	int dh = (this->height() - temp.height()) / 2;
	current_line_ = new Line(start_point_.rx() - dw, start_point_.ry() - dh, end_point_.rx() - dw, end_point_.ry() - dh);
	cout <<"start: " <<start_point_.rx() - dw <<"," <<start_point_.ry() - dh <<"\nend: " << end_point_.rx() - dw <<", "<< end_point_.ry() - dh << endl;
	iw->CpPushBack(current_line_,in_affineset_);
	in_affineset_ = false;
}

void ImageWidget::Invert()
{
	for (int i = 0; i < ptr_image_->width(); i++)
	{
		for (int j = 0; j < ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	compare_ = false;
	warp_status_ = None;
	iw->Init();
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
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
					ptr_image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	compare_ = false;
	warp_status_ = None;
	iw->Init();
	update();
}

void ImageWidget::TurnGray()
{
	for (int i = 0; i < ptr_image_->width(); i++)
	{
		for (int j = 0; j < ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}
	compare_ = false;
	warp_status_ = None;
	iw->Init();
	update();
}

void ImageWidget::Set_ControlPoints()
{
	compare_ = false;
	if(warp_status_ != DefaultW) warp_status_ = DefaultW;
	start_point_ = end_point_=QPoint(-10,-10);
	iw = new Warping();
	iw->Init();
	update();
}

void ImageWidget::IDW_ImageWarping() {
	if (warp_status_ != DefaultW && warp_status_ != None) {
		iw = new Warping();
		iw->Init();
		return;
	}
	iw = new Warping_IDW((*iw));

	int width = ptr_image_->width();
	int height = ptr_image_->height();
	QImage image_tmp(*(ptr_image_));
	ptr_image_->fill(QColor(255, 255, 255));
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			MatrixXf new_ij = iw->WarpingIJ(i,j);
			if (new_ij(0, 0) < width && new_ij(0, 0) >= 0 && new_ij(1, 0) < height && new_ij(1, 0) >= 0) 
				ptr_image_->setPixel(new_ij(0, 0), new_ij(1, 0), image_tmp.pixel(i, j));
		}
	}
	iw->FillHole_2(ptr_image_);
	compare_ = false;
	warp_status_ = Idw;
	update();
}

void ImageWidget::RBF_ImageWarping() {
	if (warp_status_ != DefaultW && warp_status_ != None) {
		iw = new Warping();
		iw->Init();
		return;
	}
	iw = new Warping_RBF((*iw));

	int width = ptr_image_->width();
	int height = ptr_image_->height();
	QImage image_tmp(*(ptr_image_));
	ptr_image_->fill(QColor(255, 255, 255));
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			MatrixXf new_ij = iw->WarpingIJ(i, j);
			if (new_ij(0, 0) < width && new_ij(0, 0) >= 0 && new_ij(1, 0) < height && new_ij(1, 0) >= 0)
				ptr_image_->setPixel(new_ij(0, 0), new_ij(1, 0), image_tmp.pixel(i, j));
		}
	}
	iw->FillHole_2(ptr_image_);
	compare_ = false;
	warp_status_ = Rbf;
	update();
}

void ImageWidget::CP_ImageWarping() {
	if (warp_status_ != DefaultW && warp_status_ != None) {
		iw = new Warping();
		iw->Init(); 
	}
	Warping_IDW iw1((*iw));
	Warping_RBF iw2((*iw));
	QImage image_tmp(*(ptr_image_));
	ptr_image_->fill(QColor(255, 255, 255));
	*(ptr_image_1) = *(ptr_image_);

	int width = ptr_image_->width();
	int height = ptr_image_->height();
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			MatrixXf new_ij1 = iw1.WarpingIJ(i, j);
			MatrixXf new_ij2 = iw2.WarpingIJ(i, j);
			if (new_ij1(0, 0) < width && new_ij1(0, 0) >= 0 && new_ij1(1, 0) < height && new_ij1(1, 0) >= 0) 
				ptr_image_1->setPixel(new_ij1(0, 0), new_ij1(1, 0), image_tmp.pixel(i, j));
			if (new_ij2(0, 0) < width && new_ij2(0, 0) >= 0 && new_ij2(1, 0) < height && new_ij2(1, 0) >= 0) 
				ptr_image_->setPixel(new_ij2(0, 0), new_ij2(1, 0), image_tmp.pixel(i, j));
		}
	}
	iw1.FillHole_2(ptr_image_1);
	iw2.FillHole_2(ptr_image_);
	compare_ = true;
	warp_status_ = None;
	iw->Init();
	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	compare_ = false;
	warp_status_ = None;
	iw->Init();
	update();
}