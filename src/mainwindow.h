#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"

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

private slots:
    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    Controller __controller;
};
#endif // MAINWINDOW_H
