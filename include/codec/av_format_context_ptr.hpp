#pragma once

#include "status/ffmpeg_status_detail.hpp"
#include "codec/av_packet_ptr.hpp"

struct AVInputFormat;
struct AVDictionary;
struct AVFormatContext;

class AVFormatContextPtr
{
public:
    AVFormatContextPtr();
    AVFormatContextPtr(AVFormatContext* av_format_context);
    FFmpegStatusDetail open_input(
        const std::string& file_path,
        AVInputFormat* fmt,
        AVDictionary** options);
    void close_input();
    FFmpegStatusDetail find_stream_info(AVDictionary** options);
    FFmpegStatusDetail read_frame(AVPacketPtr& packet);
    AVFormatContext* get()const;
    bool is_valid()const;
    bool is_open_input()const;
    AVFormatContext* operator->()const;
    operator bool()const;
    ~AVFormatContextPtr();
private:
    bool m_is_open_input = false;
    AVFormatContext* m_av_format_context = nullptr;
};