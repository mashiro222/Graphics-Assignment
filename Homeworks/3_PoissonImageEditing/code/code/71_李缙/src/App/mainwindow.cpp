#include <QtWidgets>
#include "mainwindow.h"
#include "ChildWindow.h"
#include "ImageWidget.h"
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    mdi_area_ = new QMdiArea;
    mdi_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdi_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdi_area_);

    window_mapper_ = new QSignalMapper(this);
    connect(window_mapper_, SIGNAL(mapped(QWidget*)), this, SLOT(SetActiveSubWindow(QWidget*)));

    child_source_ = NULL;

    CreateActions();
    CreateMenus();
    CreateToolBars();
    CreateStatusBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::CreateActions()
{
    // File IO
    action_open_ = new QAction(QIcon("../resources/icon/open.png"), tr("&Open..."), this);
    action_open_->setShortcuts(QKeySequence::Open);
    action_open_->setStatusTip(tr("Open an existing file"));
    connect(action_open_, SIGNAL(triggered()), this, SLOT(Open()));

    action_save_ = new QAction(QIcon("../resources/icon/save.png"), tr("&Save"), this);
    action_save_->setShortcuts(QKeySequence::Save);
    action_save_->setStatusTip(tr("Save the document to disk"));
    connect(action_save_, SIGNAL(triggered()), this, SLOT(Save()));

    action_saveas_ = new QAction(QIcon("../resources/icon/saveas.png"),tr("Save &As..."), this);
    action_saveas_->setShortcuts(QKeySequence::SaveAs);
    action_saveas_->setStatusTip(tr("Save the document under a new name"));
    connect(action_saveas_, SIGNAL(triggered()), this, SLOT(SaveAs()));

    // Image processing
    action_invert_ = new QAction(QIcon("../resources/icon/inverse.png"),tr("Inverse"), this);
    action_invert_->setStatusTip(tr("Invert all pixel value in the image"));
    connect(action_invert_, SIGNAL(triggered()), this, SLOT(Invert()));

    action_mirror_ = new QAction(QIcon("../resources/icon/mirror.png"),tr("Mirror"), this);
    action_mirror_->setStatusTip(tr("Mirror image vertically or horizontally"));
    connect(action_mirror_, SIGNAL(triggered()), this, SLOT(Mirror()));

    action_gray_ = new QAction(QIcon("../resources/icon/gray.png"),tr("Grayscale"), this);
    action_gray_->setStatusTip(tr("Gray-scale map"));
    connect(action_gray_, SIGNAL(triggered()), this, SLOT(GrayScale()));

    action_restore_ = new QAction(QIcon("../resources/icon/redo.png"),tr("Restore"), this);
    action_restore_->setStatusTip(tr("Show origin image"));
    connect(action_restore_, SIGNAL(triggered()), this, SLOT(Restore()));


    action_rect_ = new QAction(QIcon("../resources/icon/rect.png"),tr("Rect"), this);
    connect(action_rect_, SIGNAL(triggered()), this, SLOT(ChooseRect()));

    action_elli_ = new QAction(QIcon("../resources/icon/elli.png"),tr("Elli"), this);
    connect(action_elli_, SIGNAL(triggered()), this, SLOT(ChooseElli()));

    action_poly_ = new QAction(QIcon("../resources/icon/poly.png"),tr("Poly"), this);
    connect(action_poly_, SIGNAL(triggered()), this, SLOT(ChoosePoly()));

    action_free_ = new QAction(QIcon("../resources/icon/hand.png"),tr("Free"), this);
    connect(action_free_, SIGNAL(triggered()), this, SLOT(ChooseFree()));


    action_copy_ = new QAction(QIcon("../resources/icon/copy.png"),tr("Copy"),this);
    connect(action_copy_, SIGNAL(triggered()), this, SLOT(ChooseCopy()));

    action_poi_ = new QAction(tr("Poisson"),this);
    connect(action_poi_, SIGNAL(triggered()), this, SLOT(ChoosePoi()));

}

void MainWindow::CreateMenus()
{
}

void MainWindow::CreateToolBars()
{
    toolbar_file_ = addToolBar(tr("File"));
    toolbar_file_->addAction(action_open_);
    toolbar_file_->addAction(action_save_);

    toolbar_file_->addSeparator();

    toolbar_file_->addAction(action_restore_);

    toolbar_paint_=addToolBar(tr("Paint"));
    toolbar_paint_->addAction(action_rect_);
    toolbar_paint_->addAction(action_elli_);
    toolbar_paint_->addAction(action_poly_);
    toolbar_paint_->addAction(action_free_);

    toolbar_paste_=addToolBar(tr("Paste"));
    toolbar_paste_->addAction(action_copy_);
    toolbar_paste_->addAction(action_poi_);

}

void MainWindow::CreateStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::Open()
{
    QString filename = QFileDialog::getOpenFileName(this);
    if(!filename.isEmpty())
    {
        QMdiSubWindow *existing = FindChild(filename);

        if (existing)
        {
            mdi_area_->setActiveSubWindow(existing);
            return;
        }

        ChildWindow *child = CreateChildWindow();
        if (child->LoadFile(filename))
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();

            // Change size of the child window so it can fit image size
            int x = child->geometry().x();
            int y = child->geometry().y();
            int width = child->imagewidget_->ImageWidth();
            int height = child->imagewidget_->ImageHeight();
            mdi_area_->activeSubWindow()->setFixedSize(width+2*x, height+x+y);
        }
        else
        {
            child->close();
        }
    }
}

void MainWindow::Save()
{
    SaveAs();
}

ChildWindow* MainWindow::GetChildWindow() {
    QMdiSubWindow* activeSubWindow = mdi_area_->activeSubWindow();
    if (!activeSubWindow)
        return nullptr;

    return qobject_cast<ChildWindow*>(activeSubWindow->widget());
}

void MainWindow::SaveAs()
{
    if (!mdi_area_->activeSubWindow())
        return;
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area_->activeSubWindow()->widget());
    window->imagewidget_->SaveAs();
}

ChildWindow *MainWindow::CreateChildWindow()
{
    ChildWindow *child = new ChildWindow;
    mdi_area_->addSubWindow(child);

    return child;
}

void MainWindow::SetActiveSubWindow(QWidget* window)
{
    if (!window)
    {
        return;
    }

    mdi_area_->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::Invert()
{
    if (!mdi_area_->activeSubWindow())
        return;
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area_->activeSubWindow()->widget());
    window->imagewidget_->Invert();
}

void MainWindow::Mirror()
{
    if (!mdi_area_->activeSubWindow())
        return;
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area_->activeSubWindow()->widget());
    window->imagewidget_->Mirror();
}

void MainWindow::GrayScale()
{
    if (!mdi_area_->activeSubWindow())
        return;
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area_->activeSubWindow()->widget());
    window->imagewidget_->TurnGray();
}

void MainWindow::Restore()
{
    if (!mdi_area_->activeSubWindow())
        return;
    ChildWindow *window = qobject_cast<ChildWindow *>(mdi_area_->activeSubWindow()->widget());
    window->imagewidget_->Restore();
}


void MainWindow::ChooseRect()
{
    ChildWindow* window = GetChildWindow();
    if (!window)
        return;
    window->imagewidget_->set_draw_status_to_choose();
    window->imagewidget_->set_region_shape(0);
    child_source_ = window;
}

void MainWindow::ChooseElli()
{
    ChildWindow* window = GetChildWindow();
    if (!window)
        return;
    window->imagewidget_->set_draw_status_to_choose();
    window->imagewidget_->set_region_shape(1);
    child_source_ = window;
}

void MainWindow::ChoosePoly()
{
    ChildWindow* window = GetChildWindow();
    if (!window)
        return;
    window->imagewidget_->set_draw_status_to_choose();
    window->imagewidget_->set_region_shape(2);
    child_source_ = window;
}

void MainWindow::ChooseFree()
{
    ChildWindow* window = GetChildWindow();
    if (!window)
        return;
    window->imagewidget_->set_draw_status_to_choose();
    window->imagewidget_->set_region_shape(3);
    child_source_ = window;
}



void MainWindow::ChooseCopy()
{
    ChildWindow* window = GetChildWindow();
    if (!window)
        return;
    window->imagewidget_->set_draw_status_to_paste();
    window->imagewidget_->set_source_window(child_source_);
    window->imagewidget_->set_paste_method(0);
}

void MainWindow::ChoosePoi()
{
    ChildWindow* window = GetChildWindow();
    if (!window)
        return;
    window->imagewidget_->set_draw_status_to_paste();
    window->imagewidget_->set_source_window(child_source_);
    window->imagewidget_->set_paste_method(1);
    window->imagewidget_->matrix_predecomp();
}


QMdiSubWindow *MainWindow::FindChild(const QString &filename)
{
    QString canonical_filepath = QFileInfo(filename).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdi_area_->subWindowList())
    {
        ChildWindow *child = qobject_cast<ChildWindow *>(window->widget());
        if (child->current_file() == canonical_filepath)
        {
            return window;
        }
    }

    return 0;
}

void MainWindow::TestInsidePoints()
{
    if (!mdi_area_->activeSubWindow())
        return;
    ChildWindow *child = qobject_cast<ChildWindow *>(mdi_area_->activeSubWindow()->widget());
    child->imagewidget_->test_inside_points();
}
