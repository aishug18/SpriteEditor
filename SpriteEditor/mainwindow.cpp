#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QMenu>
#include <QAction>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //connect(ui->eraseToggleButton, &QToolButton::clicked, this, &MainWindow::toggleEraseMode);

    connect(ui->penToolButton, &QToolButton::clicked, this, &MainWindow::penMode);
    connect(ui->eraseToggleButton, &QToolButton::clicked, this, &MainWindow::eraseMode);
    //connect(ui->shapeToolButton, &QToolButton::clicked, this, &MainWindow::shapeMode);
    connect(ui->colorPushButton, &QPushButton::clicked, this, &MainWindow::openColorDialog);

    connect(ui->shapeComboBox, &QComboBox::currentTextChanged, this, [this](const QString &shape) {
        ui->spriteCanvas->setShapeMode(shape); // Pass the selected shape to SpriteCanvas
    });




}

MainWindow::~MainWindow()
{
    delete ui;
}

// void MainWindow::toggleEraseMode()
// {
//     bool isErasing = ui->eraseToggleButton->isChecked();
//     ui->spriteCanvas->setEraserMode(isErasing);
// }

void MainWindow::penMode() {
    ui->spriteCanvas->setEraserMode(false);
}
void MainWindow::eraseMode() {
    ui->spriteCanvas->setEraserMode(true);
}

void MainWindow::rectangleMode() {
    ui->spriteCanvas->setShapeMode("Rectangle");
}

void MainWindow::circleMode() {
    ui->spriteCanvas->setShapeMode("Circle");
}

void MainWindow::lineMode() {
    ui->spriteCanvas->setShapeMode("Line");
}

void MainWindow::openColorDialog() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select Color");
    if (color.isValid()) {
        ui->spriteCanvas->setDrawColor(color);
    }
}

