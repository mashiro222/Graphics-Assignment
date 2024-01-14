#include "minidraw.h"

MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	view_widget_ = new ViewWidget();
	CreateAction();
	CreateToolBar();
	CreateMenu();
	CreateStatusBar();
	setCentralWidget(view_widget_);
	view_widget_->setModeSelect();
}

void MiniDraw::CreateStatusBar()
{
	pStatusBar = new QStatusBar();
	setStatusBar(pStatusBar);
	pLabel_Type = new QLabel();
	pLabel_Type->setText("MiniDraw");
	pStatusBar->addWidget(pLabel_Type);

}

void MiniDraw::onAction_ColorTriggerred()
{
	QColor c = QColorDialog::getColor(QColor(spinBox_R->value(), spinBox_B->value(), spinBox_B->value()));
	spinBox_R->setValue(c.red());
	spinBox_G->setValue(c.green());
	spinBox_B->setValue(c.blue());
	SetColor();
}
void MiniDraw::CreatePenSettings()
{
	pLabel_Width = new QLabel();
	pLabel_Width->setText("    Pen Width ");
	pToolBar->addWidget(pLabel_Width);

	pen_width_slider_ = new QSlider();
	//pen_width_slider_->setGeometry(QRect(800, 5, 100, 15));
	pen_width_slider_->setOrientation(Qt::Horizontal);
	pen_width_slider_->setRange(1, 50);
	pToolBar->addWidget(pen_width_slider_);
	pen_width_slider_->setFixedSize(100, 15);

	connect(pen_width_slider_, &QSlider::valueChanged, this, &MiniDraw::SetPenWidth);

	pLabel_Color = new QLabel();
	pLabel_Color->setText("  RGB Value:");
	pToolBar->addWidget(pLabel_Color);

	spinBox_R = new QSpinBox();
	//spinBox_R->setGeometry(950, 2, 60, 20);
	spinBox_R->setRange(0, 255);
	spinBox_G = new QSpinBox();
	//spinBox_G->setGeometry(1030, 2, 60, 20);
	spinBox_G->setRange(0, 255);
	spinBox_B = new QSpinBox();
	//spinBox_B->setGeometry(1110, 2, 60, 20);
	spinBox_B->setRange(0, 255);
	pToolBar->addWidget(spinBox_R);
	pToolBar->addWidget(spinBox_G);
	pToolBar->addWidget(spinBox_B);
	connect(spinBox_R, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MiniDraw::SetColor);
	connect(spinBox_G, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MiniDraw::SetColor);
	connect(spinBox_B, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MiniDraw::SetColor);

	pToolBar->addAction(Action_Color);

	pCheckBox_Filled = new QCheckBox();
	pCheckBox_Filled->setText("Filled");
	connect(pCheckBox_Filled, &QCheckBox::clicked, this, &MiniDraw::onCheckBox_FilledClicked);
	pToolBar->addWidget(pCheckBox_Filled);
}

void MiniDraw::CreateAction()
{
	Action_About = new QAction(tr("&About"), this);
	Action_About->setIcon(QIcon("icon/guanyu.png"));
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);

	Action_Line = new QAction(tr("&Line"), this);
	Action_Line->setIcon(QIcon("icon/straight.png"));
	connect(Action_Line, &QAction::triggered, this, &MiniDraw::onAction_LineTriggered);


	Action_Rect = new QAction(tr("&Rect"), this);
	Action_Rect->setIcon(QIcon("icon/xingzhuang-juxing.png"));
	connect(Action_Rect, &QAction::triggered, this, &MiniDraw::onAction_RectTriggered);

	Action_Ellipse = new QAction(tr("&Ellipse"), this);
	Action_Ellipse->setIcon(QIcon("icon/tuoyuanxing.png"));
	connect(Action_Ellipse, &QAction::triggered, this, &MiniDraw::onAction_EllipseTriggered);

	Action_Polygon = new QAction(tr("&Polygon"), this);
	Action_Polygon->setIcon(QIcon("icon/vectorpolygon.png"));
	connect(Action_Polygon, &QAction::triggered, this, &MiniDraw::onAction_PolygonTriggered);

	Action_Freehand = new QAction(tr("&Freehand"), this);
	Action_Freehand->setIcon(QIcon("icon/quxian.png"));
	connect(Action_Freehand, &QAction::triggered, this, &MiniDraw::onAction_FreehandTriggered);

	Action_Bezier = new QAction(tr("&Bezier"), this);
	Action_Bezier->setIcon(QIcon("icon/curve.png"));
	connect(Action_Bezier, &QAction::triggered, this, &MiniDraw::onAction_BezierTriggered);

	Action_Select = new QAction(tr("&Select"), this);
	Action_Select->setIcon(QIcon("icon/shubiaozhizhen.png"));
	connect(Action_Select, &QAction::triggered, this, &MiniDraw::onAction_SelectTriggered);

	Action_Delete = new QAction(tr("&Delete"), this);
	Action_Delete->setIcon(QIcon("icon/delete.png"));
	connect(Action_Delete, &QAction::triggered, view_widget_, &ViewWidget::DoDelete);
	
	Action_Color = new QAction(tr("&Color"), this);
	Action_Color->setIcon(QIcon("icon/yanse.png"));
	connect(Action_Color, &QAction::triggered, this, &MiniDraw::onAction_ColorTriggerred);
}

void MiniDraw::CreateToolBar()
{
	pToolBar = addToolBar(tr("&Main"));
	pToolBar->addAction(Action_Select);
	pToolBar->addAction(Action_Line);
	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
	pToolBar->addAction(Action_Polygon);
	pToolBar->addAction(Action_Freehand);
	pToolBar->addAction(Action_Bezier);
	pToolBar->addAction(Action_Delete);
	
	CreatePenSettings();

}

void MiniDraw::CreateMenu()
{
	pMenu = menuBar()->addMenu(tr("&Figure Tool"));
	pMenu->setTitle(tr("&Figure Tool"));
	pMenu->addAction(Action_Line);
	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
	pMenu->addAction(Action_Polygon);
	pMenu->addAction(Action_Freehand);
	pMenu->addAction(Action_Bezier);
	pMenu->addAction(Action_Color);
	pMenu->addAction(Action_Select);
	pMenu->addAction(Action_Delete);

	
	pMenuAbout = menuBar()->addMenu(tr("&About"));
	pMenuAbout->setTitle(tr("&About"));
	pMenuAbout->addAction(Action_About);
}

void MiniDraw::AboutBox()
{
	QMessageBox::about(this, tr("About"), tr("MiniDraw By Wang Ruicheng, School of BigData. \n\rIcons are from the Internet. "));
}

void MiniDraw::SetPenWidth()
{
	view_widget_->setPenWidth(pen_width_slider_->value());
}

void MiniDraw::SetColor()
{
	QPalette pa;
	pa.setColor(QPalette::Background, QColor(spinBox_R->value(), spinBox_G->value(), spinBox_B->value()));
	pLabel_Color->setAutoFillBackground(true);
	pLabel_Color->setPalette(pa);
	if (!pCheckBox_Filled->isChecked())
	{
		view_widget_->setPenColor(QColor(spinBox_R->value(), spinBox_G->value(), spinBox_B->value()));
	}
	else
	{
		view_widget_->setFilled(QBrush(QColor(spinBox_R->value(), spinBox_G->value(), spinBox_B->value()),Qt::SolidPattern));
	}
	
}
MiniDraw::~MiniDraw()
{

}

void MiniDraw::onCheckBox_FilledClicked()
{
	if (pCheckBox_Filled->isChecked())
	{
		view_widget_->setFilled(QBrush(QColor(spinBox_R->value(), spinBox_G->value(), spinBox_B->value()),Qt::SolidPattern));
	}
	else
	{
		view_widget_->setFilled();
	}
}

void  MiniDraw::onAction_LineTriggered()
{
	view_widget_->setModeLine();
	pLabel_Type->setText("Line");
}
void MiniDraw::onAction_RectTriggered()
{
	view_widget_->setModeRect();
	pLabel_Type->setText("Rect");
}
void MiniDraw::onAction_EllipseTriggered()
{
	view_widget_->setModeEllipse();
	pLabel_Type->setText("Ellipse");
}
void MiniDraw::onAction_PolygonTriggered()
{
	view_widget_->setModePolygon();
	pLabel_Type->setText("Polygon");
}
void MiniDraw::onAction_FreehandTriggered()
{
	view_widget_->setModeFreehand();
	pLabel_Type->setText("Freehand");
}
void MiniDraw::onAction_BezierTriggered()
{
	view_widget_->setModeBezier();
	pLabel_Type->setText("Bezier");
}
void MiniDraw::onAction_SelectTriggered()
{
	view_widget_->setModeSelect();
	pLabel_Type->setText("Select");
}