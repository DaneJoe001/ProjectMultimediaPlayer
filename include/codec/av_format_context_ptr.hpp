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

class AVFormatContextPtr {
public:
    AVFormatContextPtr();
    AVFormatContextPtr(AVFormatContext* av_format_context);
    AVError open_input(const std::string& file_path, AVInputFormat* fmt, AVDictionary** options);
    void close_input();
    AVError find_stream_info(AVDictionary** options);
    AVError read_frame(AVPacketPtr packet);
    AVFormatContext* get()const;
    AVFormatContext* operator->()const;
    operator bool()const;
    ~AVFormatContextPtr();
private:
    AVFormatContext* m_av_format_context = nullptr;
};