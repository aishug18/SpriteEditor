#include "animationpreview.h"
#include "spritecanvas.h"
#include <QPainter>
#include <QBrush>
#include <QResizeEvent>

AnimationPreview::AnimationPreview(SpriteCanvas* canvas, QWidget *parent)
    : QWidget(parent),
    m_canvas(canvas),
    m_currentFrame(0),
    m_pixelSize(4)
{
    setMinimumSize(80, 80);
    refreshFrames();
    calculatePixelSize();
}

QSize AnimationPreview::sizeHint() const
{
    return QSize(128, 128);
}

void AnimationPreview::updateCurrentFrame(int frameIndex)
{
    if (frameIndex >= 0 && frameIndex < m_frames.size()) {
        m_currentFrame = frameIndex;
        update();
    }
}

void AnimationPreview::refreshFrames()
{
    m_frames.clear();

    for (int i = 0; i < m_canvas->getFrameCount(); ++i) {
        QImage frame = m_canvas->getFrameImage(i);
        if (!frame.isNull()) {
            m_frames.append(frame);
        }
    }

    update();
}

void AnimationPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Draw checkerboard background to show transparency
    const int checkSize = 8;
    QBrush lightBrush(QColor(180, 180, 180));
    QBrush darkBrush(QColor(140, 140, 140));

    for (int y = 0; y < height(); y += checkSize) {
        for (int x = 0; x < width(); x += checkSize) {
            QRect rect(x, y, checkSize, checkSize);
            painter.fillRect(rect, ((x / checkSize) + (y / checkSize)) % 2 ? darkBrush : lightBrush);
        }
    }

    // Draw the current frame
    if (!m_frames.isEmpty() && m_currentFrame >= 0 && m_currentFrame < m_frames.size()) {
        const QImage& frame = m_frames[m_currentFrame];

        // Calculate the centered position
        int frameWidth = frame.width() * m_pixelSize;
        int frameHeight = frame.height() * m_pixelSize;
        int x = (width() - frameWidth) / 2;
        int y = (height() - frameHeight) / 2;

        // Draw each pixel scaled up
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

        for (int py = 0; py < frame.height(); py++) {
            for (int px = 0; px < frame.width(); px++) {
                QColor pixelColor = frame.pixelColor(px, py);
                if (pixelColor.alpha() > 0) {
                    painter.fillRect(
                        x + px * m_pixelSize,
                        y + py * m_pixelSize,
                        m_pixelSize,
                        m_pixelSize,
                        pixelColor
                        );
                }
            }
        }

        // Draw a border around the sprite
        painter.setPen(Qt::gray);
        painter.drawRect(x, y, frameWidth, frameHeight);
    }
}

void AnimationPreview::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    calculatePixelSize();
}

void AnimationPreview::calculatePixelSize()
{
    if (m_frames.isEmpty()) {
        return;
    }

    const QImage& frame = m_frames.first();
    int maxWidth = width() - 20;  // leave some margin
    int maxHeight = height() - 20;

    // Calculate the maximum pixel size that will fit
    int widthPixelSize = maxWidth / frame.width();
    int heightPixelSize = maxHeight / frame.height();

    // Use the smaller of the two to maintain aspect ratio
    m_pixelSize = qMax(1, qMin(widthPixelSize, heightPixelSize));
}
