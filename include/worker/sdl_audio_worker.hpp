#pragma once

#include <atomic>

#include <QObject>

#include "model/session_frame.hpp"
#include "audio/sdl_audio_renderer.hpp"
#include "service/time_service.hpp"

class SDLAudioWorker : public QObject
{
    Q_OBJECT
public:
    SDLAudioWorker(std::shared_ptr<TimeService> time_service, QObject* parent = nullptr);
    void init();
    void request_stop();
    void clear_stop();
public slots:
    void on_init();
    void on_start_audio();
    void on_stop_audio();
    void on_frame_ready(SessionFrame frame);
private:
    std::atomic<bool> m_is_stop = false;
    SDLAudioRenderer m_audio_renderer;
};