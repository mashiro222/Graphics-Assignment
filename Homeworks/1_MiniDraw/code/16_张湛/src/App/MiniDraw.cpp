#include "MiniDraw.h"

MiniDraw::MiniDraw(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.mainToolBar->setHidden(true);
	Init();
}


void MiniDraw::setLineEditValue(int value)
{
	int pos = size_slider->value();
	QString str = QString("%1").arg(pos);
	lineEdit->setText(" Size: " + str);
}

void MiniDraw::setColorEditValue(QColor c)
{
	QString qstr = QString::fromStdString("background-color:rgb(" + std::to_string(view_widget_->getColor().red()) + "," + std::to_string(view_widget_->getColor().green()) + "," + std::to_string(view_widget_->getColor().blue()) + ")");
	colorEdit->setStyleSheet(qstr);
}

void MiniDraw::About()
{
	QMessageBox::information(NULL, "About", 
		QString::fromLocal8Bit(
			"PB17000123 张湛\n"\
			"注：\n"\
			"“Color”可以选择绘制颜色，在“chose”下调整选中图元颜色\n"\
			"“Size”可以调整笔刷大小，在“chose”下调整选中图元边线大小\n"\
			"“Save”会保存为“MiniDraw.png”\n"\
			"“Ploygon”以单击鼠标右键结束。\n"\
			"其余图像若用鼠标右键画，则绘制对称的图像（圆、正方形）\n"\
			"“Chose”左键拉动可以选择图元，右键可以拖动选中图元\n"\
			"“Delete”删除选中图元\n"\
			"“Undo”撤销上一步所画图元\n"
		),
		QMessageBox::Yes
	);
}

void MiniDraw::Save()
{
	QPixmap pixmap(view_widget_->size());
	view_widget_->render(&pixmap);//保存当前窗口
	pixmap.save("MiniDraw.png");
}

void MiniDraw::Init()
{
	view_widget_ = new ViewWidget(this);

	about_action_ = new QAction(tr("&About"), this);
	color_action_ = new QAction(tr("&Color:"), this);
	line_action_ = new QAction(tr("&Line"), this);
	rectangle_action_ = new QAction(tr("&Rectangle"), this);
	ellipse_action_ = new QAction(tr("&Ellipse"), this);
	polygon_action_ = new QAction(tr("&Polygon"), this);
	freehand_action_ = new QAction(tr("&Freehand"), this);
	undo_action_ = new QAction(tr("&Undo"), this);
	save_action_ = new QAction(tr("&Save"), this);
	chose_action_ = new QAction(tr("&Chose"), this);
	delete_action_ = new QAction(tr("&Delete"), this);

	connect(about_action_, &QAction::triggered, this, &MiniDraw::About);
	connect(color_action_, &QAction::triggered, view_widget_, &ViewWidget::showColor);
	connect(color_action_, &QAction::triggered, this, &MiniDraw::setColorEditValue);
	connect(line_action_, &QAction::triggered, view_widget_, &ViewWidget::set_figure_type_to_line);
	connect(rectangle_action_, &QAction::triggered, view_widget_, &ViewWidget::set_figure_type_to_rectangle);
	connect(ellipse_action_, &QAction::triggered, view_widget_, &ViewWidget::set_figure_type_to_ellipse);
	connect(polygon_action_, &QAction::triggered, view_widget_, &ViewWidget::set_figure_type_to_polygon);
	connect(freehand_action_, &QAction::triggered, view_widget_, &ViewWidget::set_figure_type_to_freehand);
	connect(undo_action_, &QAction::triggered, view_widget_, &ViewWidget::undo_current_figure);
	connect(save_action_, &QAction::triggered, this, &MiniDraw::Save);
	connect(chose_action_, &QAction::triggered, view_widget_, &ViewWidget::activate_chose_mod);
	connect(delete_action_, &QAction::triggered, view_widget_, &ViewWidget::delete_chosed_figure);

	main_menu_ = menuBar()->addMenu(tr("&Main"));
	main_menu_->addAction(about_action_);
	main_menu_->addAction(delete_action_);
	main_menu_->addAction(undo_action_);
	main_menu_->addAction(save_action_);


	main_toolbar_ = addToolBar(tr("&Main Tool"));
	main_toolbar_->addAction(about_action_);
	main_toolbar_->addAction(chose_action_);
	main_toolbar_->addAction(delete_action_);
	main_toolbar_->addAction(undo_action_);
	main_toolbar_->addAction(save_action_);
	main_toolbar_->addSeparator();
	main_toolbar_->addAction(color_action_);

	this->addToolBarBreak(Qt::TopToolBarArea);
	draw_toolbar_ = addToolBar(tr("&Figure Tool"));
	draw_toolbar_->addAction(line_action_);
	draw_toolbar_->addAction(rectangle_action_);
	draw_toolbar_->addAction(ellipse_action_);
	draw_toolbar_->addAction(polygon_action_);
	draw_toolbar_->addAction(freehand_action_);

	setCentralWidget(view_widget_);
	
	colorEdit = new QLineEdit();
	colorEdit->setFixedWidth(15);
	colorEdit->setFixedHeight(15);
	QString qstr = QString::fromStdString("background-color:rgb(" + std::to_string(view_widget_->getColor().red()) + "," + std::to_string(view_widget_->getColor().green()) + "," + std::to_string(view_widget_->getColor().blue()) + ")");
	colorEdit->setStyleSheet(qstr);
	colorEdit->setReadOnly(true);

	main_toolbar_->addWidget(colorEdit);

	lineEdit = new QLineEdit(" Size: 1");
	lineEdit->setFixedWidth(80);
	lineEdit->setStyleSheet("border:none; background-color:rgb(240,240,240)");
	lineEdit->setReadOnly(true);

	size_slider = new QSlider(Qt::Horizontal);
	size_slider->setMinimum(1);
	size_slider->setMaximum(10);
	size_slider->setValue(1);
	size_slider->setFixedWidth(100);
	size_slider->setStyleSheet("\
		QSlider{\
		border-color: #bcbcbc;\
		}\
		QSlider::groove:horizontal\
		{\
			height: 8px;\
			background: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #B1B1B1, stop:1 #c4c4c4);\
			margin: 2px 0\
		}\
		QSlider::handle:horizontal\
		{\
			background: QRadialGradient(cx : 0, cy : 0, radius : 1, fx : 0.5, fy : 0.5, stop : 0 white, stop : 1 green);\
			width: 5px;\
			border-radius: 11px;\
			margin: -5px 6px - 5px 6px;\
			border: 3px solid #ffffff;\
		}\
		QSlider::add-page:horizontal{\
			background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #bcbcbc, stop:0.25 #bcbcbc, stop:0.5 #bcbcbc, stop:1 #bcbcbc); \
			height:3px;\
		}\
		QSlider::sub-page:horizontal{\
			background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #439cf3, stop:0.25 #439cf3, stop:0.5 #439cf3, stop:1 #439cf3);\
			height:3px;\
		}\
	");
	connect(size_slider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue(int)));
	connect(size_slider, SIGNAL(valueChanged(int)), view_widget_, SLOT(setSize(int)));

	main_toolbar_->addWidget(lineEdit);
	main_toolbar_->addWidget(size_slider);
	
}
