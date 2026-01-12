#include <QTimer>
#include <QFileInfo>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/common/core/variable_util.hpp>

#include "status/ffmpeg_status_detail.hpp"
#include "worker/media_decode_worker.hpp"
#include "codec/av_packet_ptr.hpp"

#define FFMPEG_VERSION 771

MediaDecodeWorker::MediaDecodeWorker(QObject* parent) :QObject(parent) {}
MediaDecodeWorker::~MediaDecodeWorker() {}
void MediaDecodeWorker::init()
{
    if (!m_format_context)
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to init!");
        return;
    }
    m_decode_timer = new QTimer(this);
    connect(m_decode_timer, &QTimer::timeout, this, &MediaDecodeWorker::decode_tick);
    m_decode_timer->start(10);
}

void MediaDecodeWorker::decode_tick()
{
    if (is_update_file_path)
    {
        init_decoder();
    }
    if (is_update_file_path)
    {
        return;
    }
    if (!m_format_context.is_open_input())
    {
        return;
    }
    if (m_is_paused)
    {
        return;
    }
    decode_to_packet();
}
uint64_t MediaDecodeWorker::get_media_duration() const
{
    if (!m_format_context)
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to get video duration:m_format_context is null!");
        return 0;
    }
    /// @brief 通过duration获取总时长
    uint64_t duration = m_format_context->duration;
    return duration / AV_TIME_BASE;
}
void MediaDecodeWorker::set_media_file_path(QString video_file_path)
{
    m_video_file_path = video_file_path;
    is_update_file_path = true;
}

bool MediaDecodeWorker::init_decoder()
{
    if (!m_format_context)
    {
        return false;
    }
    if (m_format_context.is_open_input())
    {
        m_format_context.close_input();
        m_codec_contexts.clear();
        m_default_video_stream_index.reset();
        m_default_audio_stream_index.reset();
    }
    if (m_video_file_path.isEmpty())
    {
        return false;
    }
    auto file_info = QFileInfo(m_video_file_path);
    if (!file_info.exists())
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to set media file path: file not exists!");
        return false;
    }
    is_update_file_path = false;
    /// @brief 打开输入流并读取标头。编解码器未打开。
    auto open_input_status =
        m_format_context.open_input(m_video_file_path.toStdString().c_str(), nullptr, nullptr);
    if (open_input_status.is_error())
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to open input: {}", open_input_status.message());
        return false;
    }
    SessionEntity session;
    session.session_id = m_session_id;
    emit session_ready(session);
    /// @brief 新增：探测流信息,不调用不能获取duration
    auto find_stream_status =
        m_format_context.find_stream_info(nullptr);
    if (find_stream_status.is_error())
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to find stream: {}", find_stream_status.message());
        /// @brief 关闭输入流
        m_format_context.close_input();
        return false;
    }
    auto default_video_stream_index = av_find_best_stream(m_format_context.get(), AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (default_video_stream_index >= 0)
    {
        m_default_video_stream_index = default_video_stream_index;
    }
    auto default_audio_stream_index = av_find_best_stream(m_format_context.get(), AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (default_audio_stream_index >= 0)
    {
        m_default_audio_stream_index = default_audio_stream_index;
    }
    if (!m_default_video_stream_index.has_value() && !m_default_audio_stream_index.has_value())
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to find default stream!");
        return false;
    }
    /// @brief 遍历媒体流
    for (unsigned int i = 0;i < m_format_context->nb_streams;i++)
    {
        if (i != m_default_video_stream_index.value_or(i + 1) &&
            i != m_default_audio_stream_index.value_or(i + 1))
        {
            continue;
        }
        /// @brief 获取当前下标的媒体流
        AVStream* stream = m_format_context->streams[i];
        /// @brief 获取当前下标媒体流的参数
        /// @note 高版本无法直接通过nb_streams[i]获取参数
        auto codecpar = stream->codecpar;

        m_codec_contexts.emplace(i, AVCodecContextPtrPending{ AVCodecContextPtr(),codecpar->codec_type,0,stream->time_base });
        const AVCodec* codec =
            avcodec_find_decoder(codecpar->codec_id);
        if (!codec)
        {
            DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to find decoder for codec id:{}");
            return false;
        }
        /// @brief 创建解码器上下文
        m_codec_contexts[i].codec_context.alloc_context3(codec);
        /// @brief 将解码参数复制到解码器上下文
        auto param_to_context_status =
            m_codec_contexts[i].codec_context.parameters_to_context(codecpar);
        if (param_to_context_status.is_error())
        {
            DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to copy params to context:{}", param_to_context_status.message());
            return false;
        }
        auto open2_status =
            m_codec_contexts[i].codec_context.open2(codec, nullptr);
        if (open2_status.is_error())
        {
            DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to open2 codec:{}", open2_status.message());
            return false;
        }
        DANEJOE_LOG_TRACE("default", "decode_mp4", "Video decoder name:{}", codec->name);
    }
    return true;
}

bool MediaDecodeWorker::decode_to_packet()
{
    /// @brief 视频压缩数据
    AVPacketPtr packet;
    auto allocated_status = packet.ensure_allocated();
    if (allocated_status.is_error())
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "Failed to allocate packet:{}", allocated_status.message());
        return false;
    }
    /// @brief 读取视频数据
    auto read_frame_error =
        m_format_context.read_frame(packet);
    if (read_frame_error.is_error())
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "错误信息: {}", FFmpegStatusDetail(read_frame_error).message());
        return false;
    }
    else if (read_frame_error.is_branch())
    {
        /// @todo 调整返回值语义，处理分支
        return true;
    }
    decode_to_frame(packet->stream_index, packet);
    return false;
}

bool MediaDecodeWorker::decode_to_frame(unsigned int stream_index, AVPacketPtr& packet)
{
    if (!packet)
    {
        DANEJOE_LOG_ERROR("default", "decode_mp4", "packet is null!");
        return false;
    }
    auto codec_context_pendding_it = m_codec_contexts.find(stream_index);
    if (codec_context_pendding_it == m_codec_contexts.end())
    {
        return true;
    }
    bool has_send_packet = false;
    while (true)
    {
        if (!has_send_packet)
        {
            auto send_packet_status =
                codec_context_pendding_it->second.codec_context.send_packet(packet);
            if (send_packet_status.is_error())
            {
                return false;
            }
            else if (send_packet_status.is_ok())
            {
                has_send_packet = true;
            }
        }
        AVFramePtr frame;
        frame.ensure_allocated();
        auto receive_frame_status =
            codec_context_pendding_it->second.codec_context.receive_frame(frame);
        if (receive_frame_status.is_error())
        {
            return false;
        }
        else if (receive_frame_status.is_ok())
        {
            int64_t relative_time_us = av_rescale_q(frame->pts, codec_context_pendding_it->second.time_base, AVRational{ 1, 1000000 });
            switch (codec_context_pendding_it->second.type)
            {
                case AVMEDIA_TYPE_VIDEO:
                {
                    auto video_frame_id =
                        codec_context_pendding_it->second.frame_id++;
                    emit video_frame_ready({ m_session_id,video_frame_id,relative_time_us,frame });
                }
                break;
                case AVMEDIA_TYPE_AUDIO:
                {
                    auto audio_frame_id =
                        codec_context_pendding_it->second.frame_id++;
                    emit audio_frame_ready({ m_session_id,audio_frame_id,relative_time_us,frame });
                    // DANEJOE_LOG_DEBUG("default", "decode_mp4", "audio frame_id:{}", audio_frame_id);
                }
                break;
                default:
                    break;
            }
            continue;
        }
        else if (receive_frame_status == FFmpegStatusDetail(AVERROR(EAGAIN)))
        {
            if (has_send_packet)
            {
                break;
            }
            else
            {
                continue;
            }
        }
        else if (receive_frame_status == FFmpegStatusDetail(AVERROR_EOF))
        {
            break;
        }
        else
        {
            break;
        }
    }
    return true;
}

void MediaDecodeWorker::on_init() { init(); }

void MediaDecodeWorker::on_decode_paused(bool is_paused)
{
    m_is_paused = is_paused;
}

void MediaDecodeWorker::on_decode_media_file(int64_t session_id, QString video_file_path)
{
    m_video_file_path = video_file_path;
    m_session_id = session_id;
    is_update_file_path = true;
}