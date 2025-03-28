#ifndef ANIMATIONPREVIEW_H
#define ANIMATIONPREVIEW_H

#include <QWidget>
#include <QVector>
#include <QImage>

class SpriteCanvas;

class AnimationPreview : public QWidget
{
    Q_OBJECT

public:
    explicit AnimationPreview(SpriteCanvas* canvas, QWidget *parent = nullptr);
    QSize sizeHint() const override;

public slots:
    void updateCurrentFrame(int frameIndex);
    void refreshFrames();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    SpriteCanvas* m_canvas;
    int m_currentFrame;
    QVector<QImage> m_frames;
    int m_pixelSize;
    void calculatePixelSize();
};

#endif // ANIMATIONPREVIEW_H
