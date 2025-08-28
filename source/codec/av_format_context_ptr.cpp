#include "codec/av_format_context_ptr.hpp"

AVFormatContextPtr::AVFormatContextPtr()
{
    m_av_format_context = avformat_alloc_context();
}
AVFormatContextPtr::AVFormatContextPtr(AVFormatContext* av_format_context)
{
    m_av_format_context = av_format_context;
}
AVError AVFormatContextPtr::open_input(const std::string& file_path, AVInputFormat* fmt, AVDictionary** options)
{
    return AVError(avformat_open_input(&m_av_format_context, file_path.c_str(), fmt, options));
}
void AVFormatContextPtr::close_input()
{
    if (m_av_format_context)
    {
        avformat_close_input(&m_av_format_context);
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
    close_input();
    if (m_av_format_context)
    {
        avformat_free_context(m_av_format_context);
    }
}

AVFormatContextPtr::operator bool()const
{
    return m_av_format_context != nullptr;
}

AVError AVFormatContextPtr::find_stream_info(AVDictionary** options)
{
    return AVError(avformat_find_stream_info(m_av_format_context, options));
}

AVError AVFormatContextPtr::read_frame(AVPacketPtr packet)
{
    return AVError(av_read_frame(m_av_format_context, packet.get()));
}