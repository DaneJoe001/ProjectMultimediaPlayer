#include "codec/av_frame_ptr.hpp"

AVFramePtr::AVFramePtr(int width, int height, AVPixelFormat format, int align)
{
    init(width, height, format, align);
}

AVFramePtr::AVFramePtr()
{
    /// @brief 创建AVFrame结构
    /// @note 避免get()返回nullptr
    m_frame = av_frame_alloc();
}

AVError AVFramePtr::init(int width, int height, AVPixelFormat format, int align)
{
    /// @todo 考虑当值不匹配时释放重构
    if (m_frame)
    {
        /// @brief 释放AVFrame存储的内容
        av_frame_unref(m_frame);
    }
    else
    {
        /// @brief 分配一个AVFrame结构
        m_frame = av_frame_alloc();
    }
    if (m_frame)
    {
        /// @brief 分配AVFrame结构所使用的数据
        m_frame->width = width;
        m_frame->height = height;
        m_frame->format = format;
        /// @note align为0时，使用默认对齐方式
        m_error = av_frame_get_buffer(m_frame, align);
        /// @brief 错误处理
        if (!m_error)
        {
            return m_error;
        }
        switch (m_frame->format)
        {
        case AV_PIX_FMT_YUV420P:
            m_frame->linesize[0] = width;
            m_frame->linesize[1] = width / 2;
            m_frame->linesize[2] = width / 2;
            break;
        case AV_PIX_FMT_YUVJ420P:
            // m_fmt = FrameFmt::YUVJ420P;
            break;
        case AV_PIX_FMT_YUV422P:
            // m_fmt = FrameFmt::YUV422P;
        default:break;
        }
    }
    else
    {
        m_error = AVError(AVERROR(ENOMEM));
    }
    return m_error;
}
AVFramePtr::AVFramePtr(const AVFramePtr& frame)
{
    /// @brief 分配新的AVFrame结构
    m_frame = av_frame_alloc();
    /// @brief 判断源是否有效
    if (frame.get() && m_frame)
    {
        /// @brief 分配成功的情况下引用计数加1
        m_error = av_frame_ref(m_frame, frame.get());
    }
}
AVFramePtr::AVFramePtr(AVFramePtr&& frame) :m_frame(frame.get())
{
    /// @brief 清理源
    m_error = frame.m_error;
    frame.m_frame = nullptr;
}
AVFramePtr& AVFramePtr::operator=(const AVFramePtr& frame)
{
    /// @brief 判断源是否为自身
    if (this == &frame)
    {
        return *this;
    }
    /// @brief 当自身对象已存在时，先释放
    if (m_frame)
    {
        /// @brief 当自身结构存在时通过unref释放资源但保留frame结构本身
        av_frame_unref(m_frame);
    }
    else
    {
        /// @brief 分配新的AVFrame结构
        m_frame = av_frame_alloc();
    }
    /// @brief 判断源是否有效
    if (frame.get() && m_frame)
    {
        /// @brief 分配成功的情况下引用计数加1
        m_error = av_frame_ref(m_frame, frame.get());
    }
    return *this;
}

AVFramePtr& AVFramePtr::operator=(AVFramePtr&& frame)
{
    if (this == &frame)
    {
        return *this;
    }
    /// @brief 当自身对象已被构建时先释放自身
    if (m_frame)
    {
        av_frame_free(&m_frame);
    }
    m_frame = frame.get();
    frame.m_frame = nullptr;
    return *this;
}
AVFrame* AVFramePtr::get()const
{
    return m_frame;
}
AVFrame* AVFramePtr::operator->()const
{
    return m_frame;
}
AVFramePtr::~AVFramePtr()
{
    if (m_frame)
    {
        av_frame_free(&m_frame);
    }
}

AVFramePtr::operator bool()const
{
    return m_frame != nullptr;
}

AVError AVFramePtr::get_error()const
{
    return m_error;
}

std::size_t AVFramePtr::use_count()const
{
    if (m_frame)
    {
        return av_buffer_get_ref_count(m_frame->buf[0]);
    }
    return 0;
}