/**
 * @file sdl_audio_worker.hpp
 * @brief SDL音频worker
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <atomic>

#include <QObject>

#include "model/session_frame.hpp"
#include "audio/sdl_audio_renderer.hpp"
#include "service/time_service.hpp"

/**
 * @class SDLAudioWorker
 * @brief SDL音频worker
 */
class SDLAudioWorker : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param time_service 时间服务
     * @param parent Qt父对象
     */
    SDLAudioWorker(std::shared_ptr<TimeService> time_service, QObject* parent = nullptr);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 请求停止
     */
    void request_stop();
    /**
     * @brief 清理停止标记
     */
    void clear_stop();
public slots:
    /**
     * @brief 初始化槽
     */
    void on_init();
    /**
     * @brief 开始音频
     */
    void on_start_audio();
    /**
     * @brief 停止音频
     */
    void on_stop_audio();
    /**
     * @brief 响应帧就绪
     * @param frame 帧
     */
    void on_frame_ready(SessionFrame frame);
private:
    /// @brief 是否停止
    std::atomic<bool> m_is_stop = false;
    /// @brief SDL音频渲染器
    SDLAudioRenderer m_audio_renderer;
};