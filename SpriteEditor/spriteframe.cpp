/**
 * @file spriteframe.cpp
 * @author Your Name
 * @course Your Course
 * @brief Implementation of a single frame used in the sprite animation.
 * @checkedby Reviewer Name
 */

#include "spriteframe.h"

/**
 * @brief Constructs a blank SpriteFrame of specified dimensions.
 * @param width Width of the frame in pixels.
 * @param height Height of the frame in pixels.
 */
SpriteFrame::SpriteFrame(int width, int height)
    : m_image(width, height, QImage::Format_RGBA8888)
{
    m_image.fill(Qt::transparent);
}

/**
 * @brief Constructs a SpriteFrame from an existing QImage.
 * @param frameImage Image to use for this frame.
 */
SpriteFrame::SpriteFrame(const QImage &frameImage)
    : m_image(frameImage)
{
}

/**
 * @brief Gets the image associated with this frame.
 * @return QImage representing the current frame.
 */
QImage SpriteFrame::image() const
{
    return m_image;
}

/**
 * @brief Sets the image data for this frame.
 * @param image New image to assign.
 */
void SpriteFrame::setImage(const QImage &image)
{
    m_image = image;
}
