#pragma once
#include <QWidget>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <Eigen\Dense>
#include <Eigen\Sparse>
#include <Eigen\SparseLU>
#include "shape.h"
#include "Rect.h"
#include "Freehand.h"
#include "ScanLine.h"

class ChildWindow;
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

enum DrawStatus
{
	kRect,
	kFreehand,
	kPaste, 
	kNone
};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(ChildWindow *relatewindow);
	~ImageWidget(void);

	//To Do
	int ImageWidth();											// Width of image
	int ImageHeight();											// Height of image
	void set_draw_status_to_chooseRect();
	void set_draw_status_to_chooseFreehand();
	void set_draw_status_to_paste_mixed();
	void set_draw_status_to_paste();
	void set_Copy();

	cv::Mat image();
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

	Eigen::MatrixXd selected_area;
	Eigen::SparseLU<Eigen::SparseMatrix<double>> *coeff_Matrix;

private:
	//change QImage to Opencv
	cv::Mat 					image_mat_;						// image 
	cv::Mat						image_mat_backup_;
	int base_x;//the left up point of the area
	int base_y;
	int w_selected;
	int h_selected;
	Shape *shape_;
	bool drawing;
	bool mixed;

	// Pointer of child window
	ChildWindow					*source_window_;				// Source child window

	// Signs
	DrawStatus					draw_status_;					// Enum type of draw status
	bool						is_choosing_;
	bool						is_pasting_;
};

