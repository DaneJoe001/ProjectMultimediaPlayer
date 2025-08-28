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
    if (m_packet && other.m_packet)
    {
        av_packet_ref(m_packet, other.m_packet);
    }
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
        /// @brief 当自身结构存在时通过unref释放资源但保留frame结构本身
        av_packet_unref(m_packet);
    }
    else
    {
        /// @brief 分配新的AVPacket结构
        m_packet = av_packet_alloc();
    }
    if (other.m_packet && m_packet)
    {
        av_packet_ref(m_packet, other.m_packet);
    }
    return *this;
}

AVPacketPtr& AVPacketPtr::operator=(AVPacketPtr&& other)
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

AVPacket* AVPacketPtr::get()
{
    return m_packet;
}

AVPacket* AVPacketPtr::operator->()
{
    return m_packet;
}
