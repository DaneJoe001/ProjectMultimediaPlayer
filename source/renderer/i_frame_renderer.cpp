#include "log/manage_logger.hpp"

#include "renderer/i_frame_renderer.hpp"

IFrameRenderer::IFrameRenderer() {}

IFrameRenderer::~IFrameRenderer() {}

void IFrameRenderer::Frame::init_info()
{
    switch (fmt)
    {
    case FrameFmt::YUV420P:
        pitch = size.x;break;
    case FrameFmt::RGBA8888:
        pitch = size.x * 4;
        pixel_size = 4;
        break;
    case FrameFmt::RGB888:
        pitch = size.x * 3;
        pixel_size = 3;
        break;
    case FrameFmt::ARGB8888:
        pitch = size.x * 4;
        pixel_size = 4;
        break;
    default:
        break;
    }
}

bool IFrameRenderer::set_dest_area(const DaneJoe::Pos<int>& pos, const DaneJoe::Size<int>& size)
{
    set_dest_pos(pos);
    return set_dest_size(size);
}

void IFrameRenderer::set_dest_pos(const DaneJoe::Pos<int>& pos)
{
    std::lock_guard<std::mutex> lock(m_dest_pos_mutex);
    m_dest_pos = pos;
}

bool IFrameRenderer::set_dest_size(const DaneJoe::Size<int>& size)
{
    std::lock_guard<std::mutex> lock(m_dest_size_mutex);
    // 当前不考虑size的合法性，将不合法参数当默认配置处理
    m_dest_size = size;
    return true;
}

DaneJoe::Size<int> IFrameRenderer::get_dest_size()const
{
    return m_dest_size;
}

DaneJoe::Pos<int> IFrameRenderer::get_dest_pos()const
{
    return m_dest_pos;
}

bool IFrameRenderer::set_raw_frame_size(const DaneJoe::Size<int>& size)
{
    std::lock_guard<std::mutex> lock(m_raw_frame_size_mutex);
    if (size.quadrant() != DaneJoe::Size<int>::Quadrant::FIRST)
    {
        m_error_code.store(1);
        return false;
    }
    m_raw_frame_size = size;
    return true;
}

int IFrameRenderer::error_code()
{
    int temp = m_error_code.load();
    m_error_code.store(0);
    return temp;
}

std::string IFrameRenderer::error_msg(int error_code)
{
    switch (error_code)
    {
    case 0:
        return "NO_ERROR";
    case 1:
        return "INVALID_SIZE";
    default:
        return "UNKNOWN_ERROR";
    }
}

bool IFrameRenderer::set_window_size(const DaneJoe::Size<int>& size)
{
    std::lock_guard<std::mutex> lock(m_window_size_mutex);
    if (size.quadrant() != DaneJoe::Size<int>::Quadrant::FIRST)
    {
        m_error_code.store(1);
        return false;
    }
    m_window_size = size;
    return true;
}