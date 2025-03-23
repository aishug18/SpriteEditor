#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->eraseToggleButton, &QToolButton::clicked, this, &MainWindow::toggleEraseMode);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleEraseMode()
{
    bool isErasing = ui->eraseToggleButton->isChecked();
    ui->spriteCanvas->setEraserMode(isErasing);
}
