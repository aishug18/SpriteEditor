#ifndef SPRITEFRAME_H
#define SPRITEFRAME_H

#include <QImage>

class SpriteFrame
{
public:
    SpriteFrame(int width, int height);
    SpriteFrame(const QImage &frameImage);

    QImage image() const;
    void setImage(const QImage &image);

private:
    QImage m_image;
};

#endif // SPRITEFRAME_H
