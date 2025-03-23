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
}

void SpriteCanvas::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        drawAt(event->pos().x(), event->pos().y());
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
