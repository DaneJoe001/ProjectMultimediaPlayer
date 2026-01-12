/**
 * @file media_controller.hpp
 * @brief 媒体控制器
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <queue>
#include <memory>
#include <optional>
#include <atomic>

#include <QObject>
#include <QTimer>
#include <QQueue>

#include "model/session_entity.hpp"
#include "model/session_frame.hpp"
#include "service/time_service.hpp"

/**
 * @struct SessionPending
 * @brief 会话暂存信息
 */
struct SessionPending
{
    /// @brief 会话实体
    SessionEntity session;
    /// @brief 视频帧队列
    std::queue<SessionFrame> video_frames;
    /// @brief 音频帧队列
    std::queue<SessionFrame> audio_frames;
};

/**
 * @class MediaController
 * @brief 媒体控制器
 */
class MediaController : public QObject
{
public:
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param time_service 时间服务
     * @param parent Qt父对象
     */
    MediaController(std::shared_ptr<TimeService> time_service, QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~MediaController() override;
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 停止音频
     */
    void stop_audio();
    /**
     * @brief 停止视频
     */
    void stop_video();

signals:
    /**
     * @brief 输出视频帧到渲染器
     * @param frame 帧数据
     */
    void renderer_video_frame(SessionFrame frame);
    /**
     * @brief 输出音频帧到渲染器
     * @param frame 帧数据
     */
    void renderer_audio_frame(SessionFrame frame);
    /**
     * @brief 通知解码暂停/恢复
     * @param is_paused 是否暂停
     */
    void paused_decode(bool is_paused);
public slots:
    /**
     * @brief 清空音视频缓冲
     */
    void on_clear_buffer();
    /**
     * @brief 会话已准备就绪
     * @param session 会话实体
     */
    void on_session_ready(SessionEntity session);
    /**
     * @brief 视频帧已准备就绪
     * @param frame 帧数据
     */
    void on_video_frame_ready(SessionFrame frame);
    /**
     * @brief 音频帧已准备就绪
     * @param frame 帧数据
     */
    void on_audio_frame_ready(SessionFrame frame);
    /**
     * @brief 视频帧定时器tick
     */
    void on_video_frame_timer_tick();
    /**
     * @brief 音频帧定时器tick
     */
    void on_audio_frame_timer_tick();
private:
    /// @brief 时间服务
    std::shared_ptr<TimeService> m_time_service = nullptr;
    /// @brief 视频帧定时器
    QTimer* m_video_frame_timer;
    /// @brief 音频帧定时器
    QTimer* m_audio_frame_timer;
    /// @brief 当前会话
    SessionEntity m_session;
    /// @brief 视频帧缓冲队列
    QQueue<SessionFrame> m_video_frames;
    /// @brief 音频帧缓冲队列
    QQueue<SessionFrame> m_audio_frames;
    /// @brief 音频基准时间戳（us）
    std::optional<int64_t> m_audio_pts_base_us;
    /// @brief 指标：已渲染视频帧数量
    std::atomic<int64_t> m_metrics_video_rendered = 0;
    /// @brief 指标：丢弃视频帧数量
    std::atomic<int64_t> m_metrics_video_dropped = 0;
    /// @brief 指标：时间差累计（us）
    std::atomic<int64_t> m_metrics_diff_sum_us = 0;
    /// @brief 指标：时间差统计次数
    std::atomic<int64_t> m_metrics_diff_count = 0;
    /// @brief 指标：时间差绝对值最大值（us）
    std::atomic<int64_t> m_metrics_diff_max_abs_us = 0;
    /// @brief 指标：视频队列长度
    std::atomic<int64_t> m_metrics_video_queue_size = 0;
    /// @brief 指标：音频队列长度
    std::atomic<int64_t> m_metrics_audio_queue_size = 0;
    /// @brief 指标任务ID
    uint64_t m_metrics_task_id = 0;
};