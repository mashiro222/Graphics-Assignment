#include "mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
    w.setWindowTitle("Poisson Image Edit");
    w.setWindowIcon(QIcon("../resources/icon/title.png"));
	w.show();
	return a.exec();
}
