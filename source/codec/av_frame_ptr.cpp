#include "codec/av_frame_ptr.hpp"

AVFramePtr::AVFramePtr(int width, int height, AVPixelFormat format, int align)
{
    init(width, height, format, align);
}

AVFramePtr::AVFramePtr() noexcept
{
    m_frame = nullptr;
}
AVError AVFramePtr::ensure_allocated() noexcept
{
    if (m_frame == nullptr)
    {
        m_frame = av_frame_alloc();
        if (!m_frame)
        {
            m_error = AVError(AVERROR(ENOMEM));
            return m_error;
        }
    }
    m_error = AVError(0);
    return m_error;
}

AVError AVFramePtr::init(int width, int height, AVPixelFormat format, int align)
{
    // 分配结构体
    AVError alloc_err = ensure_allocated();
    if (alloc_err.failed())
    {
        return m_error;
    }
    av_frame_unref(m_frame);

    m_frame->width = width;
    m_frame->height = height;
    m_frame->format = format;
    m_error = av_frame_get_buffer(m_frame, align);
    if (m_error.failed())
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
        break;
    case AV_PIX_FMT_YUV422P:
    default:
        break;
    }
    return m_error;
}

AVFramePtr::AVFramePtr(const AVFramePtr& frame)
{
    if (!frame.get())
    {
        m_frame = nullptr;
        m_error = frame.m_error;
        return;
    }
    m_frame = av_frame_alloc();
    if (!m_frame)
    {
        m_error = AVError(AVERROR(ENOMEM));
        return;
    }
    m_error = av_frame_ref(m_frame, frame.get());
    if (m_error)
    {
        av_frame_free(&m_frame);
    }
}

// 移动构造：转移指针与错误状态，源清空
AVFramePtr::AVFramePtr(AVFramePtr&& frame) noexcept : m_frame(frame.get())
{
    m_error = frame.m_error;
    frame.m_frame = nullptr;
}

// 拷贝赋值：强异常安全，先构造临时再替换当前对象
AVFramePtr& AVFramePtr::operator=(const AVFramePtr& frame)
{
    if (this == &frame)
    {
        return *this;
    }
    AVFrame* new_frame = nullptr;
    AVError new_error;
    if (frame.get())
    {
        new_frame = av_frame_alloc();
        if (!new_frame)
        {
            return *this; // 分配失败，不改变当前对象
        }
        new_error = av_frame_ref(new_frame, frame.get());
        if (new_error)
        {
            av_frame_free(&new_frame);
            return *this; // 引用失败，不改变当前对象
        }
    }
    // 生效替换（先释放旧的再接管新的）
    av_frame_free(&m_frame);
    m_frame = new_frame;
    m_error = frame.m_error;
    return *this;
}

// 移动赋值：释放当前资源并接管源（noexcept）
AVFramePtr& AVFramePtr::operator=(AVFramePtr&& frame) noexcept
{
    if (this == &frame)
    {
        return *this;
    }
    if (m_frame)
    {
        av_frame_free(&m_frame);
    }
    m_frame = frame.get();
    m_error = frame.m_error;
    frame.m_frame = nullptr;
    return *this;
}

AVFrame* AVFramePtr::get() noexcept
{
    return m_frame;
}

const AVFrame* AVFramePtr::get() const noexcept
{
    return m_frame;
}

AVFrame* AVFramePtr::operator->() noexcept
{
    return m_frame;
}

const AVFrame* AVFramePtr::operator->() const noexcept
{
    return m_frame;
}

AVFrame& AVFramePtr::operator*() noexcept
{
    return *m_frame;
}

const AVFrame& AVFramePtr::operator*() const noexcept
{
    return *m_frame;
}

AVFramePtr::~AVFramePtr() noexcept
{
    if (m_frame)
    {
        av_frame_free(&m_frame);
    }
}

AVFramePtr::operator bool() const noexcept
{
    return m_frame != nullptr;
}

AVError AVFramePtr::get_error() const noexcept
{
    return m_error;
}

std::size_t AVFramePtr::use_count() const noexcept
{
    if (m_frame && m_frame->buf[0])
    {
        return static_cast<std::size_t>(av_buffer_get_ref_count(m_frame->buf[0]));
    }
    return 0;
}

void AVFramePtr::reset() noexcept
{
    if (m_frame)
    {
        av_frame_free(&m_frame);
    }
}

void AVFramePtr::unref() noexcept
{
    if (m_frame)
    {
        av_frame_unref(m_frame);
    }
}

AVFrame* AVFramePtr::release() noexcept
{
    AVFrame* ptr = m_frame;
    m_frame = nullptr;
    return ptr;
}

void AVFramePtr::swap(AVFramePtr& other) noexcept
{
    std::swap(m_frame, other.m_frame);
    std::swap(m_error, other.m_error);
}