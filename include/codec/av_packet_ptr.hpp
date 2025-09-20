#pragma once

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}

#include "codec/av_error.hpp"

class AVPacketPtr
{
public:
    AVPacketPtr() noexcept;
    ~AVPacketPtr();
    AVPacketPtr(const AVPacketPtr& other);
    AVPacketPtr(AVPacketPtr&& other)noexcept;
    AVPacketPtr& operator=(const AVPacketPtr& other);
    AVPacketPtr& operator=(AVPacketPtr&& other)noexcept;
    operator bool()const;
    AVError ensure_allocated() noexcept;
    AVPacket* get()noexcept;
    AVPacket* release()noexcept;
    void reset();
    AVError ref(AVPacketPtr other);
    void unref()noexcept;
    AVError get_error() const noexcept;
    AVPacket& operator*();
    AVPacket* operator->()noexcept;
    void swap(AVPacketPtr& other)noexcept;
private:
    AVError m_error;
    AVPacket* m_packet = nullptr;
};