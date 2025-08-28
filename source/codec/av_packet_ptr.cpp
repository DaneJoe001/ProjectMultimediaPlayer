#include "codec/av_packet_ptr.hpp"

AVPacketPtr::AVPacketPtr()
{
    m_packet = av_packet_alloc();
}

AVPacketPtr::~AVPacketPtr()
{
    if (m_packet)
    {
        av_packet_free(&m_packet);
    }
}

AVPacketPtr::AVPacketPtr(const AVPacketPtr& other)
{
    m_packet = av_packet_alloc();
    av_packet_ref(m_packet, other.m_packet);
}

AVPacketPtr::AVPacketPtr(AVPacketPtr&& other)
{
    m_packet = other.m_packet;
    other.m_packet = nullptr;
}

AVPacketPtr& AVPacketPtr::operator=(const AVPacketPtr& other)
{
    if (this == &other)
    {
        return *this;
    }
    if (m_packet)
    {
        av_packet_unref(m_packet);
    }
    else
    {
        m_packet = av_packet_alloc();
    }
    av_packet_ref(m_packet, other.m_packet);
}

AVPacketPtr& AVPacketPtr::operator=(AVPacketPtr&& other)
{
    if (this == &other)
    {
        return *this;
    }
    m_packet = other.m_packet;
    other.m_packet = nullptr;
}

AVPacketPtr::operator bool()const
{
    return m_packet != nullptr;
}

AVPacket* AVPacketPtr::get()
{
    return m_packet;
}

AVPacket* AVPacketPtr::operator->()
{
    return m_packet;
}
