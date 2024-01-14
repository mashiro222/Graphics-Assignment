#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QSlider>
#include <QLineEdit>
#include "ui_MiniDraw.h"
#include "ViewWidget.h"

class MiniDraw : public QMainWindow
{
	Q_OBJECT

public:
	MiniDraw(QWidget *parent = Q_NULLPTR);

private:
	Ui::MiniDrawClass ui;

	QMenu* main_menu_;

	QToolBar* main_toolbar_;
		QAction* about_action_;
		QAction* undo_action_;
		QAction* delete_action_;
		QAction* save_action_;
		QAction* chose_action_;
		QAction* color_action_;
			QSlider* size_slider;
			QLineEdit* lineEdit;
			QLineEdit* colorEdit;

	QToolBar* draw_toolbar_;
		QAction* line_action_;
		QAction* rectangle_action_;
		QAction* ellipse_action_;
		QAction* polygon_action_;
		QAction* freehand_action_;

	ViewWidget*	view_widget_;	

	void Init();

private slots:
	void setLineEditValue(int value);
	void setColorEditValue(QColor c);
	void About();
	void Save();
};
