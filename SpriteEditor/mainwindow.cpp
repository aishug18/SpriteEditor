/**
 * @file mainwindow.cpp
 * @author Anurup Kumar | Diya Mandot | Kunj Rathod | Aishwarya Golakoti | Morris Lu
 * @course CS 3505
 * @brief Implements the main window UI logic for the Qt Sprite Editor.
 * @checkedby all of us
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QInputDialog>

/**
 * @brief Constructor for MainWindow. Sets up UI and connects all signals/slots.
 * @param parent Optional parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up mutually exclusive tool buttons
    QButtonGroup *toolsGroup = new QButtonGroup(this);
    toolsGroup->addButton(ui->penToolButton);
    toolsGroup->addButton(ui->eraseToggleButton);
    toolsGroup->addButton(ui->colorDropperToolButton);
    toolsGroup->setExclusive(true);

    // Create animation controller for sprite canvas
    animationController = new AnimationController(ui->spriteCanvas, this);

    // Initialize animation preview and controls
    initializeAnimationControls();

    // Frame management setup
    framesLayout = ui->thumbnailsLayout;
    framesScrollArea = ui->framesScrollArea;

    // Connect frame control buttons
    connect(ui->addFrameButton, &QPushButton::clicked, this, &MainWindow::addNewFrame);
    connect(ui->deleteFrameButton, &QPushButton::clicked, this, &MainWindow::deleteCurrentFrame);
    connect(ui->duplicateFrameButton, &QPushButton::clicked, this, &MainWindow::duplicateCurrentFrame);

    // Connect sprite canvas signals to main window
    connect(ui->spriteCanvas, &SpriteCanvas::frameAdded, this, &MainWindow::onFrameAdded);
    connect(ui->spriteCanvas, &SpriteCanvas::frameRemoved, this, &MainWindow::onFrameRemoved);
    connect(ui->spriteCanvas, &SpriteCanvas::currentFrameChanged, this, &MainWindow::updateFrameThumbnails);
    connect(ui->spriteCanvas, &SpriteCanvas::frameContentChanged, this, &MainWindow::updateFrameThumbnails);

    // Tool button behavior
    connect(ui->penToolButton, &QToolButton::clicked, this, &MainWindow::penMode);
    connect(ui->eraseToggleButton, &QToolButton::clicked, this, &MainWindow::eraseMode);
    connect(ui->colorPushButton, &QPushButton::clicked, this, &MainWindow::openColorDialog);

    connect(ui->colorDropperToolButton, &QToolButton::clicked, this, [this]() {
        ui->spriteCanvas->setColorDropper(true);
    });

    // Shape selection combo box
    connect(ui->shapeComboBox, &QComboBox::currentTextChanged, this, [this](const QString &shape) {
        ui->spriteCanvas->setShapeMode(shape);
        ui->penToolButton->setChecked(false);
        ui->eraseToggleButton->setChecked(false);
        ui->colorDropperToolButton->setChecked(false);
    });

    // Color synchronization
    connect(ui->spriteCanvas, &SpriteCanvas::colorChanged, this, &MainWindow::onColorChanged);

    // FPS slider connection
    connect(ui->fpsSlider, &QSlider::valueChanged, this, &MainWindow::onFpsSliderChanged);
    updateFpsLabel();

    // Add the initial frame thumbnail
    onFrameAdded(0);
}

/**
 * @brief Destructor for MainWindow.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Initializes the animation preview pane and its controls.
 */
void MainWindow::initializeAnimationControls()
{
    QVBoxLayout *previewLayout = new QVBoxLayout(ui->previewFrame);
    previewLayout->setContentsMargins(10, 10, 10, 10);
    previewLayout->setSpacing(10);

    // Animation preview widget
    animPreview = new AnimationPreview(ui->spriteCanvas, ui->previewFrame);
    previewLayout->addWidget(animPreview);

    // Controls layout
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(5);

    // Previous frame button
    QPushButton *prevButton = new QPushButton("◀", ui->previewFrame);
    prevButton->setFixedSize(30, 30);
    prevButton->setStyleSheet("background-color: #FFD5D5; border-radius: 15px;");
    connect(prevButton, &QPushButton::clicked, animationController, &AnimationController::previousFrame);
    controlsLayout->addWidget(prevButton);

    // Play/Pause button
    playPauseButton = new QPushButton("▶", ui->previewFrame);
    playPauseButton->setFixedSize(40, 40);
    playPauseButton->setStyleSheet("background-color: #FFD5D5; border-radius: 20px;");
    connect(playPauseButton, &QPushButton::clicked, this, &MainWindow::togglePlayPause);
    controlsLayout->addWidget(playPauseButton);

    // Next frame button
    QPushButton *nextButton = new QPushButton("▶", ui->previewFrame);
    nextButton->setFixedSize(30, 30);
    nextButton->setStyleSheet("background-color: #FFD5D5; border-radius: 15px;");
    connect(nextButton, &QPushButton::clicked, animationController, &AnimationController::nextFrame);
    controlsLayout->addWidget(nextButton);

    previewLayout->addLayout(controlsLayout);

    // Link sprite canvas to preview
    connect(ui->spriteCanvas, &SpriteCanvas::currentFrameChanged, animPreview, &AnimationPreview::updateCurrentFrame);
    connect(ui->spriteCanvas, &SpriteCanvas::frameContentChanged, animPreview, &AnimationPreview::refreshFrames);
    connect(ui->spriteCanvas, &SpriteCanvas::frameAdded, animPreview, &AnimationPreview::refreshFrames);
}

/**
 * @brief Enables pen tool and disables others.
 */
void MainWindow::penMode()
{
    ui->spriteCanvas->setEraserMode(false);
    ui->spriteCanvas->setShapeMode(false);
    ui->spriteCanvas->setColorDropper(false);
}

/**
 * @brief Enables eraser tool and disables others.
 */
void MainWindow::eraseMode()
{
    ui->spriteCanvas->setEraserMode(true);
    ui->spriteCanvas->setShapeMode(false);
    ui->spriteCanvas->setColorDropper(false);
}

/**
 * @brief Opens a QColorDialog to choose drawing color.
 */
void MainWindow::openColorDialog()
{
    QColorDialog colorDialog(this);
    colorDialog.setOption(QColorDialog::ShowAlphaChannel);

    if (colorDialog.exec() == QDialog::Accepted) {
        QColor color = colorDialog.selectedColor();
        if (color.isValid()) {
            ui->spriteCanvas->setDrawColor(color);
        }
    }
}

/**
 * @brief Saves the sprite sheet to a .ssp file.
 */
void MainWindow::on_saveButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save Sprite", "", "Sprite Sheet Project (*.ssp)");
    if (!filePath.isEmpty()) {
        if (!ui->spriteCanvas->saveToJson(filePath)) {
            QMessageBox::warning(this, "Save Error", "Failed to save the sprite.");
        }
    }
}

/**
 * @brief Loads a sprite sheet from a .ssp file.
 */
void MainWindow::on_loadButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Sprite", "", "Sprite Sheet Project (*.ssp)");
    if (!filePath.isEmpty()) {
        if (!ui->spriteCanvas->loadFromJson(filePath)) {
            QMessageBox::warning(this, "Load Error", "Failed to load the sprite.");
        } else {
            updateFrameThumbnails();
        }
    }
}

/**
 * @brief Adds a new frame to the animation.
 */
void MainWindow::addNewFrame()
{
    ui->spriteCanvas->addNewFrame();
}

/**
 * @brief Deletes the current frame.
 */
void MainWindow::deleteCurrentFrame()
{
    ui->spriteCanvas->deleteCurrentFrame();
}

/**
 * @brief Duplicates the current frame.
 */
void MainWindow::duplicateCurrentFrame()
{
    ui->spriteCanvas->duplicateCurrentFrame();
}

/**
 * @brief Adds a thumbnail widget when a frame is added.
 * @param frameIndex Index of the new frame.
 */
void MainWindow::onFrameAdded(int frameIndex)
{
    FrameThumbnail *thumbnail = new FrameThumbnail();
    thumbnail->setImage(ui->spriteCanvas->getFrameImage(frameIndex));
    thumbnail->setProperty("frameIndex", frameIndex);

    connect(thumbnail, &FrameThumbnail::clicked, this, &MainWindow::onFrameSelected);

    frameThumbnails.append(thumbnail);
    framesLayout->addWidget(thumbnail);

    updateFrameThumbnails();
}

/**
 * @brief Removes the thumbnail for the deleted frame.
 * @param frameIndex Index of the removed frame.
 */
void MainWindow::onFrameRemoved(int frameIndex)
{
    for (int i = 0; i < frameThumbnails.size(); ++i) {
        if (frameThumbnails[i]->property("frameIndex").toInt() == frameIndex) {
            FrameThumbnail *thumbnail = frameThumbnails.takeAt(i);
            framesLayout->removeWidget(thumbnail);
            delete thumbnail;
            break;
        }
    }

    for (int i = 0; i < frameThumbnails.size(); ++i) {
        int oldIndex = frameThumbnails[i]->property("frameIndex").toInt();
        if (oldIndex > frameIndex) {
            frameThumbnails[i]->setProperty("frameIndex", oldIndex - 1);
        }
    }

    updateFrameThumbnails();
}

/**
 * @brief Selects a frame when its thumbnail is clicked.
 */
void MainWindow::onFrameSelected()
{
    FrameThumbnail *thumbnail = qobject_cast<FrameThumbnail *>(sender());
    if (thumbnail) {
        int frameIndex = thumbnail->property("frameIndex").toInt();
        ui->spriteCanvas->setCurrentFrame(frameIndex);
    }
}

/**
 * @brief Updates all thumbnails to reflect current frame and image.
 */
void MainWindow::updateFrameThumbnails()
{
    int currentFrameIndex = ui->spriteCanvas->getCurrentFrameIndex();

    for (FrameThumbnail *thumbnail : frameThumbnails) {
        int frameIndex = thumbnail->property("frameIndex").toInt();
        thumbnail->setImage(ui->spriteCanvas->getFrameImage(frameIndex));
        thumbnail->setSelected(frameIndex == currentFrameIndex);
    }
}

/**
 * @brief Toggles the animation between play and pause.
 */
void MainWindow::togglePlayPause()
{
    animationController->togglePlayPause();
    updatePlayButtonState();
}

/**
 * @brief Updates the play/pause button icon based on current state.
 */
void MainWindow::updatePlayButtonState()
{
    if (animationController->isPlaying()) {
        playPauseButton->setText("⏸");
        playPauseButton->setToolTip("Pause Animation");
    } else {
        playPauseButton->setText("▶");
        playPauseButton->setToolTip("Play Animation");
    }
}

/**
 * @brief Updates FPS and label when slider is changed.
 * @param value New FPS value.
 */
void MainWindow::onFpsSliderChanged(int value)
{
    animationController->setFps(value);
    updateFpsLabel();
}

/**
 * @brief Updates the FPS label to match current controller FPS.
 */
void MainWindow::updateFpsLabel()
{
    int fps = animationController->getFps();
    ui->fpsValueLabel->setText(QString::number(fps));
}

/**
 * @brief Updates the color picker button’s appearance.
 * @param color Selected drawing color.
 */
void MainWindow::onColorChanged(const QColor &color)
{
    QString styleSheet = QString("QPushButton { background-color: %1; border-radius: 12px; }")
    .arg(color.name());
    ui->colorPushButton->setStyleSheet(styleSheet);
}

/**
 * @brief Opens a dialog to resize the sprite canvas.
 */
void MainWindow::on_resizeCanvasButton_clicked()
{
    const int maxSize = 32;

    bool ok1, ok2;
    int width = QInputDialog::getInt(this, "Canvas Width", "Enter new width (max 32):", 16, 1, maxSize, 1, &ok1);
    int height = QInputDialog::getInt(this, "Canvas Height", "Enter new height (max 32):", 16, 1, maxSize, 1, &ok2);

    if (ok1 && ok2) {
        ui->spriteCanvas->resizeCanvas(width, height);
    }
}
