#include "controller/media_controller.hpp"
#include <danejoe/logger/logger_manager.hpp>


MediaController::MediaController(
    std::shared_ptr<TimeService> time_service,
    QObject* parent) :
    QObject(parent),
    m_time_service(time_service)
{}

void MediaController::init()
{
    m_video_frame_timer = new QTimer(this);
    m_audio_frame_timer = new QTimer(this);
    connect(m_video_frame_timer, &QTimer::timeout, this, &MediaController::on_video_frame_timer_tick);
    connect(m_audio_frame_timer, &QTimer::timeout, this, &MediaController::on_audio_frame_timer_tick);
    m_video_frame_timer->start(10);
    m_audio_frame_timer->start(3);
}

void MediaController::on_clear_buffer()
{
    m_audio_frames.clear();
    m_video_frames.clear();
}

void MediaController::on_session_ready(SessionEntity session)
{
    m_session = session;
}
void MediaController::on_video_frame_ready(SessionFrame frame)
{
    m_video_frames.enqueue(frame);
}
void MediaController::on_audio_frame_ready(SessionFrame frame)
{
    m_audio_frames.enqueue(frame);
}

void MediaController::on_video_frame_timer_tick()
{
    if (m_video_frames.size() <= 0)
    {
        return;
    }
    auto frame = m_video_frames.front();
    if (frame.session_id != m_session.session_id)
    {
        m_video_frames.dequeue();
        return;
    }
    auto diff_us = frame.relative_time_us - m_time_service->get_audio_time_us() + (500 * 1000);
    if (diff_us >= 10 * 1000)
    {
        return;
    }
    else if (diff_us < -50 * 1000)
    {
        m_video_frames.dequeue();
        return;
    }
    // DANEJOE_LOG_TRACE("default", "MediaController", "Audio time clock:{},Frame relative time:{},Diff time:{}", m_time_service->get_audio_time_us(), frame.relative_time_us, diff_us);

    emit renderer_video_frame(m_video_frames.dequeue());
}
void MediaController::on_audio_frame_timer_tick()
{
    if (m_audio_frames.size() <= 0)
    {
        return;
    }
    if (m_audio_frames.size() > 0)
    {
        auto frame = m_audio_frames.front();
        if (frame.session_id != m_session.session_id)
        {
            m_audio_frames.dequeue();
            return;
        }
        emit renderer_audio_frame(m_audio_frames.dequeue());
    }
}
