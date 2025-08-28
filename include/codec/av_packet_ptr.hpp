#pragma once

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}

class AVPacketPtr
{
public:
    AVPacketPtr();
    ~AVPacketPtr();
    AVPacketPtr(const AVPacketPtr& other);
    AVPacketPtr(AVPacketPtr&& other);
    AVPacketPtr& operator=(const AVPacketPtr& other);
    AVPacketPtr& operator=(AVPacketPtr&& other);
    operator bool()const;
    AVPacket* get();
    AVPacket* operator->();
private:
    AVPacket* m_packet = nullptr;
};