#include "spriteframe.h"

SpriteFrame::SpriteFrame(int width, int height)
    : m_image(width, height, QImage::Format_RGBA8888)
{
    m_image.fill(Qt::transparent);
}

SpriteFrame::SpriteFrame(const QImage &frameImage)
    : m_image(frameImage)
{
}

QImage SpriteFrame::image() const
{
    return m_image;
}

void SpriteFrame::setImage(const QImage &image)
{
    m_image = image;
}
