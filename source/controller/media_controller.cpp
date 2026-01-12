#include "controller/media_controller.hpp"
#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/concurrent/timer/timer_manager.hpp>
#include <chrono>


MediaController::MediaController(
    std::shared_ptr<TimeService> time_service,
    QObject* parent) :
    QObject(parent),
    m_time_service(time_service)
{}

MediaController::~MediaController()
{
    if (m_metrics_task_id != 0)
    {
        DaneJoe::TimerManager::get_instance().cancel_periodic_task(m_metrics_task_id);
        m_metrics_task_id = 0;
    }
}

void MediaController::init()
{
    m_video_frame_timer = new QTimer(this);
    m_audio_frame_timer = new QTimer(this);
    connect(m_video_frame_timer, &QTimer::timeout, this, &MediaController::on_video_frame_timer_tick);
    connect(m_audio_frame_timer, &QTimer::timeout, this, &MediaController::on_audio_frame_timer_tick);
    m_video_frame_timer->start(5);
    m_audio_frame_timer->start(5);

    using namespace std::chrono_literals;
    m_metrics_task_id = DaneJoe::TimerManager::get_instance().add_periodic_task(
        1s,
        [this]()
        {
            int64_t rendered = m_metrics_video_rendered.exchange(0);
            int64_t dropped = m_metrics_video_dropped.exchange(0);
            int64_t diff_sum_us = m_metrics_diff_sum_us.exchange(0);
            int64_t diff_count = m_metrics_diff_count.exchange(0);
            int64_t diff_max_abs_us = m_metrics_diff_max_abs_us.exchange(0);
            int64_t vq_len = m_metrics_video_queue_size.load();
            int64_t aq_len = m_metrics_audio_queue_size.load();

            double avg_ms = 0.0;
            if (diff_count > 0)
            {
                avg_ms = static_cast<double>(diff_sum_us) / static_cast<double>(diff_count) / 1000.0;
            }
            double max_ms = static_cast<double>(diff_max_abs_us) / 1000.0;

            DANEJOE_LOG_INFO(
                "default",
                "METRIC",
                "fps={} drop={} vq={} aq={} av_diff_avg={:.2f}ms av_diff_max={:.2f}ms",
                rendered,
                dropped,
                vq_len,
                aq_len,
                avg_ms,
                max_ms);
        },
        -1);
}

void MediaController::on_clear_buffer()
{
    m_audio_frames.clear();
    m_video_frames.clear();
    m_audio_pts_base_us.reset();
    m_metrics_video_queue_size.store(0);
    m_metrics_audio_queue_size.store(0);
}

void MediaController::on_session_ready(SessionEntity session)
{
    m_session = session;
    m_audio_pts_base_us.reset();
}
void MediaController::on_video_frame_ready(SessionFrame frame)
{
    m_video_frames.enqueue(frame);
    if (m_video_frames.size() >= 256)
    {
        emit paused_decode(true);
    }
    m_metrics_video_queue_size.store(static_cast<int64_t>(m_video_frames.size()));
}
void MediaController::on_audio_frame_ready(SessionFrame frame)
{
    if (!m_audio_pts_base_us.has_value())
    {
        m_audio_pts_base_us = frame.relative_time_us;
    }
    m_audio_frames.enqueue(frame);
    m_metrics_audio_queue_size.store(static_cast<int64_t>(m_audio_frames.size()));
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
    int64_t video_time_us = frame.relative_time_us;
    if (m_audio_pts_base_us.has_value())
    {
        video_time_us -= m_audio_pts_base_us.value();
    }
    constexpr int64_t AUDIO_OUTPUT_LATENCY_US = 120 * 1000;
    constexpr int64_t VIDEO_LEAD_US = 100 * 1000;
    int64_t audio_time_us = m_time_service->get_audio_time_us();
    int64_t effective_audio_time_us = audio_time_us - AUDIO_OUTPUT_LATENCY_US;
    auto diff_us = video_time_us - effective_audio_time_us - VIDEO_LEAD_US;
    if (diff_us >= 15 * 1000)
    {
        return;
    }
    else if (diff_us < -100 * 1000)
    {
        m_metrics_video_dropped.fetch_add(1);
        m_video_frames.dequeue();
        m_metrics_video_queue_size.store(static_cast<int64_t>(m_video_frames.size()));
        return;
    }
    // DANEJOE_LOG_TRACE("default", "MediaController", "Audio time clock:{},Frame relative time:{},Diff time:{}", m_time_service->get_audio_time_us(), frame.relative_time_us, diff_us);

    auto abs_diff_us = diff_us >= 0 ? diff_us : -diff_us;
    m_metrics_video_rendered.fetch_add(1);
    m_metrics_diff_sum_us.fetch_add(abs_diff_us);
    m_metrics_diff_count.fetch_add(1);
    int64_t prev_max = m_metrics_diff_max_abs_us.load();
    while (abs_diff_us > prev_max &&
        !m_metrics_diff_max_abs_us.compare_exchange_weak(prev_max, abs_diff_us))
    {
    }
    emit renderer_video_frame(m_video_frames.dequeue());
    if (m_video_frames.size() < 128)
    {
        emit paused_decode(false);
    }
    m_metrics_video_queue_size.store(static_cast<int64_t>(m_video_frames.size()));
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
            m_metrics_audio_queue_size.store(static_cast<int64_t>(m_audio_frames.size()));
            return;
        }
        emit renderer_audio_frame(m_audio_frames.dequeue());
        m_metrics_audio_queue_size.store(static_cast<int64_t>(m_audio_frames.size()));
    }
}
