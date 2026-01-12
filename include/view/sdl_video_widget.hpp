/**
 * @file sdl_video_widget.hpp
 * @brief SDL视频组件
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <memory>
#include <cstdint>
#include <chrono>

#include <QWidget>

#include <SDL2/SDL.h>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "renderer/i_frame_renderer.hpp"
#include "codec/av_frame_ptr.hpp"



/// @brief 前向声明
class IFrameRenderer;
class QVBoxLayout;
class QLabel;

/**
 * @class SDLVideoWidget
 * @brief SDL视频组件
 */
class SDLVideoWidget :public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit SDLVideoWidget(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     * @note 释放资源
     */
    ~SDLVideoWidget();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 关闭
     */
    void close();
    /**
     * @brief 休眠
     * @param ms 休眠时间
     */
    void sleep(std::chrono::milliseconds ms);
    /**
     * @brief 获取帧队列
     * @return 帧队列弱引用
     */
    std::weak_ptr<DaneJoe::MpmcBoundedQueue<AVFramePtr>> get_frame_queue();
private:
    /**
     * @brief 定时器事件
     * @param event 事件
     */
    void timerEvent(QTimerEvent* event)override;
    /**
     * @brief 窗口大小改变事件
     * @param event 事件
     */
    void resizeEvent(QResizeEvent* event)override;
    /**
     * @brief 窗口显示事件
     * @param event 事件
     */
    void showEvent(QShowEvent* event)override;
    /**
     * @brief 窗口关闭事件
     * @param event 事件
     */
    void closeEvent(QCloseEvent* event)override;
    /**
     * @brief 初始化渲染器
     */
    void init_renderer();
private:
    /// @brief 是否初始化
    bool m_is_init = false;
    /// @brief 内部定时器
    int m_timer_id = -1;
    /// @brief 渲染器
    std::shared_ptr<IFrameRenderer> m_renderer = nullptr;
    /// @brief SDL标签
    QLabel* m_sdl_label;
    /// @brief 视频帧率
    uint8_t m_video_rate = 25;
    /// @brief 窗口布局
    QVBoxLayout* m_main_layout;
    /// @brief 帧队列
    std::shared_ptr<DaneJoe::MpmcBoundedQueue<AVFramePtr>> m_frame_queue;
};
