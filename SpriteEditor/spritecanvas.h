#ifndef SPRITECANVAS_H
#define SPRITECANVAS_H

#include <QWidget>
#include <QColor>
#include <QImage>
#include <QVector>
#include <QPoint>
#include "spriteframe.h"

class SpriteCanvas : public QWidget
{
    Q_OBJECT

public:
    //------------------------------------------------------
    //explicit SpriteCanvas(QWidget *parent = nullptr);
    //QSize sizeHint() const override;

    explicit SpriteCanvas(QWidget *parent = nullptr);
    void resizeCanvas(int newWidth, int newHeight);
    QSize sizeHint() const override;

    void setDrawColor(const QColor &color);
    void setEraserMode(bool enabled);
    void setShapeMode(QString shape);
    void setShapeMode(bool mode);
    void setColorDropper(bool mode);

    // Frame management
    void addNewFrame();
    void deleteCurrentFrame();
    void setCurrentFrame(int frameIndex);
    int getCurrentFrameIndex() const;
    int getFrameCount() const;
    QImage getFrameImage(int frameIndex) const;
    void duplicateCurrentFrame();

    // Saving & Loading
    bool saveToJson(const QString &filePath);
    bool loadFromJson(const QString &filePath);

signals:
    void frameAdded(int frameIndex);
    void frameRemoved(int frameIndex);
    void currentFrameChanged(int frameIndex);
    void frameContentChanged(int frameIndex);
    void colorChanged(const QColor &newColor);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    static const int pixelSize = 10;

    //-----------------------------------
    //QImage image;
    int gridWidth = 32;
    int gridHeight = 32;

    // Frame management
    QVector<SpriteFrame> frames;
    int currentFrameIndex;

    // Drawing tools
    QColor drawColor;
    bool erasing;
    bool shapeMode;
    bool colorDropper;
    QString currentShape;
    QPoint startPoint;
    QPoint endPoint;

    void drawAt(int x, int y);
    void drawShape(const QPoint &start, const QPoint &end);
    QImage getCurrentImage() const;
    void setCurrentImage(const QImage &image);
};

#endif // SPRITECANVAS_H
