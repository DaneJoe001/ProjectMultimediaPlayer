#pragma once

#include <optional>
#include <unordered_map>

#include <QObject>

extern "C"
{
#include <libavutil/avutil.h>
}

#include "codec/av_packet_ptr.hpp"
#include "codec/av_format_context_ptr.hpp"
#include "codec/av_codec_context_ptr.hpp"
#include "model/session_entity.hpp"

#include "model/session_frame.hpp"

struct AVCodecContextPtrPending
{
    AVCodecContextPtr codec_context;
    AVMediaType type;
    int64_t frame_id;
    AVRational time_base;
};

class MediaDecodeWorker : public QObject
{
    Q_OBJECT
public:
    MediaDecodeWorker(QObject* parent = nullptr);
    ~MediaDecodeWorker();
    void init();
    uint64_t get_media_duration() const;
    void set_media_file_path(QString video_file_path);
    void decode_tick();
    bool init_decoder();
    bool decode_to_packet();
    bool decode_to_frame(unsigned int stream_index, AVPacketPtr& packet);
signals:
    void session_ready(SessionEntity session);
    void video_frame_ready(SessionFrame frame);
    void audio_frame_ready(SessionFrame frame);
public slots:
    void on_decode_paused(bool is_paused);
    void on_init();
    void on_decode_media_file(int64_t session_id, QString video_file_path);
private:
    std::unordered_map<unsigned int, AVCodecContextPtrPending> m_codec_contexts;
    int64_t m_session_id = 0;

    bool is_update_file_path = true;
    bool m_is_paused = false;
    QString m_video_file_path;
    QTimer* m_decode_timer = nullptr;

    AVFormatContextPtr m_format_context;

    std::optional<unsigned int> m_default_video_stream_index = std::nullopt;
    std::optional<unsigned int> m_default_audio_stream_index = std::nullopt;
};