#pragma once

#include <queue>
#include <memory>

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
    void init();
    void stop_audio();
    void stop_video();
signals:
    void renderer_video_frame(SessionFrame frame);
    void renderer_audio_frame(SessionFrame frame);
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
};