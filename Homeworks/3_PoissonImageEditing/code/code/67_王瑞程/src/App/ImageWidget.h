#pragma once
#include <QWidget>
#include <opencv2/core/core.hpp>
#include "PoissonFusion.h"

class ChildWindow;
QT_BEGIN_NAMESPACE
class cv::Mat;
class QPainter;
QT_END_NAMESPACE

enum EnumDrawStatus
{
	kChoosePolygon,
	kChooseFreehand,
	kPasteClone,
	kPasteSeamless,
	kNone
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
	void set_draw_status(EnumDrawStatus astatus);
	cv::Mat image();
	void set_source_window(ChildWindow* childwindow);
	void set_mixed(bool amixed);

protected:
	void paintEvent(QPaintEvent *paintevent);
	
	void mousePressEvent(QMouseEvent *mouseevent);
	void mousePressChoosePolygon(QMouseEvent *mouseevent);
	void mousePressChooseFreehand(QMouseEvent *mouseevent);
	void mousePressPasteClone(QMouseEvent *mouseevent);
	void mousePressPasteSeamless(QMouseEvent *mouseevent);

	void mouseMoveEvent(QMouseEvent *mouseevent);
	void mouseMoveChoosePolygon(QMouseEvent *mouseevent);
	void mouseMoveChooseFreehand(QMouseEvent *mouseevent);
	void mouseMovePasteClone(QMouseEvent *mouseevent);
	void mouseMovePasteSeamless(QMouseEvent *mouseevent);

	void mouseReleaseEvent(QMouseEvent *mouseevent);
	void mouseReleaseChoosePolygon(QMouseEvent *mouseevent);
	void mouseReleaseChooseFreehand(QMouseEvent *mouseevent);
	void mouseReleasePasteClone(QMouseEvent *mouseevent);
	void mouseReleasePasteSeamless(QMouseEvent *mouseevent);

	void DrawPasteClone();
	void DrawPasteSeamless();

public:
	// File IO
	
	/*Open an image file, support ".bmp, .png, .jpg" format*/
	void Open(QString filename);			
	/* Save image to current file. (Temporarily not supported for security of your files */
	void Save();				
	/* Save image to another file*/
	void SaveAs();												


	// Image processing

	/* Invert pixel value in image*/
	void Invert();					
	/* Mirror image vertically or horizontally*/
	void Mirror(bool horizontal=false, bool vertical=true);		
	/* Turn image to gray-scale map*/
	void TurnGray();		
	/* Restore image to origin*/
	void Restore();											

	void CopytoClipboard();

	void BeginPasteClone();
	void BeginPasteSeamless();
	
public:
	QPoint						point_start_;
	QPoint						point_last;
	QPoint						point_end_;						

private:
	cv::Mat						image_mat_active;
	cv::Mat						image_mat;						// image 
	cv::Mat						image_mat_backup;
	
	CPoissonFusion				poisson_editing;
	
	QPolygon					choosed_polygon;
	QPoint						paste_pos;

	// Pointer of child window
	ChildWindow					*source_window_;				// Source child window

	// Signs
	EnumDrawStatus				draw_status_;					// Enum type of draw status
	bool						is_choosing_;
	bool						is_choosing_polyedge;
	bool						is_pasting_;
	bool						is_pasting_holding;

};

