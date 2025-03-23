#ifndef SPRITECANVAS_H
#define SPRITECANVAS_H

#include <QWidget>
#include <QColor>
#include <QImage>

class SpriteCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit SpriteCanvas(QWidget *parent = nullptr);
    QSize sizeHint() const override;

    void setDrawColor(const QColor &color);
    void setEraserMode(bool enabled);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    static const int pixelSize = 20;
    int gridWidth = 16;
    int gridHeight = 16;
    QImage image;
    QColor drawColor;
    bool erasing;

    void drawAt(int x, int y);
};

#endif // SPRITECANVAS_H
