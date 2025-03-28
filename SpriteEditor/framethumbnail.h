#ifndef FRAMETHUMBNAIL_H
#define FRAMETHUMBNAIL_H

#include <QWidget>
#include <QImage>

class FrameThumbnail : public QWidget
{
    Q_OBJECT

public:
    explicit FrameThumbnail(QWidget *parent = nullptr);
    void setImage(const QImage &image);
    void setSelected(bool selected);
    bool isSelected() const;
    QSize sizeHint() const override;

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QImage m_image;
    bool m_selected;
    static const int THUMBNAIL_SIZE = 64;
};

#endif // FRAMETHUMBNAIL_H
