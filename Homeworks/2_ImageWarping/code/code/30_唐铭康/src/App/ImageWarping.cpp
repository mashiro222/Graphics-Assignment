#include "ImageWarping.h"
#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "ImageWidget.h"
#include <iostream>

ImageWarping::ImageWarping(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	imagewidget_ = new ImageWidget;
	setCentralWidget(imagewidget_);

	CreateActions();
	ConnectActionsToWidget();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();
}

ImageWarping::~ImageWarping()
{
}

void ImageWarping::CreateActions()
{
	action_open_ = new QAction(QIcon("Resources/images/open.png"), tr("&Open..."), this);
	action_open_->setShortcuts(QKeySequence::Open);
	action_open_->setStatusTip(tr("Open an existing file"));

	action_save_ = new QAction(QIcon("Resources/images/save.png"), tr("&Save"), this);
	action_save_->setShortcuts(QKeySequence::Save);
	action_save_->setStatusTip(tr("Save the document to disk"));

	action_saveas_ = new QAction(tr("Save &As..."), this);
	action_saveas_->setShortcuts(QKeySequence::SaveAs);
	action_saveas_->setStatusTip(tr("Save the document under a new name"));

	action_invert_ = new QAction(QIcon("Resources/images/inverse.png"), tr("Inverse"), this);
	action_invert_->setStatusTip(tr("Invert all pixel value in the image"));

	action_mirror_h_ = new QAction(QIcon("Resources/images/mirror_h.png"), tr("Mirror(H)"), this);
	action_mirror_h_->setStatusTip(tr("Mirror image horizontally"));

	action_mirror_v_ = new QAction(QIcon("Resources/images/mirror_v.png"), tr("Mirror(V)"), this);
	action_mirror_v_->setStatusTip(tr("Mirror image vertically"));

	action_gray_ = new QAction(QIcon("Resources/images/grey.png"), tr("Grayscale"), this);
	action_gray_->setStatusTip(tr("Gray-scale map"));

	action_restore_ = new QAction(QIcon("Resources/images/restore.png"), tr("Restore"), this);
	action_restore_->setStatusTip(tr("Show origin image"));

	action_undo_ = new QAction(QIcon("Resources/images/undo.png"), tr("Undo"), this);
	action_undo_->setShortcuts(QKeySequence::Undo);
	action_undo_->setStatusTip(tr("Revoke an operation "));

	action_close_= new QAction(QIcon("Resources/images/close.png"), tr("Close"), this);
	action_close_->setShortcuts(QKeySequence::Close);
	action_close_->setStatusTip(tr("Close the file "));

	action_start_setting_ = new QAction(tr("SetWarp"), this);
	action_start_setting_->setStatusTip(tr("Start setting warping data"));

	action_idw_ = new QAction(tr("IDW"), this);
	action_idw_->setStatusTip(tr("Warp the image with inverse distance-weighted function"));

	action_rbf_ = new QAction(tr("RBF"), this);
	action_rbf_->setStatusTip(tr("Warp the image with radius basis function"));

	action_clear_ = new QAction(tr("ClearWarp"), this);
	action_clear_->setStatusTip(tr("Clear all input points"));

	action_visibility_= new QAction(QIcon("Resources/images/Invisible.png"), tr("HidePoints"), this);
	action_visibility_->setStatusTip(tr("Hide input points"));

	connect(action_open_, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(action_close_, SIGNAL(triggered()), this, SLOT(Close()));
	connect(action_idw_, SIGNAL(triggered()), this, SLOT(CallIDW()));
	connect(action_rbf_, SIGNAL(triggered()), this, SLOT(CallRBF()));
	connect(action_visibility_, SIGNAL(triggered()), this, SLOT(Change_Visibility()));
}

void ImageWarping::ConnectActionsToWidget()
{
	connect(action_save_, SIGNAL(triggered()), imagewidget_, SLOT(Save()));
	connect(action_saveas_, SIGNAL(triggered()), imagewidget_, SLOT(SaveAs()));
	connect(action_invert_, SIGNAL(triggered()), imagewidget_, SLOT(Invert()));
	connect(action_mirror_h_, SIGNAL(triggered()), imagewidget_, SLOT(Mirror_H()));
	connect(action_mirror_v_, SIGNAL(triggered()), imagewidget_, SLOT(Mirror_V()));
	connect(action_gray_, SIGNAL(triggered()), imagewidget_, SLOT(TurnGray()));
	connect(action_restore_, SIGNAL(triggered()), imagewidget_, SLOT(Restore()));
	connect(action_undo_, SIGNAL(triggered()), imagewidget_, SLOT(Undo()));
	connect(action_start_setting_, SIGNAL(triggered()), imagewidget_, SLOT(StartSetting()));
	connect(action_clear_, SIGNAL(triggered()), imagewidget_, SLOT(ClearSetting()));
}

void ImageWarping::CreateMenus()
{
	// File menu
	menu_file_ = menuBar()->addMenu(tr("&File"));
	menu_file_->setStatusTip(tr("File menu"));
	menu_file_->addAction(action_open_);
	menu_file_->addAction(action_save_);
	menu_file_->addAction(action_saveas_);
	menu_file_->addAction(action_close_);

	// Edit menu
	menu_edit_ = menuBar()->addMenu(tr("&Edit"));
	menu_edit_->setStatusTip(tr("Edit menu"));
	menu_edit_->addAction(action_undo_);
	menu_edit_->addAction(action_restore_);
	menu_edit_->addSeparator();
	menu_edit_->addAction(action_invert_);
	menu_edit_->addAction(action_mirror_h_);
	menu_edit_->addAction(action_mirror_v_);
	menu_edit_->addAction(action_gray_);

	// Warp menu
	menu_warp_ = menu_edit_->addMenu(tr("Warp"));
	menu_warp_->addAction(action_start_setting_);
	menu_warp_->addAction(action_idw_);
	menu_warp_->addAction(action_rbf_);
	menu_warp_->addAction(action_clear_);
	menu_warp_->addAction(action_visibility_);
}

void ImageWarping::CreateToolBars()
{
	// File toolbar
	toolbar_file_ = addToolBar(tr("File"));
	toolbar_file_->addAction(action_open_);
	toolbar_file_->addAction(action_save_);
	toolbar_file_->addAction(action_close_);

	// Add separator in toolbar 
	toolbar_file_->addSeparator();
	toolbar_file_->addAction(action_undo_);
	toolbar_file_->addAction(action_restore_);

	// Edit toolbar
	toolbar_edit_ = addToolBar(tr("Edit"));
	toolbar_edit_->addAction(action_invert_);
	toolbar_edit_->addAction(action_mirror_h_);
	toolbar_edit_->addAction(action_mirror_v_);
	toolbar_edit_->addAction(action_gray_);

	// Add separator in toolbar
	toolbar_edit_->addSeparator();
	toolbar_edit_->addAction(action_start_setting_);
	toolbar_edit_->addAction(action_idw_);
	toolbar_edit_->addAction(action_rbf_);
	toolbar_edit_->addAction(action_clear_);
	toolbar_edit_->addAction(action_visibility_);

	// Settings toolbar
	this->addToolBarBreak(Qt::TopToolBarArea);
	toolbar_settings_ = addToolBar(tr("Settings"));
	spinbox_prefix_ = new QLabel;
	spinbox_prefix_->setText(tr("Grid size: "));
	toolbar_settings_->addWidget(spinbox_prefix_);
	spinbox_ = new QSpinBox;
	spinbox_->setMinimum(-1);
	spinbox_->setMaximum(-1);
	spinbox_->setValue(-1);
	toolbar_settings_->addWidget(spinbox_);

	// Add separator in toolbar
	toolbar_settings_->addSeparator();
	checkbox_warp_prefix_ = new QLabel;
	checkbox_warp_prefix_->setText("Warp input points: ");
	toolbar_settings_->addWidget(checkbox_warp_prefix_);
	checkbox_warp_ = new QCheckBox;
	checkbox_warp_->setCheckState(Qt::Unchecked);
	toolbar_settings_->addWidget(checkbox_warp_);

	// Add separator in toolbar
	toolbar_settings_->addSeparator();
	slider_imgsize_prefix_ = new QLabel;
	slider_imgsize_prefix_->setText(tr("Image Size: 100% "));
	toolbar_settings_->addWidget(slider_imgsize_prefix_);
	slider_imgsize_ = new QSlider;
	slider_imgsize_->setOrientation(Qt::Horizontal); 
	slider_imgsize_->setFixedWidth(100);
	slider_imgsize_->setMinimum(1);  
	slider_imgsize_->setMaximum(300);  
	slider_imgsize_->setValue(100);
	slider_imgsize_->setSingleStep(1);  
	connect(slider_imgsize_, SIGNAL(valueChanged(int)), this, SLOT(setImageSize(int)));
	toolbar_settings_->addWidget(slider_imgsize_);

	// Set font
	toolbar_file_->setFont(QFont::QFont("Times New Roman"));
	toolbar_edit_->setFont(QFont::QFont("Times New Roman"));
	toolbar_settings_->setFont(QFont::QFont("Times New Roman"));
}

void ImageWarping::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void ImageWarping::OpenFile()
{
	QImage* image_ = imagewidget_->Open();
	if (image_)
	{
		int min = image_->width();
		if (image_->height() < min)
		{
			min = image_->height();
		}
		spinbox_->setMinimum(1);
		spinbox_->setMaximum(min - 1);
		spinbox_->setValue(1 + min / 20);
	}

	image_ = Q_NULLPTR;
	return;
}

void ImageWarping::CallIDW()
{
	imagewidget_->IDWWarping(spinbox_->value(), checkbox_warp_->checkState() == Qt::Checked);
}

void ImageWarping::CallRBF()
{
	imagewidget_->RBFWarping(spinbox_->value(), checkbox_warp_->checkState() == Qt::Checked);
}

void ImageWarping::Change_Visibility()
{
	int visible = imagewidget_->Change_Visibility();
	if (visible)
	{
		action_visibility_->setText("HidePoints");
		action_visibility_->setIcon(QIcon("Resources/images/Invisible.png"));
	}
	else
	{
		action_visibility_->setText("ShowPoints");
		action_visibility_->setIcon(QIcon("Resources/images/Visible.png"));
	}
}

void ImageWarping::setImageSize(int value)
{
	QString text = "Image Size: ";
	if (value < 100)
	{
		text += " ";
		if (value < 10)
		{
			text += " ";
		}
	}
	text += QString::number(value);
	text += "% ";
	slider_imgsize_prefix_->setText(text);
	imagewidget_->set_ratio(value / 100.0);
}

void ImageWarping::Close()
{
	disconnect(action_save_, SIGNAL(triggered()), imagewidget_, SLOT(Save()));
	disconnect(action_saveas_, SIGNAL(triggered()), imagewidget_, SLOT(SaveAs()));
	disconnect(action_invert_, SIGNAL(triggered()), imagewidget_, SLOT(Invert()));
	disconnect(action_mirror_h_, SIGNAL(triggered()), imagewidget_, SLOT(Mirror_H()));
	disconnect(action_mirror_v_, SIGNAL(triggered()), imagewidget_, SLOT(Mirror_V()));
	disconnect(action_gray_, SIGNAL(triggered()), imagewidget_, SLOT(TurnGray()));
	disconnect(action_restore_, SIGNAL(triggered()), imagewidget_, SLOT(Restore()));
	disconnect(action_undo_, SIGNAL(triggered()), imagewidget_, SLOT(Undo()));
	disconnect(action_start_setting_, SIGNAL(triggered()), imagewidget_, SLOT(StartSetting()));
	delete imagewidget_;

	imagewidget_ = new ImageWidget;
	setCentralWidget(imagewidget_);
	ConnectActionsToWidget();

	spinbox_->setMinimum(-1);
	spinbox_->setValue(-1);
	spinbox_->setMaximum(-1);	
}