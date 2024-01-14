#include "minidraw.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	QApplication a(argc, argv);
	MiniDraw w; // 创建主窗口
	w.show(); // 显示主窗口
	return a.exec(); // 将应用交给Qt托管，进入事件循环
}
