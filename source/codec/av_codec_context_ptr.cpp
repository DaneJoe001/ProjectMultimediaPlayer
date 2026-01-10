extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}
#include <danejoe/logger/logger_manager.hpp>
#include "codec/av_codec_context_ptr.hpp"

AVCodecContextPtr::AVCodecContextPtr()
{

}

AVCodecContextPtr::~AVCodecContextPtr()
{
    if (m_codec_context)
    {
        /// @brief 释放解码器上下文
        avcodec_free_context(&m_codec_context);
    }
}

AVCodecContext* AVCodecContextPtr::get()const
{
    return m_codec_context;
}

AVCodecContext* AVCodecContextPtr::operator->()const
{
    return m_codec_context;
}

bool AVCodecContextPtr::alloc_context3(const AVCodec* codec)
{
    if (m_codec_context)
    {
        avcodec_free_context(&m_codec_context);
    }
    m_codec_context = avcodec_alloc_context3(codec);
    return m_codec_context != nullptr;
}

FFmpegStatusDetail AVCodecContextPtr::open2(const AVCodec* codec, AVDictionary** options)
{
    if (!codec)
    {
        DANEJOE_LOG_WARN("default", "AVCodecContextPtr", "Failed to open codec,codec is null");
        return FFmpegStatusDetail(AVERROR(EINVAL));
    }
    return FFmpegStatusDetail(avcodec_open2(m_codec_context, codec, options));
}

FFmpegStatusDetail AVCodecContextPtr::parameters_to_context(const AVCodecParameters* parameters)
{
    if (!parameters)
    {
        DANEJOE_LOG_WARN("default", "AVCodecContextPtr", "Failed to parameters to context,parameters is null");
        return FFmpegStatusDetail(AVERROR(EINVAL));
    }
    return FFmpegStatusDetail(avcodec_parameters_to_context(m_codec_context, parameters));
}

FFmpegStatusDetail AVCodecContextPtr::send_packet(AVPacketPtr& packet)
{
    if (!packet)
    {
        DANEJOE_LOG_WARN("default", "AVCodecContextPtr", "Failed to send packet,packet is null");
        return FFmpegStatusDetail(AVERROR(EINVAL));
    }
    return FFmpegStatusDetail(avcodec_send_packet(m_codec_context, packet.get()));
}

FFmpegStatusDetail AVCodecContextPtr::receive_frame(AVFramePtr& frame)
{
    if (!frame)
    {
        DANEJOE_LOG_WARN("default", "AVCodecContextPtr", "Failed to receive frame,frame is null");
        return FFmpegStatusDetail(AVERROR(EINVAL));
    }
    return FFmpegStatusDetail(avcodec_receive_frame(m_codec_context, frame.get()));
}