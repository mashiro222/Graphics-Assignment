#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
	class QAction;
	class QMenu;
	class ViewWidget;
	class QImage;
	class QPainter;
	class QRect;
	class ImageWidget;
QT_END_NAMESPACE

namespace Ui {
	class MainWindowClass;
}
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent *e);
	void paintEvent(QPaintEvent *paintevent);

private slots:
	
private:
	void AboutImageWarping();

	void CreateActions();
	void CreateMenus();
	void CreateToolBars();
	void CreateStatusBar();

private:
	Ui::MainWindowClass *ui;

	QMenu		*menu_file_;
	QMenu		*menu_edit_;
	QMenu		*menu_help_;
	QToolBar	*toolbar_file_;
	QToolBar	*toolbar_edit_;
	QAction		*action_new_;
	QAction		*action_open_;
	QAction		*action_save_;
	QAction		*action_saveas_;
	QAction		*action_invert_;
	QAction		*action_mirror_;
	QAction		*action_gray_;
	QAction		*action_restore_;
	QAction		*minidraw_about_action_;
	
	QAction		*action_set_controlpoints_;
	QAction		*action_idw_imagewarping_;
	QAction		*action_rbf_imagewarping_;
	QAction		*action_cp_imagewarping_;

	ImageWidget		*imagewidget_;
};

#endif // MAINWINDOW_H
