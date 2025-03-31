/**
 * @file animationcontroller.cpp
 * @author Anurup Kumar | Diya Mandot | Kunj Rathod | Aishwarya Golakoti | Morris Lu
 * @course CS 3505
 * @brief Controls the playback and timing of sprite animations.
 * @checkedby Kunj Rathod
 */

#include "animationcontroller.h"
#include "spritecanvas.h"

/**
 * @brief Constructs the AnimationController and sets up the timer.
 * @param canvas Pointer to the SpriteCanvas associated with this controller.
 * @param parent Optional parent QObject.
 */
AnimationController::AnimationController(SpriteCanvas *canvas, QObject *parent)
    : QObject(parent),
    m_canvas(canvas),
    m_playing(false),
    m_fps(12)  // Default frames per second
{
    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, &AnimationController::onTimerTick);

    updateTimerInterval();
}

/**
 * @brief Destructor. Stops the timer.
 */
AnimationController::~AnimationController()
{
    m_timer->stop();
}

/**
 * @brief Checks if the animation is currently playing.
 * @return true if playing, false otherwise.
 */
bool AnimationController::isPlaying() const
{
    return m_playing;
}

/**
 * @brief Gets the current FPS value.
 * @return Integer value representing frames per second.
 */
int AnimationController::getFps() const
{
    return m_fps;
}

/**
 * @brief Sets the frames per second for playback.
 * @param fps New frame rate (must be between 1 and 60).
 */
void AnimationController::setFps(int fps)
{
    if (fps > 0 && fps <= 60 && fps != m_fps) {
        m_fps = fps;
        updateTimerInterval();
    }
}

/**
 * @brief Starts playing the animation if not already playing.
 */
void AnimationController::play()
{
    if (!m_playing && m_canvas->getFrameCount() > 1) {
        m_playing = true;
        m_timer->start();
    }
}

/**
 * @brief Pauses the animation playback.
 */
void AnimationController::pause()
{
    if (m_playing) {
        m_playing = false;
        m_timer->stop();
    }
}

/**
 * @brief Toggles between play and pause.
 */
void AnimationController::togglePlayPause()
{
    if (m_playing) {
        pause();
    } else {
        play();
    }
}

/**
 * @brief Advances to the next frame in the animation.
 */
void AnimationController::nextFrame()
{
    if (m_canvas->getFrameCount() > 1) {
        int nextFrameIndex = (m_canvas->getCurrentFrameIndex() + 1)
        % m_canvas->getFrameCount();

        m_canvas->setCurrentFrame(nextFrameIndex);
    }
}

/**
 * @brief Moves to the previous frame in the animation.
 */
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

/**
 * @brief Stops the animation and resets to the first frame.
 */
void AnimationController::stop()
{
    pause();
    m_canvas->setCurrentFrame(0);
}

/**
 * @brief Callback for the timer tick. Advances to the next frame.
 */
void AnimationController::onTimerTick()
{
    nextFrame();
}

/**
 * @brief Updates the timer interval based on the current FPS.
 */
void AnimationController::updateTimerInterval()
{
    int interval = 1000 / m_fps;  // Convert FPS to milliseconds
    m_timer->setInterval(interval);
}
