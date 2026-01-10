#include <QThread>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#include <danejoe/logger/logger_manager.hpp>
#include "service/media_decode_service.hpp"
#include "worker/media_decode_worker.hpp"

MediaDecodeService::MediaDecodeService(QObject* parent) :QObject(parent) {}
MediaDecodeService::~MediaDecodeService()
{
    if (m_decode_thread)
    {
        m_decode_thread->quit();
        m_decode_thread->wait();
    }
    if (m_decode_worker)
    {
        m_decode_worker->deleteLater();
    }
}
void MediaDecodeService::init()
{
    m_decode_thread = new QThread();
    m_decode_worker = new MediaDecodeWorker();
    m_decode_worker->moveToThread(m_decode_thread);
    connect(this, &MediaDecodeService::decode_video_file, m_decode_worker, &MediaDecodeWorker::on_decode_media_file);
    connect(m_decode_worker, &MediaDecodeWorker::video_frame_ready, this, &MediaDecodeService::on_video_frame_ready);
    connect(m_decode_thread, &QThread::started, m_decode_worker, &MediaDecodeWorker::on_init);
    m_decode_thread->start();
}

void MediaDecodeService::on_video_frame_ready(SessionFrame frame)
{
    emit video_frame_ready(frame);
}

void MediaDecodeService::on_decode_media_file(QString video_file_path)
{
    auto session_id = m_session_id++;
    emit decode_video_file(session_id, video_file_path);
}