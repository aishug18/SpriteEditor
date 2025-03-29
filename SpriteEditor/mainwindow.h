#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include "animationcontroller.h"
#include "animationpreview.h"
#include "framethumbnail.h"

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
    void penMode();
    void eraseMode();
    void openColorDialog();
    void on_saveButton_clicked();
    void on_loadButton_clicked();
    void addNewFrame();
    void deleteCurrentFrame();
    void duplicateCurrentFrame();
    void onFrameAdded(int frameIndex);
    void onFrameRemoved(int frameIndex);
    void onFrameSelected();
    void togglePlayPause();
    void onFpsSliderChanged(int value);
    void onColorChanged(const QColor &color);

    void on_resizeCanvasButton_clicked();

private:
    Ui::MainWindow *ui;
    AnimationController *animationController;
    AnimationPreview *animPreview;
    QPushButton *playPauseButton;

    // Frame thumbnails
    QVBoxLayout *framesLayout;
    QScrollArea *framesScrollArea;
    QList<FrameThumbnail*> frameThumbnails;

    void initializeAnimationControls();
    void updateFrameThumbnails();
    void updatePlayButtonState();
    void updateFpsLabel();
};
#endif // MAINWINDOW_H
