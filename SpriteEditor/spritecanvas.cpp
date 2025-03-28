#include "spritecanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>   // for debugging, remove before submitting

SpriteCanvas::SpriteCanvas(QWidget *parent)
    : QWidget(parent),
    drawColor(Qt::black),
    erasing(false),
    shapeMode(false),
    colorDropper(false),
    currentFrameIndex(0)
{
    // Create the first frame
    addNewFrame();
}

QSize SpriteCanvas::sizeHint() const {
    return QSize(gridWidth * pixelSize, gridHeight * pixelSize);
}

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

// Frame management methods
void SpriteCanvas::addNewFrame() {
    frames.append(SpriteFrame(gridWidth, gridHeight));
    currentFrameIndex = frames.size() - 1;
    emit frameAdded(currentFrameIndex);
    emit currentFrameChanged(currentFrameIndex);
    update();
}

void SpriteCanvas::deleteCurrentFrame() {
    // Don't delete if it's the only frame
    if (frames.size() <= 1) {
        return;
    }

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
    if (frames.isEmpty()) {
        return QImage();
    }
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
    if (frames.isEmpty()) {
        return;
    }

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
    if(colorDropper) {  // Get the color of the pixel clicked
        int gridX = event->pos().x() / pixelSize;
        int gridY = event->pos().y() / pixelSize;

        if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
            QImage currentImage = getCurrentImage();
            setDrawColor(currentImage.pixelColor(gridX, gridY));
        }
    }
    else if(!shapeMode) {
        drawAt(event->pos().x(), event->pos().y());
    }

    startPoint = event->pos();  // For drawing a shape
}

void SpriteCanvas::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && !shapeMode) {
        drawAt(event->pos().x(), event->pos().y());
    }
}

void SpriteCanvas::mouseReleaseEvent(QMouseEvent *event) {
    if(!shapeMode) {    // Don't draw any shapes if not in shape mode
        return;
    }

    endPoint = event->pos();
    drawShape(startPoint, endPoint);
}

void SpriteCanvas::drawShape(const QPoint &start, const QPoint &end) {
    int x1 = start.x() / pixelSize;
    int y1 = start.y() / pixelSize;
    int x2 = end.x() / pixelSize;
    int y2 = end.y() / pixelSize;

    QImage currentImage = getCurrentImage();
    QPainter painter(&currentImage);
    painter.setPen(erasing ? Qt::transparent : drawColor);  // Draws outline

    // Get the geometry for shape size
    int left = std::min(x1, x2);
    int top = std::min(y1, y2);
    int width = std::abs(x2 - x1);
    int height = std::abs(y2 - y1);
    int size = std::max(width, height); // Used for a square and a perfect circle

    if (currentShape == "Square") {
        painter.drawRect(QRect(left, top, size, size));
    }
    else if (currentShape == "Rectangle") {
        painter.drawRect(QRect(left, top, width, height));
    }
    else if (currentShape == "Circle") {    // Draws a perfect circle
        painter.drawEllipse(QRect(left, top, size, size));
    }
    else if (currentShape == "Line") {  // Only draw vertical or horizontal lines
        if(height > width) {  // Vertical line
            painter.drawLine(x1, y1, x1, y2);
        }
        else {  // Horizontal line
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
                QJsonArray pixel;
                pixel.append(color.red());
                pixel.append(color.green());
                pixel.append(color.blue());
                pixel.append(color.alpha());
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

bool SpriteCanvas::loadFromJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    int w = root["width"].toInt();
    int h = root["height"].toInt();
    int frameCount = root["frameCount"].toInt(1); // Default to 1 if not specified

    // Clear existing frames
    frames.clear();

    // Load all frames
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

    // Set current frame
    currentFrameIndex = root["currentFrame"].toInt(0);
    if (currentFrameIndex >= frames.size()) {
        currentFrameIndex = 0;
    }

    // If no frames were loaded, create at least one
    if (frames.isEmpty()) {
        addNewFrame();
    }

    emit currentFrameChanged(currentFrameIndex);
    update();
    return true;
}
