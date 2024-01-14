#pragma once
#include <QWidget>
#include <vector>
#include "Line.h"
#include "Warping.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

using std::vector;

enum WarpType
{
	None = -1,
	DefaultW = 0,
	Idw = 1,
	Rbf = 2,
};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

protected:
	void paintEvent(QPaintEvent *paintevent);

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void Set_ControlPoints();
	void IDW_ImageWarping();
	void RBF_ImageWarping();
	void CP_ImageWarping();

private:
	QImage		*ptr_image_;				// image 
	QImage		*ptr_image_1;
	QImage		*ptr_image_backup_;
	QImage		temp;
	
	Warping *iw;
	WarpType warp_status_;
	bool in_affineset_;
	bool compare_;
	QPoint	start_point_;
	QPoint	end_point_;
	Line* current_line_;

	QString filename_;
};

