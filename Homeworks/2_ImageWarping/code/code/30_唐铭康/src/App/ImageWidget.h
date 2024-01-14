#pragma once
#include <QWidget>
#include "WarpingIDW.h"
#include "WarpingRBF.h"

QT_BEGIN_NAMESPACE
	class	QImage;
	class	QPainter;
QT_END_NAMESPACE

enum class WarpType
{
	IDW,
	RBF,
	None,
	Setting,
};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget();
	~ImageWidget();

protected:
	void	paintEvent(QPaintEvent*);

private:
	void	mousePressEvent(QMouseEvent* event);
	void	mouseMoveEvent(QMouseEvent* event);
	void	mouseReleaseEvent(QMouseEvent* event);

	void	draw_InputLine(const QLine& line);

public:
	void	Mirror(bool ishorizontal, bool isvertical);			// Mirror image horizontally or vertically
	void	DoWarp(int setgap = 0, bool warpPoints = false);

public slots:

	// File IO
	QImage*	Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void	Save();												// Save image to current file
	void	SaveAs();											// Save image to another file


	// Image processing
	void	Invert();											// Invert pixel value in image
	void	Mirror_H();											// Mirror image horizontally
	void	Mirror_V();											// Mirror image vertically
	void	TurnGray();											// Turn image to gray-scale map
	void	Restore();											// Restore image to origin
	void	Undo();												// Revoke an operation
	void    StartSetting();										// Start to set warping data
	void	IDWWarping(int setgap = 0, bool warpPoints = false);// Warp the image with inverse distance-weighted function
	void	RBFWarping(int setgap = 0, bool warpPoints = false);// Warp the image with inverse radius basis function
	void	ClearSetting();										// Clear all anchor points
	int		Change_Visibility();								// Hide or show input points
	void	set_ratio(double ratio);							// Set image size ratio

private:
	QImage*				ptr_image_;				// image 
	QImage*				ptr_image_backup_;		// backup of original image
	QVector<QImage*>	ptr_image_history_;		// backup of image processing
	QString				file_name_;				// file

	int		left();
	int		top();
	double	image_ratio_ = 1.0;

	void	SaveHistory();		// Add current image to history

	bool	visible = true;

private:
	bool				draw_status_;
	QLine*				current_line_;						
	WarpType			warp_type_;

private:
	QVector<QLine*>		pair_array_;			// (pi, qi)
};