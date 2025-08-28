#pragma once

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}

#include "codec/av_error.hpp"
#include "codec/av_packet_ptr.hpp"
#include "codec/av_frame_ptr.hpp"

class AVCodecContextPtr
{
public:
    AVCodecContextPtr();
    ~AVCodecContextPtr();
    AVCodecContext* get()const;
    void alloc_context3(const AVCodec* codec);
    AVError send_packet(AVPacketPtr packet);
    AVError receive_frame(AVFramePtr frame);
    AVError parameters_to_context(const AVCodecParameters* parameters);
    AVError open2(const AVCodec* codec, AVDictionary** options);
    AVCodecContext* operator->()const;
private:
    AVCodecContext* m_codec_context = nullptr;
};