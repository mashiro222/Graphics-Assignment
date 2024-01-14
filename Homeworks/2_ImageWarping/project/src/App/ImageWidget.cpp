#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include <vector>
#include <map>
#include "IDWWarper.h"
#include "RBFWarper.h"
#include <cmath>

using std::cout;
using std::endl;

//用于显示图像，并提供一些图像处理功能

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	setMouseTracking(true);
}


ImageWidget::~ImageWidget(void)
{
}

void ImageWidget::loadDebug(void) {
	points.clear();
	points.push_back(std::make_pair(IPoint(0, 0), IPoint(0, 0)));
	points.push_back(std::make_pair(IPoint(255, 0), IPoint(255, 0)));
	points.push_back(std::make_pair(IPoint(0, 255), IPoint(0, 255)));
	points.push_back(std::make_pair(IPoint(255, 255), IPoint(255, 255)));
	points.push_back(std::make_pair(IPoint(128, 100), IPoint(128, 80)));
	points.push_back(std::make_pair(IPoint(128, 155), IPoint(128, 175)));
	points.push_back(std::make_pair(IPoint(100, 128), IPoint(80, 128)));
	points.push_back(std::make_pair(IPoint(155, 128), IPoint(175, 128)));
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
	QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_); 



	//获得左上角点
	img_origin.setX((width() - ptr_image_->width()) / 2);
	img_origin.setY((height() - ptr_image_->height()) / 2);
	//qDebug("img_origin: %d %d", img_origin.X(), img_origin.Y());

	//绘制用户画的线
	for (int i = 0; i < points.size(); i++) {
		painter.drawLine(points[i].first.X() + img_origin.X(), points[i].first.Y() + img_origin.Y(), points[i].second.X() + img_origin.X(), points[i].second.Y() + img_origin.Y());
	}

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* event) {
	if (Qt::LeftButton == event->button()) {
		mouse_status_ = true;
		start_point_ = end_point_ = event->pos();
	}
	qDebug("press %d %d", event -> pos().rx(), event->pos().ry());
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event) {
	if (mouse_status_ == true) {
		end_point_ = event->pos();
		update();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event) {
	//获取约束点
	IPoint p, q;
	p.setX(start_point_.rx() - img_origin.X()).setY(start_point_.ry() - img_origin.Y());
	q.setX(end_point_.rx() - img_origin.X()).setY(end_point_.ry() - img_origin.Y());
	//存进去
	points.push_back(std::make_pair(p, q));
	
	mouse_status_ = false;
	update();
	qDebug("Adding (%d,%d)->(%d,%d) (orig: (%d,%d)->(%d,%d)), total %d now.",
		start_point_.rx() - img_origin.X(),
		start_point_.ry() - img_origin.Y(),
		end_point_.rx() - img_origin.X(),
		end_point_.ry() - img_origin.Y(), start_point_.rx(),
		start_point_.ry(), end_point_.rx(), end_point_.ry(),
		points.size());
}

void ImageWidget::IDWWarp() {
	doWarp<IDWWarper>();
}

void ImageWidget::RBFWarp() {
	doWarp<RBFWarper>();
}

//核心函数，提供p，q点生成新的
template <typename Warper>
void ImageWidget::doWarp() {
	QImage image_new(ptr_image_->width(), ptr_image_->height(), ptr_image_->format());
	
	Warper::getInstance()->initialize(points);

	std::vector<std::vector<int>> check(ptr_image_->width(), std::vector<int>(ptr_image_->height(), 0));

	
	for (int i = 0; i< ptr_image_->width(); i++) {
		for (int j = 0; j < ptr_image_->height(); j++) {
			QRgb color = ptr_image_->pixel(i, j);
			//p=(i,j),q=f(i,j)
			IPoint p, q;
			p.setX(i).setY(j);
			q = Warper::getInstance()->func(p);
			if (q.X() >= ptr_image_->width() || q.X() <= 0 || q.Y() >= ptr_image_->height() || q.Y() <= 0) {
				continue;
			}
			check[q.X()][q.Y()] = 1;
			image_new.setPixel(q.X(), q.Y(), qRgb(qRed(color), qGreen(color), qBlue(color)));

		}
	}
	*ptr_image_ = image_new;
	update();
}



void ImageWidget::Clear() {
	points.clear();
	update();
}


// 辅助函数：计算两点之间的欧氏距离
double ImageWidget::Distance(int x1, int y1, int x2, int y2) {
	return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

IPoint ImageWidget::findNearestNonWhitePixel(int x, int y) {
	//int maxDistance = std::max(ptr_image_->width(), ptr_image_->height()); // 设置一个最大距离
	IPoint nearestNonWhitePixel(-1,-1);
	double max = 10000;
	for (int i = 0; i < ptr_image_->width(); ++i) {
		for (int j = 0; j < ptr_image_->height(); ++j) {
			QRgb color = ptr_image_->pixel(i, j);
			if (qRed(color) < 255 || qGreen(color) < 255 || qBlue(color) < 255) {
				// 找到非纯白色点，更新最近非纯白色点并更新最小距离
				double dist = Distance(x, y, i, j);
				if (dist < max && dist < 6) {
					max = dist;
					nearestNonWhitePixel.setX(i);
					nearestNonWhitePixel.setY(j);
				}
			}
		}
	}

	return nearestNonWhitePixel;
}


void ImageWidget::Fillgap() {
	//修改ptr_image_
	//k为迭代次数
	for (int k = 0; k < 15; k++) {
		for (int i = 1; i < ptr_image_->width() - 1; i++)
		{
			for (int j = 1; j < ptr_image_->height() - 1; j++)
			{
				QRgb color = ptr_image_->pixel(i, j);
				if (qRed(color) == 205 && qGreen(color) == 205 && qBlue(color) == 205) {
					int R, G, B; R = 0; G = 0; B = 0;
					std::vector<QRgb> color_around(8);
					color_around[0] = ptr_image_->pixel(i - 1, j - 1);
					color_around[1] = ptr_image_->pixel(i - 1, j);
					color_around[2] = ptr_image_->pixel(i - 1, j + 1);
					color_around[3] = ptr_image_->pixel(i, j - 1);
					color_around[4] = ptr_image_->pixel(i, j + 1);
					color_around[5] = ptr_image_->pixel(i + 1, j - 1);
					color_around[6] = ptr_image_->pixel(i + 1, j);
					color_around[7] = ptr_image_->pixel(i + 1, j + 1);
					for (int k = 0; k < 8; k++) {
						R += qRed(color_around[k]);
						G += qGreen(color_around[k]);
						B += qBlue(color_around[k]);
					}

					ptr_image_->setPixel(i, j, qRgb(R / 8, G / 8, B / 8));
				}
			}
		}
	}


	////另一种方法
	//int time = 0;
	//QImage* ptr_image_temp = ptr_image_;
	//for (int i = 1; i < ptr_image_->width() - 1; i++) {
	//	for (int j = 1; j < ptr_image_->width() - 1; j++) {
	//		QRgb color = ptr_image_->pixel(i, j);
	//		if (qRed(color) == 255 && qGreen(color) == 255 && qBlue(color) == 255) {
	//			time++;
	//			printf("i %d j %d time %d\n", i, j, time);
	//			IPoint temp = findNearestNonWhitePixel(i, j);
	//			int i_temp = temp.X();
	//			int j_temp = temp.Y();
	//			QRgb color_temp = ptr_image_->pixel(i_temp, j_temp);
	//			ptr_image_temp->setPixel(i, j, qRgb(qRed(color_temp), qGreen(color_temp), qBlue(color_temp)));
	//		}
	//	}
	//}
	//ptr_image_ = ptr_image_temp;

	update();
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
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

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

// equivalent member function of class QImage
//*(ptr_image_) = ptr_image_->mirrored(true, true);
void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else			//仅水平翻转			
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)		//仅垂直翻转
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
	}

	update();
}

void ImageWidget::TurnGray()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	update();
}
