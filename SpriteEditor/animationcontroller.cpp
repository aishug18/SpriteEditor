#include "animationcontroller.h"
#include "spritecanvas.h"

AnimationController::AnimationController(SpriteCanvas* canvas, QObject *parent)
    : QObject(parent),
    m_canvas(canvas),
    m_playing(false),
    m_fps(12) // Default FPS
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &AnimationController::onTimerTick);
    updateTimerInterval();
}

AnimationController::~AnimationController()
{
    m_timer->stop();
}

bool AnimationController::isPlaying() const
{
    return m_playing;
}

int AnimationController::getFps() const
{
    return m_fps;
}

void AnimationController::setFps(int fps)
{
    if (fps > 0 && fps <= 60 && fps != m_fps) {
        m_fps = fps;
        updateTimerInterval();
    }
}

void AnimationController::play()
{
    if (!m_playing && m_canvas->getFrameCount() > 1) {
        m_playing = true;
        m_timer->start();
    }
}

void AnimationController::pause()
{
    if (m_playing) {
        m_playing = false;
        m_timer->stop();
    }
}

void AnimationController::togglePlayPause()
{
    if (m_playing) {
        pause();
    } else {
        play();
    }
}

void AnimationController::nextFrame()
{
    if (m_canvas->getFrameCount() > 1) {
        int nextFrameIndex = (m_canvas->getCurrentFrameIndex() + 1) % m_canvas->getFrameCount();
        m_canvas->setCurrentFrame(nextFrameIndex);
    }
}

void AnimationController::previousFrame()
{
    if (m_canvas->getFrameCount() > 1) {
        int prevFrameIndex = m_canvas->getCurrentFrameIndex() - 1;
        if (prevFrameIndex < 0) {
            prevFrameIndex = m_canvas->getFrameCount() - 1;
        }
        m_canvas->setCurrentFrame(prevFrameIndex);
    }
}

void AnimationController::stop()
{
    pause();
    m_canvas->setCurrentFrame(0);
}

void AnimationController::onTimerTick()
{
    nextFrame();
}

void AnimationController::updateTimerInterval()
{
    int interval = 1000 / m_fps;
    m_timer->setInterval(interval);
}
