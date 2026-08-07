#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , __controller()
{
    ui->setupUi(this);
    __controller.initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

