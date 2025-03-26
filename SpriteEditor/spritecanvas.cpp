#include "spritecanvas.h"
#include <QPainter>
#include <QMouseEvent>

SpriteCanvas::SpriteCanvas(QWidget *parent)
    : QWidget(parent),
    image(gridWidth, gridHeight, QImage::Format_RGBA8888),
    drawColor(Qt::black),
    erasing(false)
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
    drawAt(event->pos().x(), event->pos().y());

    // for shape stuff
    startPoint = event->pos();
}

void SpriteCanvas::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        drawAt(event->pos().x(), event->pos().y());
    }
}

void SpriteCanvas::mouseReleaseEvent(QMouseEvent *event) {  // for drawing shapes
    // CONSIDER DRAWING FILLED IN SHAPES VS OUTLINES
    endPoint = event->pos();

    int x1 = startPoint.x() / pixelSize;
    int y1 = startPoint.y() / pixelSize;
    int x2 = endPoint.x() / pixelSize;
    int y2 = endPoint.y() / pixelSize;

    if (currentShape == "Rectangle") {
        for (int x = x1; x <= x2; ++x) {
            for (int y = y1; y <= y2; ++y) {
                image.setPixelColor(x, y, drawColor);
            }
        }
    }
    else if (currentShape == "Circle") {
        // Implement circle drawing logic


        int radius = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
        drawCircle(x1, y1, radius);     // this draws from center (where user clicked) to out.
    }
    else if (currentShape == "Line") {
        // Implement line drawing logic
        for (int x = x1; x <= x2; ++x) {
            for (int y = y1; y <= y2; ++y) {
                image.setPixelColor(x, y, drawColor);
            }
        }
    }
    update();
}

void SpriteCanvas::drawCircle(int centerX, int centerY, int radius) {
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x*x + y*y <= radius*radius) { // Check if within circle boundary
                drawAt(centerX + x, centerY + y);   // should be the same as below logic

                // int gridX = (centerX + x) / pixelSize;
                // int gridY = (centerY + y) / pixelSize;

                // if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
                //     image.setPixelColor(gridX, gridY, drawColor);
                // }
            }
        }
    }
}

void SpriteCanvas::drawAt(int x, int y) {
    int gridX = x / pixelSize;
    int gridY = y / pixelSize;

    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
        image.setPixelColor(gridX, gridY, erasing ? Qt::transparent : drawColor);
        update();
    }
}
