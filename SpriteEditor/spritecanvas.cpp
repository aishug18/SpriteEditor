/**
 * @file spritecanvas.cpp
 * @author Anurup Kumar | Diya Mandot | Kunj Rathod | Aishwarya Golakoti | Morris Lu
 * @course CS 3505
 * @brief Implements the drawing logic and frame management for SpriteCanvas.
 * @checkedby Reviewer Name
 */

#include "spritecanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

/**
 * @brief Constructs the SpriteCanvas and initializes the first frame.
 * @param parent Optional parent widget.
 */
SpriteCanvas::SpriteCanvas(QWidget *parent)
    : QWidget(parent),
    drawColor(Qt::black),
    erasing(false),
    shapeMode(false),
    colorDropper(false),
    currentFrameIndex(0)
{
    addNewFrame();
}

QSize SpriteCanvas::sizeHint() const {
    return QSize(gridWidth * pixelSize, gridHeight * pixelSize);
}

/**
 * @brief Sets the drawing color and emits colorChanged.
 * @param color New drawing color.
 */
void SpriteCanvas::setDrawColor(const QColor &color) {
    drawColor = color;
    emit colorChanged(color);
}

void SpriteCanvas::setEraserMode(bool enabled) {
    erasing = enabled;
}

void SpriteCanvas::setShapeMode(QString shape) {
    currentShape = shape;
    shapeMode = true;
    colorDropper = false;
}

void SpriteCanvas::setShapeMode(bool mode) {
    shapeMode = mode;
}

void SpriteCanvas::setColorDropper(bool mode) {
    colorDropper = mode;
}

/**
 * @brief Adds a new blank frame and sets it as current.
 */
void SpriteCanvas::addNewFrame() {
    frames.append(SpriteFrame(gridWidth, gridHeight));
    currentFrameIndex = frames.size() - 1;
    emit frameAdded(currentFrameIndex);
    emit currentFrameChanged(currentFrameIndex);
    update();
}

/**
 * @brief Deletes the current frame if more than one exists.
 */
void SpriteCanvas::deleteCurrentFrame() {
    if (frames.size() <= 1) return;

    int frameToRemove = currentFrameIndex;
    int newCurrentFrame = (currentFrameIndex > 0) ? currentFrameIndex - 1 : 0;

    frames.removeAt(frameToRemove);
    currentFrameIndex = newCurrentFrame;

    emit frameRemoved(frameToRemove);
    emit currentFrameChanged(currentFrameIndex);
    update();
}

void SpriteCanvas::setCurrentFrame(int frameIndex) {
    if (frameIndex >= 0 && frameIndex < frames.size() && frameIndex != currentFrameIndex) {
        currentFrameIndex = frameIndex;
        emit currentFrameChanged(currentFrameIndex);
        update();
    }
}

int SpriteCanvas::getCurrentFrameIndex() const {
    return currentFrameIndex;
}

int SpriteCanvas::getFrameCount() const {
    return frames.size();
}

QImage SpriteCanvas::getFrameImage(int frameIndex) const {
    if (frameIndex >= 0 && frameIndex < frames.size()) {
        return frames[frameIndex].image();
    }
    return QImage();
}

/**
 * @brief Duplicates the current frame.
 */
void SpriteCanvas::duplicateCurrentFrame() {
    if (currentFrameIndex >= 0 && currentFrameIndex < frames.size()) {
        QImage currentImg = frames[currentFrameIndex].image();
        frames.insert(currentFrameIndex + 1, SpriteFrame(currentImg));
        currentFrameIndex++;
        emit frameAdded(currentFrameIndex);
        emit currentFrameChanged(currentFrameIndex);
        update();
    }
}

QImage SpriteCanvas::getCurrentImage() const {
    if (frames.isEmpty()) return QImage();
    return frames[currentFrameIndex].image();
}

void SpriteCanvas::setCurrentImage(const QImage &image) {
    if (!frames.isEmpty()) {
        frames[currentFrameIndex].setImage(image);
        update();
        emit frameContentChanged(currentFrameIndex);
    }
}

void SpriteCanvas::paintEvent(QPaintEvent *) {
    if (frames.isEmpty()) return;

    QPainter painter(this);
    QImage currentImage = getCurrentImage();

    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = currentImage.pixelColor(x, y);
            painter.fillRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize, color);
            painter.setPen(Qt::gray);
            painter.drawRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
        }
    }
}

void SpriteCanvas::mousePressEvent(QMouseEvent *event) {
    if (colorDropper) {
        int gridX = event->pos().x() / pixelSize;
        int gridY = event->pos().y() / pixelSize;
        if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
            QImage currentImage = getCurrentImage();
            setDrawColor(currentImage.pixelColor(gridX, gridY));
        }
    } else if (!shapeMode) {
        drawAt(event->pos().x(), event->pos().y());
    }
    startPoint = event->pos();
}

void SpriteCanvas::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && !shapeMode) {
        drawAt(event->pos().x(), event->pos().y());
    }
}

void SpriteCanvas::mouseReleaseEvent(QMouseEvent *event) {
    if (!shapeMode) return;
    endPoint = event->pos();
    drawShape(startPoint, endPoint);
}

/**
 * @brief Draws the selected shape on the canvas.
 */
void SpriteCanvas::drawShape(const QPoint &start, const QPoint &end) {
    int x1 = start.x() / pixelSize;
    int y1 = start.y() / pixelSize;
    int x2 = end.x() / pixelSize;
    int y2 = end.y() / pixelSize;

    QImage currentImage = getCurrentImage();
    QPainter painter(&currentImage);
    painter.setPen(erasing ? Qt::transparent : drawColor);

    int left = std::min(x1, x2);
    int top = std::min(y1, y2);
    int width = std::abs(x2 - x1);
    int height = std::abs(y2 - y1);
    int size = std::max(width, height);

    if (currentShape == "Square") {
        painter.drawRect(QRect(left, top, size, size));
    } else if (currentShape == "Rectangle") {
        painter.drawRect(QRect(left, top, width, height));
    } else if (currentShape == "Circle") {
        painter.drawEllipse(QRect(left, top, size, size));
    } else if (currentShape == "Line") {
        if (height > width) {
            painter.drawLine(x1, y1, x1, y2);
        } else {
            painter.drawLine(x1, y1, x2, y1);
        }
    }

    setCurrentImage(currentImage);
}

void SpriteCanvas::drawAt(int x, int y) {
    int gridX = x / pixelSize;
    int gridY = y / pixelSize;
    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
        QImage currentImage = getCurrentImage();
        currentImage.setPixelColor(gridX, gridY, erasing ? Qt::transparent : drawColor);
        setCurrentImage(currentImage);
    }
}

/**
 * @brief Saves the current sprite project to a JSON file.
 * @param filePath The destination file path.
 * @return True if saved successfully, false otherwise.
 */
bool SpriteCanvas::saveToJson(const QString &filePath) {
    QJsonObject root;
    root["width"] = gridWidth;
    root["height"] = gridHeight;
    root["frameCount"] = frames.size();

    QJsonArray framesArray;
    for (int frameIndex = 0; frameIndex < frames.size(); frameIndex++) {
        QJsonObject frameObj;
        QImage frameImage = frames[frameIndex].image();
        QJsonArray pixelRows;
        for (int y = 0; y < gridHeight; ++y) {
            QJsonArray row;
            for (int x = 0; x < gridWidth; ++x) {
                QColor color = frameImage.pixelColor(x, y);
                QJsonArray pixel = { color.red(), color.green(), color.blue(), color.alpha() };
                row.append(pixel);
            }
            pixelRows.append(row);
        }
        frameObj["pixels"] = pixelRows;
        framesArray.append(frameObj);
    }
    root["frames"] = framesArray;
    root["currentFrame"] = currentFrameIndex;

    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(doc.toJson());
    return true;
}

/**
 * @brief Loads a sprite project from a JSON file.
 * @param filePath The file path to load from.
 * @return True if loaded successfully, false otherwise.
 */
bool SpriteCanvas::loadFromJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    int w = root["width"].toInt();
    int h = root["height"].toInt();
    int frameCount = root["frameCount"].toInt(1);

    frames.clear();

    QJsonArray framesArray = root["frames"].toArray();
    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
        QJsonObject frameObj = framesArray[frameIndex].toObject();
        QJsonArray pixelRows = frameObj["pixels"].toArray();
        SpriteFrame newFrame(w, h);
        QImage frameImage = newFrame.image();

        for (int y = 0; y < h; ++y) {
            QJsonArray row = pixelRows[y].toArray();
            for (int x = 0; x < w; ++x) {
                QJsonArray pixel = row[x].toArray();
                QColor color(pixel[0].toInt(), pixel[1].toInt(), pixel[2].toInt(), pixel[3].toInt());
                frameImage.setPixelColor(x, y, color);
            }
        }
        newFrame.setImage(frameImage);
        frames.append(newFrame);
    }

    currentFrameIndex = root["currentFrame"].toInt(0);
    if (currentFrameIndex >= frames.size()) currentFrameIndex = 0;
    if (frames.isEmpty()) addNewFrame();

    emit currentFrameChanged(currentFrameIndex);
    update();
    return true;
}

/**
 * @brief Resizes the sprite canvas to the new width and height.
 * @param newWidth New canvas width in pixels.
 * @param newHeight New canvas height in pixels.
 */
void SpriteCanvas::resizeCanvas(int newWidth, int newHeight) {
    gridWidth = newWidth;
    gridHeight = newHeight;

    QImage blankImage(gridWidth, gridHeight, QImage::Format_RGBA8888);
    blankImage.fill(Qt::transparent);

    if (!frames.isEmpty()) {
        frames[currentFrameIndex].setImage(blankImage);
        emit frameContentChanged(currentFrameIndex);
    }

    updateGeometry();
    update();
}
