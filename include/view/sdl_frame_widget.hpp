#pragma once

#include <memory>

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
 * @class WindowMain
 * @brief 主窗口
 * @note 主窗口
  */
class SDLFrameWidget :public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit SDLFrameWidget(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     * @note 释放资源
     */
    ~SDLFrameWidget();
    void init();
    void draw(AVFramePtr frame);
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent* event);
private:
    void init_renderer();
private:
    /// @brief 是否初始化
    bool m_is_init = false;
    /// @brief 渲染器
    std::shared_ptr<IFrameRenderer> m_renderer = nullptr;
    /// @brief SDL标签
    QLabel* m_sdl_label;
    /// @brief 窗口布局
    QVBoxLayout* m_main_layout;
};
