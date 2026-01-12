#pragma once

#include <queue>
#include <memory>
#include <optional>
#include <atomic>

#include <QObject>
#include <QTimer>
#include <QQueue>

#include "model/session_entity.hpp"
#include "model/session_frame.hpp"
#include "service/time_service.hpp"

struct SessionPending
{
    SessionEntity session;
    std::queue<SessionFrame> video_frames;
    std::queue<SessionFrame> audio_frames;
};

class MediaController : public QObject
{
public:
    Q_OBJECT
public:
    MediaController(std::shared_ptr<TimeService> time_service, QObject* parent = nullptr);
    ~MediaController() override;
    void init();
    void stop_audio();
    void stop_video();

signals:
    void renderer_video_frame(SessionFrame frame);
    void renderer_audio_frame(SessionFrame frame);
    void paused_decode(bool is_paused);
public slots:
    void on_clear_buffer();
    void on_session_ready(SessionEntity session);
    void on_video_frame_ready(SessionFrame frame);
    void on_audio_frame_ready(SessionFrame frame);
    void on_video_frame_timer_tick();
    void on_audio_frame_timer_tick();
private:
    std::shared_ptr<TimeService> m_time_service = nullptr;
    QTimer* m_video_frame_timer;
    QTimer* m_audio_frame_timer;
    SessionEntity m_session;
    QQueue<SessionFrame> m_video_frames;
    QQueue<SessionFrame> m_audio_frames;
    std::optional<int64_t> m_audio_pts_base_us;
    std::atomic<int64_t> m_metrics_video_rendered = 0;
    std::atomic<int64_t> m_metrics_video_dropped = 0;
    std::atomic<int64_t> m_metrics_diff_sum_us = 0;
    std::atomic<int64_t> m_metrics_diff_count = 0;
    std::atomic<int64_t> m_metrics_diff_max_abs_us = 0;
    std::atomic<int64_t> m_metrics_video_queue_size = 0;
    std::atomic<int64_t> m_metrics_audio_queue_size = 0;
    uint64_t m_metrics_task_id = 0;
};