#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "Clipboard.h"
#include "ImageWidget.h"

class ChildWindow;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QCheckBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	// File IO
//	void New();
	void Open();								// Open an existing file
	void Save();								// Save image to file
	void SaveAs();
	ChildWindow *CreateChildWindow();
	void SetActiveSubWindow(QWidget* window);

	// Image Processing
	void Invert();								// Invert each pixel's rgb value
	void Mirror();								// Mirror image vertically or horizontally
	void GrayScale();							// Turn image to gray-scale map
	void Restore();								// Restore image to origin

	//Select
	void ChoosePolygon();							// Choose polygon region
	void ChooseFreehand();							// Choose freehand region
	//Copy
	void Copy();									// Copy choosed region to clipboard

	//Paste
	void PasteClone();								// Paste rect region to object image
	void PasteSeamless();							// Poisson image editing


private:
	void CreateActions();
	void CreateMenus();
	void CreateToolBars();
	void CreateStatusBar();

	QMdiSubWindow *FindChild(const QString &filename);
	ChildWindow* GetChildWindow();
	
private:
	Ui::MainWindowClass ui;

	QMenu						*menu_file_;
	QMenu						*menu_edit_;
	QMenu						*menu_help_;
	QToolBar					*toolbar_file_;
	QToolBar					*toolbar_edit_;
//	QAction						*action_new_;
	QAction						*action_open_;
	QAction						*action_save_;
	QAction						*action_saveas_;

	QAction						*action_invert_;
	QAction						*action_mirror_;
	QAction						*action_gray_;
	QAction						*action_restore_;

	QAction						*action_choose_polygon_;
	QAction						*action_choose_freehand;
	QAction						*action_copy_;
	QAction						*action_paste_clone;
	QAction						*action_paste_seamless;
	QCheckBox					*checkBox_mixed;

	QMdiArea					*mdi_area_;
	//QSignalMapper				*window_mapper_;

	ChildWindow					*child_source_;

public:
	CClipboard					*shared_clipboard;		//剪贴板，每个子窗体共享一个剪贴板

};

#endif // MAINWINDOW_H
