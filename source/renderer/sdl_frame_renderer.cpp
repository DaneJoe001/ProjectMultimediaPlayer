#include <exception>
#include <stdexcept>

#include <iostream>

#include "renderer/sdl_frame_renderer.hpp"


std::atomic<int> SDLVideoSystem::m_init_times = 0;
SDLVideoSystem::SDLVideoSystem()
{
    /// @brief 获取操作之前的原子计数
    int prev = m_init_times.fetch_add(1, std::memory_order_acq_rel);
    if (prev == 0)
    {
        int check_video_init = SDL_InitSubSystem(SDL_INIT_VIDEO);
        if (check_video_init < 0)
        {
            DANEJOE_LOG_ERROR("default", "SDLVideoSystem", "SDL_Init failed:{}", SDL_GetError());
            /// @brief 操作失败时减少原子计数
            m_init_times.fetch_sub(1, std::memory_order_acq_rel);
            throw std::runtime_error("SDL_Init failed!");
        }
    }
}

SDLVideoSystem::~SDLVideoSystem()
{
    int prev = m_init_times.fetch_sub(1, std::memory_order_acq_rel);
    if (prev == 1)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
}

void SDL_window_deleter::operator()(SDL_Window* window)const
{
    if (window)
    {
        SDL_DestroyWindow(window);
    }
}

void SDL_renderer_deleter::operator()(SDL_Renderer* renderer)const
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
}

void SDL_texture_deleter::operator()(SDL_Texture* texture)const
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
}

SDLFrameRenderer::SDLFrameRenderer(){}

SDLFrameRenderer::SDLFrameRenderer(std::string window_name, DaneJoe::Size<int> window_size, void* window)
{
    /// @brief 确保窗口大小合法
    m_window_size = window_size.quadrant() == DaneJoe::Size<int>::Quadrant::FIRST ? window_size : m_default_size;

    /// @brief 设置缩放质量
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    /// @brief 创建窗口
    set_window(window_name, m_window_size, window);
    /// @brief 渲染目标区域
    set_dest_area({ 0,0 }, m_window_size);
    /// @brief 初始化原始帧大小
    set_raw_frame_size(m_window_size);
    init(FrameFmt::ARGB8888);
}

SDLFrameRenderer::~SDLFrameRenderer() {}

bool SDLFrameRenderer::init(FrameFmt fmt)
{
    set_fmt(fmt);
    /// @brief 初始化锁
    std::lock_guard<std::mutex> lock(m_sdl_init_mutex);
    /// @brief 创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        /// @brief 使用软解码
        renderer = SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_SOFTWARE);
    }
    if (!check_renderer(renderer))
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "SDL_CreateRenderer failed:{}", SDL_GetError());
        return false;
    }
    m_renderer.reset(renderer);
    /// @brief 创建纹理
    /// @note 初始化时以窗口宽高创建纹理
    SDL_Texture* texture = SDL_CreateTexture(m_renderer.get(), m_pixel_format, SDL_TEXTUREACCESS_STREAMING, m_window_size.x, m_window_size.y);
    if (!check_texture(texture))
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "SDL_CreateTexture failed:{}", SDL_GetError());
        return false;
    }
    m_texture.reset(texture);
    return true;
}

bool SDLFrameRenderer::is_exit()
{
    SDL_Event event;
    SDL_WaitEventTimeout(&event, 2);
    if (event.type == SDL_QUIT)
    {
        DANEJOE_LOG_TRACE("default", "SDLFrameRenderer", "SDL_Event:SDL_QUIT");
        return true;
    }
    return false;
}

void SDLFrameRenderer::set_fmt(FrameFmt fmt)
{
    std::lock_guard<std::mutex> lock(m_set_pixel_fmt_mutex);
    m_pixel_format = fmt_convert(fmt);
}

SDL_PixelFormatEnum SDLFrameRenderer::fmt_convert(FrameFmt fmt)
{
    switch (fmt)
    {
    case FrameFmt::RGB888:
        return SDL_PIXELFORMAT_RGB888;
    case FrameFmt::RGBA8888:
        return SDL_PIXELFORMAT_RGBA8888;
    case FrameFmt::YUV420P:
        return SDL_PIXELFORMAT_IYUV;
    case FrameFmt::ARGB8888:
        return SDL_PIXELFORMAT_ARGB8888;
    default:
        return SDL_PIXELFORMAT_UNKNOWN;
    }
}

bool SDLFrameRenderer::draw(
    uint8_t* y,
    int y_pitch,
    uint8_t* u,
    int u_pitch,
    uint8_t* v,
    int v_pitch,
    int width,
    int height)
{
    if (!y || !u || !v || width <= 0 || height <= 0)
    {
        return false;
    }
    auto ret = SDL_UpdateYUVTexture(m_texture.get(),
        nullptr,
        y,
        y_pitch,
        u,
        u_pitch,
        v,
        v_pitch);
    if (ret < 0)
    {
        DANEJOE_LOG_ERROR("default", "Texture", "UpdateYUVTexture failed: {}", SDL_GetError());
        return false;
    }
    /// @brief 清理渲染器
    SDL_RenderClear(m_renderer.get());
    /// @brief 复制纹理到渲染器
    if (m_dest_pos.quadrant() != DaneJoe::Pos<int>::Quadrant::FIRST)
    {
        SDL_RenderCopy(m_renderer.get(), m_texture.get(), nullptr, nullptr);
    }
    else
    {
        SDL_Rect dest_area = { m_dest_pos.x, m_dest_pos.y, m_dest_size.x, m_dest_size.y };
        SDL_RenderCopy(m_renderer.get(), m_texture.get(), nullptr, &dest_area);
    }

    /// @brief 显示渲染器
    SDL_RenderPresent(m_renderer.get());
    return true;
}

bool SDLFrameRenderer::draw(AVFramePtr frame)
{
    if (!frame)
    {
        return false;
    }
    switch (frame->format)
    {
    case AV_PIX_FMT_YUV420P:
        return draw(frame->data[0],
            frame->linesize[0],
            frame->data[1],
            frame->linesize[1],
            frame->data[2],
            frame->linesize[2],
            frame->width,
            frame->height);
    default:
        return false;
    }
    return true;

}

bool SDLFrameRenderer::draw(std::shared_ptr<Frame> frame)
{
    if (!check_frame(frame))
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "check_frame failed");
        return false;
    }
    std::scoped_lock lock(m_draw_mutex, m_sdl_init_mutex, m_set_pixel_fmt_mutex, m_set_window_mutex);
    /// @brief 当帧格式或者窗口大小改变时，重新创建Texture

    if (!check_window(m_window.get()) || !check_renderer(m_renderer.get()) || !reset_texture(frame) || !check_texture(m_texture.get()))
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "Environment check failed");
        return false;
    }
    /// @brief 更新纹理
    // SDL_UpdateTexture(m_texture.get(), nullptr, frame->data.data(), frame->pitch);
    int check_update_texture = 0;
    if (frame->fmt == FrameFmt::YUV420P)
    {
        int width = frame->size.x;
        int height = frame->size.y;
        const uint8_t* y_plane = frame->data.data();
        const uint8_t* u_plane = y_plane + width * height;
        const uint8_t* v_plane = u_plane + (width / 2) * (height / 2);
        int y_pitch = width;
        int u_pitch = width / 2;
        int v_pitch = width / 2;
        check_update_texture = SDL_UpdateYUVTexture(
            m_texture.get(), nullptr,
            y_plane, y_pitch,
            u_plane, u_pitch,
            v_plane, v_pitch
        );
    }
    else
    {
        check_update_texture = SDL_UpdateTexture(m_texture.get(), nullptr, frame->data.data(), frame->pitch);
    }
    if (check_update_texture != 0)
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "SDL_UpdateTexture error:{}", SDL_GetError());
        return false;
    }
    /// @brief 清理渲染器
    SDL_RenderClear(m_renderer.get());
    /// @brief 复制纹理到渲染器
    if (m_dest_pos.quadrant() != DaneJoe::Pos<int>::Quadrant::FIRST)
    {
        SDL_RenderCopy(m_renderer.get(), m_texture.get(), nullptr, nullptr);
    }
    else
    {
        SDL_Rect dest_area = { m_dest_pos.x, m_dest_pos.y, m_dest_size.x, m_dest_size.y };
        SDL_RenderCopy(m_renderer.get(), m_texture.get(), nullptr, &dest_area);
    }

    /// @brief 显示渲染器
    SDL_RenderPresent(m_renderer.get());
    return true;
}

bool SDLFrameRenderer::set_window(std::string window_name, DaneJoe::Size<int> window_size, void* window)
{
    SDL_Window* new_window = nullptr;
    m_window_size = window_size;
    m_window_name = window_name;
    /// @brief 当传递的窗口指针为空时创建新的窗口
    std::lock_guard<std::mutex> lock(m_set_window_mutex);
    if (window == nullptr)
    {
        new_window = SDL_CreateWindow(m_window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_window_size.x, m_window_size.y, SDL_WINDOW_SHOWN);
    }
    else
    {
        new_window = SDL_CreateWindowFrom(window);
    }
    if (!check_window(new_window))
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "SDL_CreateWindow failed:{}", SDL_GetError());
        return false;
    }
    m_window.reset(new_window);
    return true;
}

bool SDLFrameRenderer::update_window_size(DaneJoe::Size<int> window_size)
{
    if (!check_window(m_window.get()))
    {
        m_error_code.store(2);
        return false;
    }
    std::lock_guard<std::mutex> lock(m_set_window_mutex);
    SDL_Rect viewport = { 0, 0, window_size.x, window_size.y };
    SDL_RenderSetViewport(m_renderer.get(), &viewport);

    m_window_size = window_size;
    SDL_SetWindowSize(m_window.get(), window_size.x, window_size.y);
    return true;
}

std::string SDLFrameRenderer::error_msg(int error_code)
{
    if (error_code < BASE_ERROR_CODE_QUANTITY)
    {
        return IFrameRenderer::error_msg(error_code);
    }
    int temp_code = error_code - BASE_ERROR_CODE_QUANTITY;
    switch (temp_code)
    {
    case 0:
        return std::string("INVALID_FRAME");
    case 1:
        return std::string("INVALID_WINDOW");
    default:
        return SDL_GetError();
    }
}

bool SDLFrameRenderer::check_window(const SDL_Window* window)
{
    if (window)
    {
        return true;
    }
    DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "window is not set");
    return false;
}
bool SDLFrameRenderer::check_renderer(const SDL_Renderer* renderer)
{
    if (renderer)
    {
        return true;
    }
    DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "renderer is not set");
    return false;

}
bool SDLFrameRenderer::check_texture(const SDL_Texture* texture)
{
    if (texture)
    {
        return true;
    }
    DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "texture is not set");
    return false;
}
bool SDLFrameRenderer::check_frame(std::shared_ptr<Frame> frame)
{
    if (!frame)
    {
        return false;
    }
    if (!frame->is_valid)
    {
        m_error_code.store(1);
        DANEJOE_LOG_TRACE("default", "SDLFrameRenderer", "Invalid frame");
        /// @todo 完善错误处理
        return false;
    }
    if (frame->size.quadrant() != DaneJoe::Size<int>::Quadrant::FIRST)
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameRenderer", "frame size is not first quadrant");
        return false;
    }
    return true;
}

bool SDLFrameRenderer::reset_texture(std::shared_ptr<Frame> frame)
{
    if (!frame)
    {
        return false;
    }

    if (!m_texture || fmt_convert(frame->fmt) != m_pixel_format ||
        frame->size != m_raw_frame_size)
    {
        m_raw_frame_size = frame->size;
        m_pixel_format = fmt_convert(frame->fmt);
        set_raw_frame_size(frame->size);

        SDL_Texture* texture = SDL_CreateTexture(m_renderer.get(), fmt_convert(frame->fmt), SDL_TEXTUREACCESS_STREAMING, frame->size.x, frame->size.y);

        if (check_texture(texture))
        {
            m_texture.reset(texture);
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}