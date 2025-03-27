#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "spritecanvas.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void penMode();
    void eraseMode();
    void openColorDialog();

    //--------------------------------
    void on_saveButton_clicked();
    void on_loadButton_clicked();
};
#endif // MAINWINDOW_H
