#include <libavutil/error.h>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}
#include <stdexcept>

#include "codec/av_packet_ptr.hpp"

AVPacketPtr::AVPacketPtr()noexcept
{
    m_packet = nullptr;
}

FFmpegStatusDetail AVPacketPtr::ensure_allocated() noexcept
{
    if (!m_packet)
    {
        m_packet = av_packet_alloc();
    }
    return m_packet == nullptr ? FFmpegStatusDetail(AVERROR(ENOMEM))
        : FFmpegStatusDetail(DaneJoe::StatusLevel::Ok);
}

AVPacketPtr::~AVPacketPtr()
{
    if (m_packet)
    {
        av_packet_free(&m_packet);
    }
}

void AVPacketPtr::reset()
{
    if (m_packet)
    {
        av_packet_free(&m_packet);
    }
    m_packet = nullptr;
}

AVPacketPtr::AVPacketPtr(const AVPacketPtr& other)
{
    if (!other.m_packet)
    {
        return;
    }
    auto m_error = ensure_allocated();
    if (m_error.is_error())
    {
        m_packet = nullptr;
        return;
    }
    m_error = ref(other);
    if (m_error.is_error())
    {
        av_packet_free(&m_packet);
        m_packet = nullptr;
        return;
    }
}

void AVPacketPtr::swap(AVPacketPtr& other)noexcept
{
    std::swap(m_packet, other.m_packet);
}

AVPacketPtr::AVPacketPtr(AVPacketPtr&& other)noexcept :
    m_packet(other.m_packet)
{
    other.m_packet = nullptr;
}

AVPacketPtr& AVPacketPtr::operator=(const AVPacketPtr& other)
{
    /// @brief 当赋值对象为自身时忽略
    if (this == &other)
    {
        return *this;
    }
    AVPacket* new_packet = nullptr;
    /// @brief 当源对象不为空时进行分配
    if (other.m_packet)
    {
        new_packet = av_packet_alloc();
        if (new_packet)
        {
            FFmpegStatusDetail error = av_packet_ref(new_packet, other.m_packet);
            if (error.is_error())
            {
                av_packet_free(&new_packet);
                new_packet = nullptr;
            }
        }
    }
    else
    {
        new_packet = nullptr;
    }
    if (m_packet)
    {
        /// @brief 当自身结构存在时通过unref释放资源但保留frame结构本身
        av_packet_free(&m_packet);
    }
    m_packet = new_packet;
    return *this;
}

AVPacketPtr& AVPacketPtr::operator=(AVPacketPtr&& other)noexcept
{
    if (this == &other)
    {
        return *this;
    }
    /// @brief 当自身对象已被构建时先释放自身
    if (m_packet)
    {
        av_packet_free(&m_packet);
    }
    m_packet = other.m_packet;
    other.m_packet = nullptr;
    return *this;
}

AVPacketPtr::operator bool()const
{
    return m_packet != nullptr;
}

AVPacket* AVPacketPtr::release()noexcept
{
    AVPacket* packet = m_packet;
    m_packet = nullptr;
    return packet;
}

AVPacket* AVPacketPtr::get()noexcept
{
    return m_packet;
}

const AVPacket* AVPacketPtr::get() const noexcept
{
    return m_packet;
}

AVPacket* AVPacketPtr::operator->()noexcept
{
    return m_packet;
}

AVPacket& AVPacketPtr::operator*()
{
    if (m_packet)
    {
        return *m_packet;
    }
    else
    {
        throw std::runtime_error("AVPacketPtr::operator*() m_packet is nullptr");
    }
}

FFmpegStatusDetail AVPacketPtr::ref(const AVPacketPtr& other)
{
    if (!other)
    {
        return FFmpegStatusDetail(DaneJoe::StatusLevel::Error);
    }

    if (m_packet)
    {
        unref();
    }
    else
    {
        ensure_allocated();
    }
    return av_packet_ref(m_packet, other.get());
}

void AVPacketPtr::unref()noexcept
{
    av_packet_unref(m_packet);
}
