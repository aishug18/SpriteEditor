#ifndef SPRITECANVAS_H
#define SPRITECANVAS_H

#include <QWidget>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QString>

class SpriteCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit SpriteCanvas(QWidget *parent = nullptr);
    QSize sizeHint() const override;

    void setDrawColor(const QColor &color);
    void setEraserMode(bool enabled);
    void setShapeMode(QString shape);
    void setShapeMode(bool mode);
    void setColorDropper(bool mode);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;    // for shapes

private:
    static const int pixelSize = 20;
    int gridWidth = 16;
    int gridHeight = 16;
    QImage image;
    QColor drawColor;
    bool erasing;

    bool shapeMode;
    bool colorDropper;  // for if we're using the color dropper tool

    QPoint startPoint;
    QPoint endPoint;
    QString currentShape;

    void drawAt(int x, int y);
    void drawCircle(int centerX, int centerY, int radius);
};

#endif // SPRITECANVAS_H
