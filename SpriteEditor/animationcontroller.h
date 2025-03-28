#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <QObject>
#include <QTimer>

class SpriteCanvas;

class AnimationController : public QObject
{
    Q_OBJECT

public:
    explicit AnimationController(SpriteCanvas* canvas, QObject *parent = nullptr);
    ~AnimationController();

    bool isPlaying() const;
    int getFps() const;
    void setFps(int fps);

public slots:
    void play();
    void pause();
    void togglePlayPause();
    void nextFrame();
    void previousFrame();
    void stop(); // Stop and reset to first frame

private slots:
    void onTimerTick();

private:
    void updateTimerInterval();
    SpriteCanvas* m_canvas;
    QTimer* m_timer;
    bool m_playing;
    int m_fps;
};

#endif // ANIMATIONCONTROLLER_H
