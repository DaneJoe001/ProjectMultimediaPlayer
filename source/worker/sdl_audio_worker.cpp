#include "worker/sdl_audio_worker.hpp"
#include <danejoe/logger/logger_manager.hpp>
#include <atomic>

SDLAudioWorker::SDLAudioWorker(
    std::shared_ptr<TimeService> time_service,
    QObject* parent) :
    QObject(parent)
{
    m_audio_renderer.set_time_service(time_service);
}

void SDLAudioWorker::request_stop()
{
    m_is_stop.store(true);
}

void SDLAudioWorker::clear_stop()
{
    m_is_stop.store(false);
}

void SDLAudioWorker::init()
{
    on_start_audio();
}

void SDLAudioWorker::on_init()
{
    init();
}

void SDLAudioWorker::on_start_audio()
{
    clear_stop();
    m_audio_renderer.start();
}

void SDLAudioWorker::on_stop_audio()
{
    request_stop();
    m_audio_renderer.stop();
}

void SDLAudioWorker::on_frame_ready(SessionFrame frame)
{
    if (m_is_stop.load())
    {
        return;
    }
    if (!frame.frame)
    {
        return;
    }
    m_audio_renderer.append_pcm(frame.frame);
}