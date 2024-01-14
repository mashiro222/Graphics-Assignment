#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ViewWidget; }
QT_END_NAMESPACE

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    ViewWidget(QWidget *parent = nullptr);
    ~ViewWidget();

private:
    Ui::ViewWidget *ui;
};
#endif // VIEWWIDGET_H
