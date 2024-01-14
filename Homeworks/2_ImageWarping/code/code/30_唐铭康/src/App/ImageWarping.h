#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ImageWarping.h"

QT_BEGIN_NAMESPACE
	class	QAction;
	class	QMenu;
	class	ViewWidget;
	class	QImage;
	class	QPainter;
	class	QRect;
	class	ImageWidget;
	class   QLabel;
	class   QSpinBox;
	class   QCheckBox;
	class   QSlider;
QT_END_NAMESPACE

class ImageWarping : public QMainWindow
{
	Q_OBJECT

public:
	ImageWarping(QWidget *parent = Q_NULLPTR);
	~ImageWarping();

private:
	void	CreateActions();
	void	ConnectActionsToWidget();
	void	CreateMenus();
	void	CreateToolBars();
	void	CreateStatusBar();

private:
	Ui::ImageWarpingClass	ui;

	QMenu*		menu_file_;
	QMenu*		menu_edit_;
	QMenu*		menu_warp_;
	QToolBar*	toolbar_file_;
	QToolBar*	toolbar_edit_;
	QToolBar*	toolbar_settings_;
	QAction*	action_open_;
	QAction*	action_save_;
	QAction*	action_saveas_;
	QAction*	action_invert_;
	QAction*	action_mirror_h_;
	QAction*	action_mirror_v_;
	QAction*	action_gray_;
	QAction*	action_restore_;
	QAction*	action_undo_;
	QAction*	action_close_;
	QAction*	action_start_setting_;
	QAction*	action_idw_;
	QAction*	action_rbf_;
	QAction*	action_clear_;
	QAction*	action_visibility_;
	QLabel*		spinbox_prefix_;
	QSpinBox*	spinbox_;
	QLabel*		checkbox_warp_prefix_;
	QCheckBox*  checkbox_warp_;
	QLabel*		slider_imgsize_prefix_;
	QSlider*    slider_imgsize_;

public:
	ImageWidget*	imagewidget_;

private slots:
	void	Close();		// Close the file
	void	OpenFile();		// Open a file
	void	CallIDW();		// Call IDWWarping()
	void	CallRBF();		// Call RBFWarping()
	void	Change_Visibility();	// Hide or show the input points
	void	setImageSize(int value);	// Set the size proportion of the image
};
