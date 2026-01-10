#pragma once

#include <atomic>
#include <cstdint>

#include <QString>
#include <QObject>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "model/session_frame.hpp"


class QTimer;
class QThread;
class MediaDecodeWorker;

class MediaDecodeService :public QObject
{
    Q_OBJECT
public:
    MediaDecodeService(QObject* parent = nullptr);
    ~MediaDecodeService();
    void init();
signals:
    void video_frame_ready(SessionFrame frame);
    void decode_video_file(
        int64_t session_id,
        QString video_file_path);
public slots:
    void on_video_frame_ready(SessionFrame frame);
    void on_decode_media_file(QString video_file_path);
private:
    std::atomic<int64_t> m_session_id = 0;
    QThread* m_decode_thread = nullptr;
    MediaDecodeWorker* m_decode_worker = nullptr;
};