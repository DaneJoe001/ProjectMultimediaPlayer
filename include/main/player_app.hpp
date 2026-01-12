/**
 * @file player_app.hpp
 * @brief 播放器应用
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <memory>

#include <QObject>
#include <QThread>
#include <QPointer>

#include "service/media_decode_service.hpp"
#include "worker/sdl_audio_worker.hpp"
#include "controller/media_controller.hpp"
#include "service/time_service.hpp"

class MainWindow;

/**
 * @class PlayerApp
 * @brief 播放器应用
 */
class PlayerApp : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent Qt父对象
     */
    PlayerApp(QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~PlayerApp();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 显示主窗口
     */
    void show_main_window();
signals:
    /**
     * @brief 停止音频信号
     */
    void stop_audio();
private:
    /**
     * @brief 清理日志
     */
    void clear_logger();
    /**
     * @brief 初始化日志
     */
    void init_logger();
    /**
     * @brief 清理数据库
     */
    void clear_database();
    /**
     * @brief 初始化数据库
     */
    void init_database();
public slots:
    /**
     * @brief 打开媒体文件
     * @param video_file_path 视频文件路径
     */
    void on_open_midea_file(QString video_file_path);
    /**
     * @brief 窗口关闭事件
     */
    void on_window_close();
private:
    /// @brief 媒体解码服务
    MediaDecodeService* m_media_decode_service = nullptr;
    /// @brief 主窗口
    MainWindow* m_main_window = nullptr;
    /// @brief SDL音频worker
    SDLAudioWorker* m_sdl_audio_worker = nullptr;
    /// @brief 音频线程
    QThread* m_audio_thread = nullptr;
    /// @brief 媒体控制器
    QPointer<MediaController> m_media_controller = nullptr;
    /// @brief 时间服务
    std::shared_ptr<TimeService> m_time_service = nullptr;
};