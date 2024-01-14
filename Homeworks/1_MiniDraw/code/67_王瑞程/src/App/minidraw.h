#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSpinBox>
#include <qmessagebox.h>
#include <ui_minidraw.h>
#include <viewwidget.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QCheckBox>

class MiniDraw : public QMainWindow
{
	Q_OBJECT

public:
	MiniDraw(QWidget *parent = 0);
	~MiniDraw();

	QMenu		*pMenu;
	QMenu		*pMenuAbout;
	QToolBar	*pToolBar;
	QAction		*Action_About;
	QAction		*Action_Line;
	QAction		*Action_Rect;
	QAction		*Action_Ellipse;
	QAction		*Action_Polygon;
	QAction		*Action_Freehand;
	QAction		*Action_Select;
	QAction		*Action_Delete;
	QStatusBar	*pStatusBar;
	QLabel		*pLabel_Width;
	QLabel		*pLabel_Type;
	QLabel		*pLabel_PosMsg;
	QLabel		*pLabel_Color;
	QCheckBox	*pCheckBox_Filled;
	QAction		*Action_Color;
	QAction		*Action_Bezier;

	void CreateMenu();
	void CreateToolBar();
	void CreateAction();
	void CreatePenSettings();
	void CreateStatusBar();

	void AboutBox();
	void SetPenWidth();
	void SetColor();				//设置默认颜色。当Filled未选中时，改变画笔颜色；否则改变笔刷颜色。
	void onAction_ColorTriggerred();
	void onCheckBox_FilledClicked();
	void onAction_LineTriggered();
	void onAction_RectTriggered();
	void onAction_EllipseTriggered();
	void onAction_PolygonTriggered();
	void onAction_FreehandTriggered();
	void onAction_BezierTriggered();
	void onAction_SelectTriggered();

private:
	Ui::MiniDrawClass ui;
	ViewWidget*	view_widget_;
	QSlider* pen_width_slider_;
	QSpinBox* spinBox_R;
	QSpinBox* spinBox_G;
	QSpinBox* spinBox_B;
};

