/**
 * @file framethumbnail.cpp
 * @author Anurup Kumar | Diya Mandot | Kunj Rathod | Aishwarya Golakoti | Morris Lu
 * @course CS 3505
 * @brief Represents a thumbnail view for a single animation frame.
 * @checkedby Morris Lu
 */

#include "framethumbnail.h"
#include <QPainter>
#include <QMouseEvent>

/**
 * @brief Constructs a FrameThumbnail widget.
 * @param parent Optional parent widget.
 */
FrameThumbnail::FrameThumbnail(QWidget *parent)
    : QWidget(parent),
    m_selected(false)
{
    setFixedSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
    setStyleSheet("background-color: transparent;");
}

/**
 * @brief Sets the image for the thumbnail, scaled to fit.
 * @param image QImage to display in the thumbnail.
 */
void FrameThumbnail::setImage(const QImage &image)
{
    m_image = image.scaled(
        THUMBNAIL_SIZE - 4,
        THUMBNAIL_SIZE - 4,
        Qt::KeepAspectRatio
        );
    update();
}

/**
 * @brief Sets the selection state of the thumbnail.
 * @param selected True if selected, false otherwise.
 */
void FrameThumbnail::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

/**
 * @brief Checks whether this thumbnail is selected.
 * @return True if selected, false otherwise.
 */
bool FrameThumbnail::isSelected() const
{
    return m_selected;
}

/**
 * @brief Provides the preferred size of the thumbnail widget.
 * @return QSize representing fixed thumbnail size.
 */
QSize FrameThumbnail::sizeHint() const
{
    return QSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
}

/**
 * @brief Paints the thumbnail including background, image, and selection state.
 * @param event Paint event (unused).
 */
void FrameThumbnail::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Draw checkerboard background to indicate transparency
    const int checkSize = 8;
    QBrush lightBrush(QColor(180, 180, 180));
    QBrush darkBrush(QColor(140, 140, 140));

    for (int y = 0; y < height(); y += checkSize) {
        for (int x = 0; x < width(); x += checkSize) {
            QRect rect(x, y, checkSize, checkSize);
            painter.fillRect(
                rect,
                ((x / checkSize) + (y / checkSize)) % 2 ? darkBrush : lightBrush
                );
        }
    }

    // Highlight background if selected
    if (m_selected) {
        painter.fillRect(rect(), QColor(255, 0, 127, 100));
    }

    // Draw thumbnail border
    painter.setPen(m_selected ? QColor(255, 0, 127) : Qt::gray);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // Draw the scaled image centered in the thumbnail
    if (!m_image.isNull()) {
        int x = (width() - m_image.width()) / 2;
        int y = (height() - m_image.height()) / 2;
        painter.drawImage(x, y, m_image);
    }
}

/**
 * @brief Handles mouse press events. Emits clicked signal on left-click.
 * @param event Pointer to the mouse event.
 */
void FrameThumbnail::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}
