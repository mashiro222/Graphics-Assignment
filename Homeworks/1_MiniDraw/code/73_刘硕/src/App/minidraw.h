#ifndef MINIDRAW_H
#define MINIDRAW_H

#include <ui_minidraw.h> // 由Qt的*.ui文件生成
#include <viewwidget.h>

#include <QtWidgets/QMainWindow>
#include <qmessagebox.h>
#include <qcombobox.h>

#include <string>

class MiniDraw : public QMainWindow
{
	Q_OBJECT

public:
	MiniDraw(QWidget *parent = 0);
	~MiniDraw();
	
	// 声明菜单
	QMenu *pMenu; 

	// 声明工具栏
	QToolBar *pToolBar; 

	// 声明尺寸
	QComboBox *pSizeComoBox;

	// 声明一个动作
	QAction *Action_About; 
	QAction *Action_Line;
	QAction *Action_Rect;
	QAction *Action_Ellipse;
	QAction *Action_Polygon;
	QAction *Action_FreeHand;
	QAction *Action_Undo;
	QAction *Action_ChangeColor;

	// QAction *Action_ChangeSize;

	// 声明函数
	void Creat_Menu(); 
	void Creat_ToolBar();
	void Creat_Action();

	void AboutBox();

private:
	Ui::MiniDrawClass ui;
	ViewWidget*	view_widget_; // 实现绘图功能
};

#endif // MINIDRAW_H
