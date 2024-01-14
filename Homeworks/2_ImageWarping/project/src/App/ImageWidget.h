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
	//��д��������⺯�����Ա���С�������Զ��������Ϊ
	void paintEvent(QPaintEvent *paintevent);//��������¼�

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
	QImage		*ptr_image_;				//���ڴ洢��ǰͼ���ָ��
	QImage		*ptr_image_backup_;			//���ڴ洢ԭʼͼ���ָ�룬�Ա�����Ҫ�ָ�ͼ��

	bool mouse_status_;
	QPoint start_point_;
	QPoint end_point_;

	//������map�洢��p�͵�q������p��q��������һ�����꣬������vector<map<���꣬����>>
	std::vector<IMap> points;
	IPoint img_origin;
	
};

