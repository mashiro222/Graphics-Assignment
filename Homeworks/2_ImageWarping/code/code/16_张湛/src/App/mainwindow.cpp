#include "mainwindow.h"
#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "ImageWidget.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	//ui.setupUi(this);

	setGeometry(300, 150, 800, 450);

	imagewidget_ = new ImageWidget();
	setCentralWidget(imagewidget_);

	CreateActions();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *e)
{

}

void MainWindow::paintEvent(QPaintEvent* paintevent)
{
	
}

void MainWindow::AboutImageWarping()
{
	QMessageBox::information(NULL, "About",
		QString::fromLocal8Bit(
			"PB17000123 张湛\n"\
			"注：\n"\
			"“IDW”只有在出现三个及以上控制点时才会计算，否则无解\n"\
			"“RBF”可以对鼠标右键画出的affineset进行响应，对应于论文中出现的仿射变换\n"\
			"“CP”会同时显示IDW和RBF的结果，用于方便比较\n"\
			"本程序没有事先固定图片四角，用两种方法实现Fillhole见报告\n"\
			"（PS：我ANN不知为何用不了，自己用kdtree实现了一个KNN算法填补空白）\n"
		),
		QMessageBox::Yes
	);
}

void MainWindow::CreateActions()
{
	minidraw_about_action_ = new QAction(tr("&About MiniDraw..."), this);
	minidraw_about_action_->setStatusTip(tr("About MiniDraw..."));
	connect(minidraw_about_action_, &QAction::triggered, this, &MainWindow::AboutImageWarping);

	action_new_ = new QAction(QIcon(":/MainWindow/Resources/images/new.jpg"), tr("&New"), this);
	action_new_->setShortcut(QKeySequence::New);
	action_new_->setStatusTip(tr("Create a new file"));
	// connect ...

	action_open_ = new QAction(QIcon(":/MainWindow/Resources/images/open.jpg"), tr("&Open..."), this);
	action_open_->setShortcuts(QKeySequence::Open);
	action_open_->setStatusTip(tr("Open an existing file"));
	connect(action_open_, &QAction::triggered, imagewidget_, &ImageWidget::Open);

	action_save_ = new QAction(QIcon(":/MainWindow/Resources/images/save.jpg"), tr("&Save"), this);
	action_save_->setShortcuts(QKeySequence::Save);
	action_save_->setStatusTip(tr("Save the document to disk"));
	// connect ...

	action_saveas_ = new QAction(tr("Save &As..."), this);
	action_saveas_->setShortcuts(QKeySequence::SaveAs);
	action_saveas_->setStatusTip(tr("Save the document under a new name"));
	connect(action_saveas_, &QAction::triggered, imagewidget_, &ImageWidget::SaveAs);

	action_invert_ = new QAction(tr("Inverse"), this);
	action_invert_->setStatusTip(tr("Invert all pixel value in the image"));
	connect(action_invert_, &QAction::triggered, imagewidget_, &ImageWidget::Invert);

	action_mirror_ = new QAction(tr("Mirror"), this);
	action_mirror_->setStatusTip(tr("Mirror image vertically or horizontally"));
	// The slot requires more arguments than the signal provides.
	//connect(action_mirror_, &QAction::triggered, imagewidget_, &ImageWidget::Mirror);
	connect(action_mirror_, &QAction::triggered, [this]() {
		imagewidget_->Mirror(); // use default arguments
		});

	action_gray_ = new QAction(tr("Grayscale"), this);
	action_gray_->setStatusTip(tr("Gray-scale map"));
	connect(action_gray_, &QAction::triggered, imagewidget_, &ImageWidget::TurnGray);

	action_restore_ = new QAction(tr("Restore"), this);
	action_restore_->setStatusTip(tr("Show origin image"));
	connect(action_restore_, &QAction::triggered, imagewidget_, &ImageWidget::Restore);
	
	action_set_controlpoints_ = new QAction(tr("ControlPoints"), this);
	action_set_controlpoints_->setStatusTip(tr("Set Control Points"));
	connect(action_set_controlpoints_, SIGNAL(triggered()), imagewidget_, SLOT(Set_ControlPoints()));

	action_idw_imagewarping_ = new QAction(tr("IDW"), this);
	action_idw_imagewarping_->setStatusTip(tr("Inverse distance-weighted interpolation image warping methods"));
	connect(action_idw_imagewarping_, SIGNAL(triggered()), imagewidget_, SLOT(IDW_ImageWarping()));

	action_rbf_imagewarping_ = new QAction(tr("RBF"), this);
	action_rbf_imagewarping_->setStatusTip(tr("Radial basis functions image warping methods"));
	connect(action_rbf_imagewarping_, SIGNAL(triggered()), imagewidget_, SLOT(RBF_ImageWarping()));

	action_cp_imagewarping_ = new QAction(tr("CP"), this);
	action_cp_imagewarping_->setStatusTip(tr("Compare IDW and RBF"));
	connect(action_cp_imagewarping_, SIGNAL(triggered()), imagewidget_, SLOT(CP_ImageWarping()));
}

void MainWindow::CreateMenus()
{
	menu_file_ = menuBar()->addMenu(tr("&File"));
	menu_file_->setStatusTip(tr("File menu"));
	menu_file_->addAction(minidraw_about_action_);
	menu_file_->addAction(action_new_);
	menu_file_->addAction(action_open_);
	menu_file_->addAction(action_save_);
	menu_file_->addAction(action_saveas_);

	menu_edit_ = menuBar()->addMenu(tr("&Edit"));
	menu_edit_->setStatusTip(tr("Edit menu"));
	menu_edit_->addAction(action_invert_);
	menu_edit_->addAction(action_mirror_);
	menu_edit_->addAction(action_gray_);
	menu_edit_->addAction(action_restore_);
	menu_edit_->addAction(action_set_controlpoints_);
	menu_edit_->addAction(action_idw_imagewarping_);
	menu_edit_->addAction(action_rbf_imagewarping_);
	menu_edit_->addAction(action_cp_imagewarping_);
}

void MainWindow::CreateToolBars()
{
	toolbar_file_ = addToolBar(tr("File"));
	toolbar_file_->addAction(action_new_);
	toolbar_file_->addAction(action_open_);
	toolbar_file_->addAction(action_save_);

	// Add separator in toolbar 
	toolbar_file_->addSeparator();
	toolbar_file_->addAction(action_invert_);
	toolbar_file_->addAction(action_mirror_);
	toolbar_file_->addAction(action_gray_);
	toolbar_file_->addAction(action_restore_);
	
	toolbar_file_->addSeparator();
	toolbar_file_->addAction(action_set_controlpoints_);
	toolbar_file_->addAction(action_idw_imagewarping_);
	toolbar_file_->addAction(action_rbf_imagewarping_);
	toolbar_file_->addAction(action_cp_imagewarping_);
}

void MainWindow::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}
