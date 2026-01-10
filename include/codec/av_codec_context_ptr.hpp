#pragma once

#include "status/ffmpeg_status_detail.hpp"
#include "codec/av_packet_ptr.hpp"
#include "codec/av_frame_ptr.hpp"

struct AVCodec;
struct AVDictionary;
struct AVCodecContext;
struct AVCodecParameters;

class AVCodecContextPtr
{
public:
    AVCodecContextPtr();
    ~AVCodecContextPtr();
    AVCodecContext* get()const;
    bool alloc_context3(const AVCodec* codec);
    FFmpegStatusDetail send_packet(AVPacketPtr& packet);
    FFmpegStatusDetail receive_frame(AVFramePtr& frame);
    FFmpegStatusDetail parameters_to_context(const AVCodecParameters* parameters);
    FFmpegStatusDetail open2(
        const AVCodec* codec,
        AVDictionary** options);
    AVCodecContext* operator->()const;
private:
    AVCodecContext* m_codec_context = nullptr;
};