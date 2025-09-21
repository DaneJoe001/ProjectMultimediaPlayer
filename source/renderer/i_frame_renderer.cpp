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