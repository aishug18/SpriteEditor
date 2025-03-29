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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up tool buttons in an exclusive group
    QButtonGroup* toolsGroup = new QButtonGroup(this);
    toolsGroup->addButton(ui->penToolButton);
    toolsGroup->addButton(ui->eraseToggleButton);
    toolsGroup->addButton(ui->colorDropperToolButton);
    toolsGroup->setExclusive(true);

    // Create animation controller
    animationController = new AnimationController(ui->spriteCanvas, this);

    // Initialize Animation Controls
    initializeAnimationControls();

    // Initialize Frame Management
    framesLayout = ui->thumbnailsLayout;
    framesScrollArea = ui->framesScrollArea;

    // Connect frame management buttons
    connect(ui->addFrameButton, &QPushButton::clicked, this, &MainWindow::addNewFrame);
    connect(ui->deleteFrameButton, &QPushButton::clicked, this, &MainWindow::deleteCurrentFrame);
    connect(ui->duplicateFrameButton, &QPushButton::clicked, this, &MainWindow::duplicateCurrentFrame);

    // Connect sprite canvas frame signals
    connect(ui->spriteCanvas, &SpriteCanvas::frameAdded, this, &MainWindow::onFrameAdded);
    connect(ui->spriteCanvas, &SpriteCanvas::frameRemoved, this, &MainWindow::onFrameRemoved);
    connect(ui->spriteCanvas, &SpriteCanvas::currentFrameChanged, this, &MainWindow::updateFrameThumbnails);
    connect(ui->spriteCanvas, &SpriteCanvas::frameContentChanged, this, &MainWindow::updateFrameThumbnails);

    // Connect drawing tools
    connect(ui->penToolButton, &QToolButton::clicked, this, &MainWindow::penMode);
    connect(ui->eraseToggleButton, &QToolButton::clicked, this, &MainWindow::eraseMode);
    connect(ui->colorPushButton, &QPushButton::clicked, this, &MainWindow::openColorDialog);
    connect(ui->colorDropperToolButton, &QToolButton::clicked, this, [this]() {
        ui->spriteCanvas->setColorDropper(true);
    });

    connect(ui->shapeComboBox, &QComboBox::currentTextChanged, this, [this](const QString &shape) {
        ui->spriteCanvas->setShapeMode(shape);
        ui->penToolButton->setChecked(false);
        ui->eraseToggleButton->setChecked(false);
        ui->colorDropperToolButton->setChecked(false);
    });

    // Connect color changed signal
    connect(ui->spriteCanvas, &SpriteCanvas::colorChanged, this, &MainWindow::onColorChanged);

    // Connect FPS slider
    connect(ui->fpsSlider, &QSlider::valueChanged, this, &MainWindow::onFpsSliderChanged);
    updateFpsLabel();

    // Add the first frame thumbnail
    onFrameAdded(0);
}

void MainWindow::initializeAnimationControls()
{
    // Create a layout for the animation controls in the preview frame
    QVBoxLayout* previewLayout = new QVBoxLayout(ui->previewFrame);
    previewLayout->setContentsMargins(10, 10, 10, 10);
    previewLayout->setSpacing(10);

    // Create animation preview widget
    animPreview = new AnimationPreview(ui->spriteCanvas, ui->previewFrame);
    previewLayout->addWidget(animPreview);

    // Create animation control buttons
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(5);

    // Previous frame button
    QPushButton* prevButton = new QPushButton("◀", ui->previewFrame);
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
    QPushButton* nextButton = new QPushButton("▶", ui->previewFrame);
    nextButton->setFixedSize(30, 30);
    nextButton->setStyleSheet("background-color: #FFD5D5; border-radius: 15px;");
    connect(nextButton, &QPushButton::clicked, animationController, &AnimationController::nextFrame);
    controlsLayout->addWidget(nextButton);

    // Add controls to preview layout
    previewLayout->addLayout(controlsLayout);

    // Connect sprite canvas frame signals to preview
    connect(ui->spriteCanvas, &SpriteCanvas::currentFrameChanged, animPreview, &AnimationPreview::updateCurrentFrame);
    connect(ui->spriteCanvas, &SpriteCanvas::frameContentChanged, animPreview, &AnimationPreview::refreshFrames);
    connect(ui->spriteCanvas, &SpriteCanvas::frameAdded, animPreview, &AnimationPreview::refreshFrames);
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

void MainWindow::on_saveButton_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save Sprite", "", "Sprite Sheet Project (*.ssp)");
    if (!filePath.isEmpty()) {
        if (!ui->spriteCanvas->saveToJson(filePath)) {
            QMessageBox::warning(this, "Save Error", "Failed to save the sprite.");
        }
    }
}

void MainWindow::on_loadButton_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Sprite", "", "Sprite Sheet Project (*.ssp)");
    if (!filePath.isEmpty()) {
        if (!ui->spriteCanvas->loadFromJson(filePath)) {
            QMessageBox::warning(this, "Load Error", "Failed to load the sprite.");
        } else {
            // Update thumbnails after loading
            updateFrameThumbnails();
        }
    }
}

void MainWindow::addNewFrame() {
    ui->spriteCanvas->addNewFrame();
}

void MainWindow::deleteCurrentFrame() {
    ui->spriteCanvas->deleteCurrentFrame();
}

void MainWindow::duplicateCurrentFrame() {
    ui->spriteCanvas->duplicateCurrentFrame();
}

void MainWindow::onFrameAdded(int frameIndex) {
    // Create a thumbnail for the new frame
    FrameThumbnail* thumbnail = new FrameThumbnail();
    thumbnail->setImage(ui->spriteCanvas->getFrameImage(frameIndex));

    // Set frame index as property
    thumbnail->setProperty("frameIndex", frameIndex);

    // Connect thumbnail click
    connect(thumbnail, &FrameThumbnail::clicked, this, &MainWindow::onFrameSelected);

    // Add to list and layout before the stretch
    frameThumbnails.append(thumbnail);
    framesLayout->addWidget(thumbnail);

    // Update selection state
    updateFrameThumbnails();
}

void MainWindow::onFrameRemoved(int frameIndex) {
    // Find and remove the thumbnail for this frame
    for (int i = 0; i < frameThumbnails.size(); i++) {
        if (frameThumbnails[i]->property("frameIndex").toInt() == frameIndex) {
            // Remove from layout and delete
            FrameThumbnail* thumbnail = frameThumbnails.takeAt(i);
            framesLayout->removeWidget(thumbnail);
            delete thumbnail;
            break;
        }
    }

    // Update the frame indices for remaining thumbnails
    for (int i = 0; i < frameThumbnails.size(); i++) {
        int oldIndex = frameThumbnails[i]->property("frameIndex").toInt();
        if (oldIndex > frameIndex) {
            frameThumbnails[i]->setProperty("frameIndex", oldIndex - 1);
        }
    }

    // Update selection state
    updateFrameThumbnails();
}

void MainWindow::onFrameSelected() {
    // Get the clicked thumbnail
    FrameThumbnail* thumbnail = qobject_cast<FrameThumbnail*>(sender());
    if (thumbnail) {
        int frameIndex = thumbnail->property("frameIndex").toInt();
        ui->spriteCanvas->setCurrentFrame(frameIndex);
    }
}

void MainWindow::updateFrameThumbnails() {
    int currentFrameIndex = ui->spriteCanvas->getCurrentFrameIndex();

    // Update all thumbnails
    for (FrameThumbnail* thumbnail : frameThumbnails) {
        int frameIndex = thumbnail->property("frameIndex").toInt();

        // Update image
        thumbnail->setImage(ui->spriteCanvas->getFrameImage(frameIndex));

        // Update selection state
        thumbnail->setSelected(frameIndex == currentFrameIndex);
    }
}

void MainWindow::togglePlayPause() {
    animationController->togglePlayPause();
    updatePlayButtonState();
}

void MainWindow::updatePlayButtonState() {
    if (animationController->isPlaying()) {
        playPauseButton->setText("⏸");
        playPauseButton->setToolTip("Pause Animation");
    } else {
        playPauseButton->setText("▶");
        playPauseButton->setToolTip("Play Animation");
    }
}

void MainWindow::onFpsSliderChanged(int value)
{
    animationController->setFps(value);
    updateFpsLabel();
}

void MainWindow::updateFpsLabel()
{
    int fps = animationController->getFps();
    ui->fpsValueLabel->setText(QString::number(fps));
}

void MainWindow::onColorChanged(const QColor &color) {
    // Update color button background
    QString styleSheet = QString("QPushButton { background-color: %1; border-radius: 12px; }").arg(color.name());
    ui->colorPushButton->setStyleSheet(styleSheet);
}

void MainWindow::on_resizeCanvasButton_clicked()
{
    const int maxSize = 32;  // You can increase this if needed

    bool ok1, ok2;
    int width = QInputDialog::getInt(this, "Canvas Width", "Enter new width (max 32):", 16, 1, maxSize, 1, &ok1);
    int height = QInputDialog::getInt(this, "Canvas Height", "Enter new height (max 32):", 16, 1, maxSize, 1, &ok2);

    if (ok1 && ok2) {
        ui->spriteCanvas->resizeCanvas(width, height);
    }
}

