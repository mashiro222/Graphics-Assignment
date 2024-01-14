#pragma once
#include <QWidget>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include "Domain.h"
#include "RectDomain.h"
#include "PolyDomain.h"
#include "Cloning.h"
#include "NormalCloning.h"
#include "SeamlessCloning.h"
#include "MixedCloning.h"

class ChildWindow;
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

enum DrawStatus
{
	kChoose, 
	kPaste, 
	kNone
};

enum CloneStatus
{
	kDefault,
	kSeamless,
	kMixed
};

enum ChooseStatus
{
	kRect,
	kPolygon,
	kFreehand
};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(ChildWindow *relatewindow);
	~ImageWidget(void);

	int ImageWidth();											// Width of image
	int ImageHeight();											// Height of image
	void set_draw_status_to_choose();
	void set_draw_status_to_paste();
	void set_clone_status_to_seamless();
	void set_clone_status_to_mixed();
	void set_clone_status_to_default();
	void set_choose_status_to_rect();
	void set_choose_status_to_polygon();
	void set_choose_status_to_freehand();
	void InitChoose();											// Init choose status
	cv::Mat* image();
	void set_source_window(ChildWindow* childwindow);

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent *mouseevent);
	void mouseMoveEvent(QMouseEvent *mouseevent);
	void mouseReleaseEvent(QMouseEvent *mouseevent);

public slots:
	// File IO
	void Open(QString filename);								// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin

public:
	QPoint						point_start_;					// Left top point of rectangle region
	QPoint						point_end_;						// Right bottom point of rectangle region
	PointList					vertice_list_;					// Vertices of Polygon
	Domain						*domain_;						// Domain of cloning
	Cloning						*clone_;

private:
	cv::Mat						*image_;						// image 
	cv::Mat						*image_backup_;

	// Pointer of child window
	ChildWindow					*source_window_;				// Source child window

	// Signs
	DrawStatus					draw_status_;					// Enum type of draw status
	CloneStatus					clone_status_;
	ChooseStatus				choose_status_;
	bool						is_choosing_;
	bool						is_pasting_;
};