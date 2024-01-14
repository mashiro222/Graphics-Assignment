#include "minidraw.h"
// 重新扫描解决方案 可以去除不必要的不完整类型的报错

MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this); // UI 初始化，就在 ui_minidraw.h 里定义了，必须最先调用

	// 颜色盘
	// pDial = new QDial();
	// pDial->setRange(1, 20);
	// pDial->setBaseSize(QSize());
	// pDial->setNotchesVisible(true);

	pSizeComoBox = new QComboBox(this);
	for (int i = 1; i <= 20; i++)
	{
		char string[5];
		pSizeComoBox->addItem(tr(itoa(i, string, 10)));
	}

	// 画布
	view_widget_ = new ViewWidget();
	Creat_Action();
	Creat_ToolBar();
	Creat_Menu();

	setCentralWidget(view_widget_);
}

void MiniDraw::Creat_Action()
{
	Action_About = new QAction(tr("&About"), this);
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);

	Action_Line = new QAction(tr("&Line"), this);
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

	Action_Rect = new QAction(tr("&Rect"), this);
	connect(Action_Rect, &QAction::triggered, view_widget_, &ViewWidget::setRect);

	Action_Ellipse = new QAction(tr("&Ellipse"), this);
	connect(Action_Ellipse, &QAction::triggered, view_widget_, &ViewWidget::setEllipse);

	Action_Polygon = new QAction(tr("&Polygon"), this);
	connect(Action_Polygon, &QAction::triggered, view_widget_, &ViewWidget::setPolygon);

	Action_FreeHand = new QAction(tr("&FreeHand"), this);
	connect(Action_FreeHand, &QAction::triggered, view_widget_, &ViewWidget::setFreehand);

	Action_Undo = new QAction(tr("&Undo"), this);
	connect(Action_Undo, &QAction::triggered, view_widget_, &ViewWidget::setUndo);

	Action_ChangeColor = new QAction(tr("&Color"), this);
	connect(Action_ChangeColor, &QAction::triggered, view_widget_, &ViewWidget::setColor);

	// Action_ChangeSize = new QAction(tr("&Width"), this);

	connect(pSizeComoBox, SIGNAL(currentIndexChanged(int)), view_widget_, SLOT(setPenWidth(int)));
	//connect(pDial, &QDial::valueChanged, view_widget_, &ViewWidget::setPenWidth);

	// Action_Undo = new QAction()
}

void MiniDraw::Creat_ToolBar()
{
	pToolBar = addToolBar(tr("&Main"));
	pToolBar->addAction(Action_About);
	pToolBar->addWidget(pSizeComoBox);
	pToolBar->addAction(Action_ChangeColor);
	pToolBar->addAction(Action_Line);
	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
	pToolBar->addAction(Action_Polygon);
	pToolBar->addAction(Action_FreeHand);
	pToolBar->addAction(Action_Undo);
	// pToolBar->addAction()
}

void MiniDraw::Creat_Menu()
{
	pMenu = menuBar()->addMenu(tr("&Figure Tool"));
	pMenu->addAction(Action_About);
	pMenu->addAction(Action_Line);
	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
	pMenu->addAction(Action_Polygon);
	pMenu->addAction(Action_FreeHand);
	pMenu->addAction(Action_Undo);
	// pMenu->addAction(Action_ChangeSize);
}

void MiniDraw::AboutBox()
{
	QMessageBox::about(this, tr("About"), tr("MiniDraw"));
}

MiniDraw::~MiniDraw()
{
	delete view_widget_;
	delete pSizeComoBox;
}
