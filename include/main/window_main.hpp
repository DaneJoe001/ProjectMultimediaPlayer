#pragma once

#include <memory>
#include <cstdint>
#include <fstream>
#include <chrono>

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include <SDL2/SDL.h>

#include "renderer/i_frame_renderer.hpp"
#include "codec/av_frame_ptr.hpp"

/// @brief 前向声明
class IFrameRenderer;

/**
 * @class WindowMain
 * @brief 主窗口
 * @note 主窗口
  */
class WindowMain :public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit WindowMain(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     * @note 释放资源
     */
    ~WindowMain();
    void sleep(std::chrono::milliseconds ms);
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
private:
    AVFramePtr m_frame_ptr;
    /// @brief 帧信息
    std::shared_ptr<IFrameRenderer::Frame> m_frame;
    /// @brief 渲染器
    std::shared_ptr<IFrameRenderer> m_renderer_1 = nullptr;
    /// @brief SDL标签
    QLabel* m_label_sdl_1;
    /// @brief 视频帧率
    uint8_t m_video_rate = 25;
    /// @brief 视频文件
    std::ifstream m_video_file;
    /// @brief 视频帧大小
    DaneJoe::Size<int> m_video_frame_size;
    /// @brief 窗口标签容器
    QWidget* m_labels_container;
    /// @brief 窗口标签布局
    QHBoxLayout* m_labels_layout;
};
