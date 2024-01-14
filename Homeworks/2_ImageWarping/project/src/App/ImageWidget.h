#pragma once
#include <QWidget>
#include "ImagePoint.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);



protected:
	//重写父类的虚拟函数，以便在小部件上自定义绘制行为
	void paintEvent(QPaintEvent *paintevent);//处理绘制事件

	template <typename Warper>
	void doWarp(void);
	double Distance(int x1, int y1, int x2, int y2);
	IPoint findNearestNonWhitePixel(int x, int y);


public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin

	void IDWWarp();
	void RBFWarp();
	void Clear();
	void Fillgap();
	void loadDebug();

private:
	QImage		*ptr_image_;				//用于存储当前图像的指针
	QImage		*ptr_image_backup_;			//用于存储原始图像的指针，以便在需要恢复图像

	bool mouse_status_;
	QPoint start_point_;
	QPoint end_point_;

	//我们用map存储点p和点q，但是p和q本身又是一个坐标，所以是vector<map<坐标，坐标>>
	std::vector<IMap> points;
	IPoint img_origin;
	
};

