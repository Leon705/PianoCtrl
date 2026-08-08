#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEBUGWINDOW

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller_()
{
    ui->setupUi(this);

#ifndef DEBUGWINDOW
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setCursor(Qt::CursorShape::BlankCursor);
    showFullScreen();
#endif

    if (std::expected<void, Controller::SystemError> res = controller_.initialize(); !res) {
        std::cerr << Controller::errorToQString(res.error()).toStdString() << std::endl;
        exit(1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

