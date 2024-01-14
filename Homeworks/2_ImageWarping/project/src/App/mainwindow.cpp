#include "mainwindow.h"
#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "ImageWidget.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	//ui.setupUi(this);

	setGeometry(300, 150, 800, 450);//设置窗口大小和位置

	imagewidget_ = new ImageWidget();//创建一个自定义小部件
	setCentralWidget(imagewidget_);//将其设置为主窗口的中心部分

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
	//todo
}

void MainWindow::paintEvent(QPaintEvent* paintevent)
{
	//todo
}

void MainWindow::CreateActions()
{
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

	action_IDW_ = new QAction(tr("IDWWarping"), this);
	action_IDW_->setStatusTip(tr("Use IDW warper"));
	connect(action_IDW_, &QAction::triggered, imagewidget_, &ImageWidget::IDWWarp);

	action_RBF_ = new QAction(tr("RBFWarping"), this);
	action_RBF_->setStatusTip(tr("Use RBF warper"));
	connect(action_RBF_, &QAction::triggered, imagewidget_, &ImageWidget::RBFWarp);

	action_clear_ = new QAction(tr("Clear"), this);
	action_clear_->setStatusTip(tr("Clear paints"));
	connect(action_clear_, &QAction::triggered, imagewidget_, &ImageWidget::Clear);

	action_fillwhitegap_ = new QAction(tr("FillGap"), this);
	action_fillwhitegap_->setStatusTip(tr("Fill white gap"));
	connect(action_fillwhitegap_, &QAction::triggered, imagewidget_, &ImageWidget::Fillgap);

	action_loadDebug_ = new QAction(tr("Debug"), this);
	action_loadDebug_->setStatusTip(tr("debug"));
	connect(action_loadDebug_, &QAction::triggered, imagewidget_, &ImageWidget::loadDebug);
}

void MainWindow::CreateMenus()
{
	menu_file_ = menuBar()->addMenu(tr("&File"));
	menu_file_->setStatusTip(tr("File menu"));
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

	menu_edit_->addAction(action_IDW_);
	menu_edit_->addAction(action_RBF_);
	menu_edit_->addAction(action_clear_);
	menu_edit_->addAction(action_fillwhitegap_);
	menu_edit_->addAction(action_loadDebug_);
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

	toolbar_file_->addAction(action_IDW_);
	toolbar_file_->addAction(action_RBF_);
	toolbar_file_->addAction(action_clear_);
	toolbar_file_->addAction(action_fillwhitegap_);
	toolbar_file_->addAction(action_loadDebug_);

}

void MainWindow::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}
