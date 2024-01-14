#include "ChildWindow.h"
#include "ImageWidget.h"


ChildWindow::ChildWindow(void)
{
	imagewidget_ = new ImageWidget(this);
	setCentralWidget(imagewidget_);
	setAttribute(Qt::WA_DeleteOnClose, true);
}


ChildWindow::~ChildWindow(void)
{
	if (imagewidget_)
	{
		delete imagewidget_;
		imagewidget_ = NULL;
	}
}

bool ChildWindow::LoadFile(QString filename)
{
	imagewidget_->Open(filename);
	return true;
}