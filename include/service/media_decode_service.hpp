#pragma once

#include <atomic>
#include <cstdint>

#include <QString>
#include <QObject>
#include <QPointer>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "model/session_frame.hpp"
#include "controller/media_controller.hpp"


class QTimer;
class QThread;
class MediaDecodeWorker;

class MediaDecodeService :public QObject
{
    Q_OBJECT
public:
    MediaDecodeService(QPointer<MediaController> media_controller, QObject* parent = nullptr);
    ~MediaDecodeService();
    void init();
signals:
    void video_frame_ready(SessionFrame frame);
    void audio_frame_ready(SessionFrame frame);
    void decode_media_file(int64_t session_id, QString video_file_path);
    void paused_decode(bool is_paused);
public slots:
    void on_decode_media_file(QString video_file_path);
    void on_paused_decode(bool is_paused);
private:
    std::atomic<int64_t> m_session_id = 0;
    QThread* m_decode_thread = nullptr;
    MediaDecodeWorker* m_decode_worker = nullptr;
    QPointer<MediaController> m_media_controller = nullptr;
};