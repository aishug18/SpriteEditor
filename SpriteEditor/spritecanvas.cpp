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
    image(gridWidth, gridHeight, QImage::Format_RGBA8888),
    drawColor(Qt::black),
    erasing(false),
    shapeMode(false)    // initialize other stuff
{
    image.fill(Qt::transparent);
}

QSize SpriteCanvas::sizeHint() const {
    return QSize(gridWidth * pixelSize, gridHeight * pixelSize);
}

void SpriteCanvas::setDrawColor(const QColor &color) {
    drawColor = color;
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

void SpriteCanvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = image.pixelColor(x, y);
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
            setDrawColor(image.pixelColor(gridX, gridY));
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
    // CONSIDER DRAWING FILLED IN SHAPES VS OUTLINES?

    if(!shapeMode) {    // Don't draw any shapes if not in shape mode
        return;
    }

    endPoint = event->pos();

    int x1 = startPoint.x() / pixelSize;
    int y1 = startPoint.y() / pixelSize;
    int x2 = endPoint.x() / pixelSize;
    int y2 = endPoint.y() / pixelSize;

    QPainter painter(&image);
    painter.setPen(drawColor);  // Draws outline
    //painter.setBrush(drawColor);    // Fills in

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

    update();
}

void SpriteCanvas::drawAt(int x, int y) {
    int gridX = x / pixelSize;
    int gridY = y / pixelSize;

    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
        // If erasing is true, draw transparent pixel to erase, otherwise draw the colored pixel
        image.setPixelColor(gridX, gridY, erasing ? Qt::transparent : drawColor);
        update();
    }
}

//-----------------------------------------------------------------------------
bool SpriteCanvas::saveToJson(const QString &filePath) {
    QJsonObject root;
    root["width"] = gridWidth;
    root["height"] = gridHeight;

    QJsonArray pixelRows;
    for (int y = 0; y < gridHeight; ++y) {
        QJsonArray row;
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = image.pixelColor(x, y);
            QJsonArray pixel;
            pixel.append(color.red());
            pixel.append(color.green());
            pixel.append(color.blue());
            pixel.append(color.alpha());
            row.append(pixel);
        }
        pixelRows.append(row);
    }

    root["pixels"] = pixelRows;

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

    if (w != gridWidth || h != gridHeight) return false; // handle resizing later if needed

    QJsonArray pixelRows = root["pixels"].toArray();
    for (int y = 0; y < h; ++y) {
        QJsonArray row = pixelRows[y].toArray();
        for (int x = 0; x < w; ++x) {
            QJsonArray pixel = row[x].toArray();
            QColor color(pixel[0].toInt(), pixel[1].toInt(), pixel[2].toInt(), pixel[3].toInt());
            image.setPixelColor(x, y, color);
        }
    }

    update();
    return true;
}
