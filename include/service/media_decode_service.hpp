/**
 * @file media_decode_service.hpp
 * @brief 媒体解码服务
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <atomic>
#include <cstdint>

#include <QString>
#include <QObject>
#include <QPointer>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "model/session_frame.hpp"
#include "controller/media_controller.hpp"


class QTimer;
class QThread;
class MediaDecodeWorker;

/**
 * @class MediaDecodeService
 * @brief 媒体解码服务
 */
class MediaDecodeService :public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param media_controller 媒体控制器
     * @param parent Qt父对象
     */
    MediaDecodeService(QPointer<MediaController> media_controller, QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~MediaDecodeService();
    /**
     * @brief 初始化
     */
    void init();
signals:
    /**
     * @brief 视频帧已就绪
     * @param frame 帧
     */
    void video_frame_ready(SessionFrame frame);
    /**
     * @brief 音频帧已就绪
     * @param frame 帧
     */
    void audio_frame_ready(SessionFrame frame);
    /**
     * @brief 触发解码媒体文件
     * @param session_id 会话ID
     * @param video_file_path 视频文件路径
     */
    void decode_media_file(int64_t session_id, QString video_file_path);
    /**
     * @brief 通知解码暂停/恢复
     * @param is_paused 是否暂停
     */
    void paused_decode(bool is_paused);
public slots:
    /**
     * @brief 响应打开媒体文件
     * @param video_file_path 视频文件路径
     */
    void on_decode_media_file(QString video_file_path);
    /**
     * @brief 响应解码暂停/恢复
     * @param is_paused 是否暂停
     */
    void on_paused_decode(bool is_paused);
private:
    /// @brief 会话ID
    std::atomic<int64_t> m_session_id = 0;
    /// @brief 解码线程
    QThread* m_decode_thread = nullptr;
    /// @brief 解码worker
    MediaDecodeWorker* m_decode_worker = nullptr;
    /// @brief 媒体控制器
    QPointer<MediaController> m_media_controller = nullptr;
};