#pragma once

#include "status/ffmpeg_status_detail.hpp"

struct AVPacket;

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
    FFmpegStatusDetail ensure_allocated() noexcept;
    AVPacket* get()noexcept;
    const AVPacket* get() const noexcept;
    AVPacket* release()noexcept;
    void reset();
    FFmpegStatusDetail ref(const AVPacketPtr& other);
    void unref()noexcept;
    AVPacket& operator*();
    AVPacket* operator->()noexcept;
    void swap(AVPacketPtr& other)noexcept;
private:
    AVPacket* m_packet = nullptr;
};