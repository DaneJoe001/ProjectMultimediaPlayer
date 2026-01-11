extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}
#include <danejoe/logger/logger_manager.hpp>
#include "codec/av_format_context_ptr.hpp"

AVFormatContextPtr::AVFormatContextPtr()
{
    m_av_format_context = avformat_alloc_context();
}
AVFormatContextPtr::AVFormatContextPtr(AVFormatContext* av_format_context)
    :m_av_format_context(av_format_context)
{}
FFmpegStatusDetail AVFormatContextPtr::open_input(const std::string& file_path, AVInputFormat* fmt, AVDictionary** options)
{
    if (!m_av_format_context)
    {
        m_av_format_context = avformat_alloc_context();
    }
    FFmpegStatusDetail status_detail = avformat_open_input(&m_av_format_context, file_path.c_str(), fmt, options);
    if (status_detail.is_ok())
    {
        m_is_open_input = true;
    }
    return status_detail;
}
void AVFormatContextPtr::close_input()
{
    if (m_av_format_context)
    {
        avformat_close_input(&m_av_format_context);
        m_is_open_input = false;
    }
}
AVFormatContext* AVFormatContextPtr::get()const
{
    return m_av_format_context;
}
AVFormatContext* AVFormatContextPtr::operator->()const
{
    return m_av_format_context;
}
AVFormatContextPtr::~AVFormatContextPtr()
{
    if (!m_av_format_context)
    {
        return;
    }
    if (m_is_open_input)
    {
        close_input();
    }
    else
    {
        avformat_free_context(m_av_format_context);
    }
}

AVFormatContextPtr::operator bool()const
{
    return m_av_format_context != nullptr;
}

FFmpegStatusDetail AVFormatContextPtr::find_stream_info(AVDictionary** options)
{
    if (!m_av_format_context)
    {
        DANEJOE_LOG_WARN("default", "AVFormatContextPtr", "Failed to find stream info,av_format_context is null");
        return FFmpegStatusDetail(AVERROR(EINVAL));
    }
    return FFmpegStatusDetail(avformat_find_stream_info(m_av_format_context, options));
}

FFmpegStatusDetail AVFormatContextPtr::read_frame(AVPacketPtr& packet)
{
    if (!m_av_format_context)
    {
        DANEJOE_LOG_WARN("default", "AVFormatContextPtr", "Failed to read frame,av_format_context is null");
        return FFmpegStatusDetail(AVERROR(EINVAL));
    }
    if (!packet.get())
    {
        return FFmpegStatusDetail(AVERROR(EINVAL));
    }
    return FFmpegStatusDetail(av_read_frame(m_av_format_context, packet.get()));
}

bool AVFormatContextPtr::is_open_input()const
{
    return m_is_open_input;
}