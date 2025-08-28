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
    return AVCodecContextPtr::get();
}

AVCodecContext* AVCodecContextPtr::operator->()const
{
    return m_codec_context;
}

void AVCodecContextPtr::alloc_context3(const AVCodec* codec)
{
    m_codec_context = avcodec_alloc_context3(codec);
}

AVError AVCodecContextPtr::open2(const AVCodec* codec, AVDictionary** options)
{
    return AVError(avcodec_open2(m_codec_context, codec, options));
}

AVError AVCodecContextPtr::parameters_to_context(const AVCodecParameters* parameters)
{
    return AVError(avcodec_parameters_to_context(m_codec_context, parameters));
}

AVError AVCodecContextPtr::send_packet(AVPacketPtr packet)
{
    return AVError(avcodec_send_packet(m_codec_context, packet.get()));
}

AVError AVCodecContextPtr::receive_frame(AVFramePtr frame)
{
    return AVError(avcodec_receive_frame(m_codec_context, frame.get()));
}