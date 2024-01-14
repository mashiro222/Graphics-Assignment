#include "minidraw.h"
#include "./ui_minidraw.h"

MiniDraw::MiniDraw(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MiniDraw)
{
    ui->setupUi(this);
}

MiniDraw::~MiniDraw()
{
    delete ui;
}

