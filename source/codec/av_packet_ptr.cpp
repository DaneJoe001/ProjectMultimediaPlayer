#include <exception>
#include <stdexcept>

#include "codec/av_packet_ptr.hpp"

AVPacketPtr::AVPacketPtr()noexcept
{
    m_packet = nullptr;
}

AVError AVPacketPtr::ensure_allocated() noexcept
{
    if (!m_packet)
    {
        m_packet = av_packet_alloc();
    }
    if (!m_packet)
    {
        m_error = AVERROR(ENOMEM);
        return m_error;
    }
    m_error = 0;
    return m_error;
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
    m_error = ensure_allocated();
    if (m_error.failed())
    {
        m_packet = nullptr;
        return;
    }
    m_error = ref(other);
    if (m_error.failed())
    {
        av_packet_free(&m_packet);
        m_packet = nullptr;
        return;
    }
}

void AVPacketPtr::swap(AVPacketPtr& other)noexcept
{
    std::swap(m_packet, other.m_packet);
    std::swap(m_error, other.m_error);
}

AVPacketPtr::AVPacketPtr(AVPacketPtr&& other)noexcept :m_error(other.m_error), m_packet(other.m_packet)
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
    AVError new_error;
    AVPacket* new_packet = nullptr;
    /// @brief 当源对象不为空时进行分配
    if (other.m_packet)
    {
        new_packet = av_packet_alloc();
        if (!new_packet)
        {
            new_error = AVERROR(ENOMEM);
        }
        else
        {
            AVError error = av_packet_ref(new_packet, other.m_packet);
            if (error.failed())
            {
                av_packet_free(&new_packet);
                new_packet = nullptr;
                new_error = error;
            }
            else
            {
                new_error = AVError();
            }
        }
    }
    else
    {
        new_error = AVError();
        new_packet = nullptr;
    }
    if (m_packet)
    {
        /// @brief 当自身结构存在时通过unref释放资源但保留frame结构本身
        av_packet_free(&m_packet);
    }
    m_packet = new_packet;
    m_error = new_error;
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
    m_error = other.m_error;
    other.m_error = AVError();
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
        throw std::runtime_error(m_error.message());
    }
}

AVError AVPacketPtr::get_error() const noexcept
{
    return m_error;
}

AVError AVPacketPtr::ref(AVPacketPtr other)
{
    if (other.get())
    {
        if (m_packet)
        {
            unref();
        }
        m_error = av_packet_ref(m_packet, other.get());
    }
    return m_error;
}

void AVPacketPtr::unref()noexcept
{
    av_packet_unref(m_packet);
}
