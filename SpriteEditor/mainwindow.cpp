#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QDebug>   // for debugging, remove before submitting


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->penToolButton, &QToolButton::clicked, this, &MainWindow::penMode);
    connect(ui->eraseToggleButton, &QToolButton::clicked, this, &MainWindow::eraseMode);
    connect(ui->colorPushButton, &QPushButton::clicked, this, &MainWindow::openColorDialog);
    connect(ui->colorDropperToolButton, &QToolButton::clicked, this, [this]() {ui->spriteCanvas->setColorDropper(true);});

    connect(ui->shapeComboBox, &QComboBox::currentTextChanged, this, [this](const QString &shape) {
            ui->spriteCanvas->setShapeMode(shape);
    });

    // find diff eraser icon
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::penMode() {
    ui->spriteCanvas->setEraserMode(false);
    ui->spriteCanvas->setShapeMode(false);
    ui->spriteCanvas->setColorDropper(false);
}
void MainWindow::eraseMode() {
    ui->spriteCanvas->setEraserMode(true);
    ui->spriteCanvas->setShapeMode(false);
    ui->spriteCanvas->setColorDropper(false);
}

void MainWindow::openColorDialog() {
    QColorDialog colorDialog(this);
    colorDialog.setOption(QColorDialog::ShowAlphaChannel); // Allow alpha selection

    if (colorDialog.exec() == QDialog::Accepted) {
        QColor color = colorDialog.selectedColor();
        if (color.isValid()) {
            ui->spriteCanvas->setDrawColor(color);
        }
    }
}

