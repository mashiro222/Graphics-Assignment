#ifndef MINIDRAW_H
#define MINIDRAW_H

#include <ui_minidraw.h> // ��Qt��*.ui�ļ�����
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
	
	// �����˵�
	QMenu *pMenu; 

	// ����������
	QToolBar *pToolBar; 

	// �����ߴ�
	QComboBox *pSizeComoBox;

	// ����һ������
	QAction *Action_About; 
	QAction *Action_Line;
	QAction *Action_Rect;
	QAction *Action_Ellipse;
	QAction *Action_Polygon;
	QAction *Action_FreeHand;
	QAction *Action_Undo;
	QAction *Action_ChangeColor;

	// QAction *Action_ChangeSize;

	// ��������
	void Creat_Menu(); 
	void Creat_ToolBar();
	void Creat_Action();

	void AboutBox();

private:
	Ui::MiniDrawClass ui;
	ViewWidget*	view_widget_; // ʵ�ֻ�ͼ����
};

#endif // MINIDRAW_H
