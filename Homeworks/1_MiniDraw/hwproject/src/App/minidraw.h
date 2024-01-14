#ifndef MINIDRAW_H
#define MINIDRAW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MiniDraw; }
QT_END_NAMESPACE

class MiniDraw : public QMainWindow
{
    Q_OBJECT

public:
    MiniDraw(QWidget *parent = nullptr);
    ~MiniDraw();

private:
    Ui::MiniDraw *ui;
};
#endif // MINIDRAW_H
