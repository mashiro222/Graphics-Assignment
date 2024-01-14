#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QAction* hello_world_action_; // 声明动作
    QMenu* main_menu_; // 声明菜单
    QToolBar* main_toolbar_; // 声明工具栏
    void CreateButtons(); // 声明函数

 private slots: void HelloWorld();
};
#endif // MAINWINDOW_H
