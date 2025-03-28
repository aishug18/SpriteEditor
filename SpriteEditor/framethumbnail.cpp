#include "framethumbnail.h"
#include <QPainter>
#include <QMouseEvent>

FrameThumbnail::FrameThumbnail(QWidget *parent)
    : QWidget(parent),
    m_selected(false)
{
    setFixedSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
    setStyleSheet("background-color: transparent;");
}

void FrameThumbnail::setImage(const QImage &image)
{
    m_image = image.scaled(THUMBNAIL_SIZE - 4, THUMBNAIL_SIZE - 4, Qt::KeepAspectRatio);
    update();
}

void FrameThumbnail::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

bool FrameThumbnail::isSelected() const
{
    return m_selected;
}

QSize FrameThumbnail::sizeHint() const
{
    return QSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
}

void FrameThumbnail::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Draw checkerboard background for transparency
    const int checkSize = 8;
    QBrush lightBrush(QColor(180, 180, 180));
    QBrush darkBrush(QColor(140, 140, 140));

    for (int y = 0; y < height(); y += checkSize) {
        for (int x = 0; x < width(); x += checkSize) {
            QRect rect(x, y, checkSize, checkSize);
            painter.fillRect(rect, ((x / checkSize) + (y / checkSize)) % 2 ? darkBrush : lightBrush);
        }
    }

    // Draw selection background if selected
    if (m_selected) {
        painter.fillRect(rect(), QColor(255, 0, 127, 100));
    }

    // Draw border
    painter.setPen(m_selected ? QColor(255, 0, 127) : Qt::gray);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // Draw the image centered
    if (!m_image.isNull()) {
        int x = (width() - m_image.width()) / 2;
        int y = (height() - m_image.height()) / 2;
        painter.drawImage(x, y, m_image);
    }
}

void FrameThumbnail::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}
