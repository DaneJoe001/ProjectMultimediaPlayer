extern "C"
{
#include <libavcodec/avcodec.h>

#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}
#include "codec/av_frame_ptr.hpp"

AVFramePtr::AVFramePtr(int width, int height, AVPixelFormat format, int align)
{
    FFmpegStatusDetail result = init(width, height, format, align);
    if (result.is_error())
    {
        if (m_frame)
        {
            av_frame_free(&m_frame);
            m_frame = nullptr;
        }
    }
}

AVFramePtr::AVFramePtr() noexcept
{
    m_frame = nullptr;
}

FFmpegStatusDetail AVFramePtr::ensure_allocated() noexcept
{
    if (m_frame == nullptr)
    {
        m_frame = av_frame_alloc();
        if (!m_frame)
        {
            return FFmpegStatusDetail(AVERROR(ENOMEM));
        }
    }
    return FFmpegStatusDetail(DaneJoe::StatusLevel::Ok);
}

FFmpegStatusDetail AVFramePtr::init(int width, int height, AVPixelFormat format, int align)
{
    // 分配结构体
    FFmpegStatusDetail alloc_status = ensure_allocated();
    if (alloc_status.is_error())
    {
        return alloc_status; // 分配失败
    }
    av_frame_unref(m_frame);

    m_frame->width = width;
    m_frame->height = height;
    m_frame->format = format;
    FFmpegStatusDetail get_buffer_status =
        FFmpegStatusDetail(av_frame_get_buffer(m_frame, align));
    if (get_buffer_status.is_error())
    {
        return get_buffer_status;
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
    return get_buffer_status;
}

AVFramePtr::AVFramePtr(const AVFramePtr& frame)
{
    if (!frame.get())
    {
        m_frame = nullptr;
        return;
    }
    m_frame = av_frame_alloc();
    if (!m_frame)
    {
        return;
    }
    FFmpegStatusDetail ref_status =
        av_frame_ref(m_frame, frame.get());
    if (ref_status.is_error())
    {
        // 引用失败，回退到空状态，保留错误码
        av_frame_free(&m_frame);
        m_frame = nullptr;  // 确保设置为 nullptr
    }
}

AVFramePtr::AVFramePtr(AVFramePtr&& frame) noexcept : m_frame(frame.get())
{
    frame.m_frame = nullptr;
}

AVFramePtr& AVFramePtr::operator=(const AVFramePtr& other)
{
    if (this == &other)
    {
        return *this;
    }
    AVFrame* new_frame = nullptr;

    if (other.get())
    {
        new_frame = av_frame_alloc();
        if (!new_frame)
        {
            return *this;
        }
        FFmpegStatusDetail ref_status =
            av_frame_ref(new_frame, other.get());
        if (ref_status.is_error())
        {
            av_frame_free(&new_frame);
            return *this;
        }
    }
    else
    {
        new_frame = nullptr;
    }
    av_frame_free(&m_frame);
    m_frame = new_frame;
    return *this;
}

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
}