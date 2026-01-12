/**
 * @file sdl_frame_renderer.hpp
 * @brief SDL帧渲染器
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <string>
#include <memory>
#include <atomic>

#include <SDL2/SDL.h>

#include "renderer/i_frame_renderer.hpp"

/**
 * @class SDLVideoSystem
 * @brief SDL视频子系统初始化器
 */
class SDLVideoSystem
{
public:
    /**
     * @brief 构造函数
     */
    SDLVideoSystem();
    /**
     * @brief 析构函数
     */
    ~SDLVideoSystem();
private:
    /// @brief SDL初始化计数
    static std::atomic<int> m_init_times;
};

/**
 * @struct SDL_window_deleter
 * @brief SDL_window的删除器
 */
struct SDL_window_deleter
{
    /**
     * @brief 删除SDL_Window
     * @param window 窗口
     */
    void operator()(SDL_Window* window)const;
};

/**
 * @struct SDL_renderer_deleter
 * @brief SDL_renderer的删除器
 */
struct SDL_renderer_deleter
{
    /**
     * @brief 删除SDL_Renderer
     * @param renderer 渲染器
     */
    void operator()(SDL_Renderer* renderer)const;
};

/**
 * @struct SDL_texture_deleter
 * @brief SDL_texture的删除器
 */
struct SDL_texture_deleter
{
    /**
     * @brief 删除SDL_Texture
     * @param texture 纹理
     */
    void operator()(SDL_Texture* texture)const;
};

/// @brief SDL_Window智能指针别名
using SDL_window_ptr = std::unique_ptr<SDL_Window, SDL_window_deleter>;
/// @brief SDL_Renderer智能指针别名
using SDL_renderer_ptr = std::unique_ptr<SDL_Renderer, SDL_renderer_deleter>;
/// @brief SDL_Texture智能指针别名
using SDL_texture_ptr = std::unique_ptr<SDL_Texture, SDL_texture_deleter>;

/**
 * @class SDLFrameRenderer
 * @brief SDL渲染器
 * @note 使用SDL渲染图像
 */
class SDLFrameRenderer : public IFrameRenderer
{
public:
    /**
     * @brief 构造函数
     * @param window_name 窗口名
     * @param window_size 窗口大小
     * @param window 窗口指针
     */
    SDLFrameRenderer(std::string window_name, DaneJoe::Size<int> window_size, void* window);
    /**
     * @brief 构造函数
     */
    SDLFrameRenderer();
    /**
     * @brief 析构函数
     */
    ~SDLFrameRenderer()override;
    /**
     * @brief 初始化
     * @return true 表示初始化成功，false 表示初始化失败
     */
    bool init() override;
    /**
     * @brief 绘制帧
     * @param frame 帧
     * @return true 表示绘制成功，false 表示绘制失败
     */
    bool draw(std::shared_ptr<Frame> frame) override;
    /**
     * @brief 绘制帧
     * @param frame ffmpeg帧
     * @return true 表示绘制成功，false 表示绘制失败
     */
    bool draw(AVFramePtr frame)override;
    /**
     * @brief 绘制YUV帧
     * @param y y平面数据
     * @param y_pitch y平面行字节数
     * @param u u平面数据
     * @param u_pitch u平面行字节数
     * @param v v平面数据
     * @param v_pitch v平面行字节数
     * @param width 宽度
     * @param height 高度
     * @return true 表示绘制成功，false 表示绘制失败
     */
    bool draw(
        uint8_t* y,
        int y_pitch,
        uint8_t* u,
        int u_pitch,
        uint8_t* v,
        int v_pitch,
        int width,
        int height);
    /**
     * @brief 设置窗口
     * @param window_name 窗口名称
     * @param window_size 窗口大小
     * @param window 窗口
     * @return true 表示设置成功，false 表示设置失败
     */
    bool set_window(std::string window_name, DaneJoe::Size<int> window_size, void* window)override;
    /**
     * @brief 是否退出
     * @return true 表示已退出，false 表示未退出
     */
    bool is_exit()override;
    /**
     * @brief 更新窗口大小
     * @param window_size 窗口大小
     * @return true 表示更新成功，false 表示更新失败
     */
    bool update_window_size(DaneJoe::Size<int> window_size)override;
    /**
     * @brief 设置帧格式
     * @param fmt 帧格式
     */
    void set_fmt(FrameFmt fmt) override;
private:
    /**
     * @brief 帧格式转换
     * @param fmt 帧格式
     * @return SDL_PixelFormatEnum
     */
    SDL_PixelFormatEnum fmt_convert(FrameFmt fmt);
    /**
     * @brief 更新SDL纹理
     * @param frame 帧
     * @return true 表示更新成功，false 表示更新失败
     */
    bool update_texture(std::shared_ptr<Frame> frame);
    /**
     * @brief 更新SDL纹理
     * @param frame ffmpeg帧
     * @return true 表示更新成功，false 表示更新失败
     */
    bool update_texture(AVFramePtr frame);
private:
    /// @brief 默认窗口尺寸
    const DaneJoe::Size<int> m_default_size = { 640, 480 };
private:
    /// @brief SDL视频系统
    SDLVideoSystem m_video_system;
    /// @brief 帧格式设置互斥锁
    std::mutex m_set_pixel_fmt_mutex;
    /// @brief SDL窗口
    /// @details 此处必须初始化为nullptr
    SDL_window_ptr m_window = nullptr;
    /// @brief SDL渲染器
    SDL_renderer_ptr m_renderer = nullptr;
    /// @brief SDL纹理
    /// @todo 处理一下解析数据与当前纹理不匹配的情况
    SDL_texture_ptr m_texture = nullptr;
    /// @brief SDL像素格式
    SDL_PixelFormatEnum m_pixel_format = SDL_PIXELFORMAT_UNKNOWN;
    /// @brief SDL初始化锁
    std::mutex m_sdl_init_mutex;
    /// @brief SDL窗口锁
    std::mutex m_set_window_mutex;
    /// @brief 纹理尺寸
    DaneJoe::Size<int> m_texture_size = { 0,0 };
};