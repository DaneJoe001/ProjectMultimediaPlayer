/**
 * @file media_decode_worker.hpp
 * @brief 媒体解码worker
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <optional>
#include <unordered_map>

#include <QObject>

extern "C"
{
#include <libavutil/avutil.h>
}

#include "codec/av_packet_ptr.hpp"
#include "codec/av_format_context_ptr.hpp"
#include "codec/av_codec_context_ptr.hpp"
#include "model/session_entity.hpp"

#include "model/session_frame.hpp"

/**
 * @struct AVCodecContextPtrPending
 * @brief AVCodecContext暂存信息
 */
struct AVCodecContextPtrPending
{
    /// @brief 编解码上下文
    AVCodecContextPtr codec_context;
    /// @brief 媒体类型
    AVMediaType type;
    /// @brief 帧ID
    int64_t frame_id;
    /// @brief 时间基
    AVRational time_base;
};

/**
 * @class MediaDecodeWorker
 * @brief 媒体解码worker
 */
class MediaDecodeWorker : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent Qt父对象
     */
    MediaDecodeWorker(QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~MediaDecodeWorker();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 获取媒体时长
     * @return 媒体时长
     */
    uint64_t get_media_duration() const;
    /**
     * @brief 设置媒体文件路径
     * @param video_file_path 视频文件路径
     */
    void set_media_file_path(QString video_file_path);
    /**
     * @brief 解码tick
     */
    void decode_tick();
    /**
     * @brief 初始化解码器
     * @return true 表示初始化成功，false 表示初始化失败
     */
    bool init_decoder();
    /**
     * @brief 解码到packet
     * @return true 表示成功，false 表示失败
     */
    bool decode_to_packet();
    /**
     * @brief 解码到frame
     * @param stream_index 流索引
     * @param packet packet
     * @return true 表示成功，false 表示失败
     */
    bool decode_to_frame(unsigned int stream_index, AVPacketPtr& packet);
signals:
    /**
     * @brief 会话已就绪
     * @param session 会话实体
     */
    void session_ready(SessionEntity session);
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
public slots:
    /**
     * @brief 响应解码暂停/恢复
     * @param is_paused 是否暂停
     */
    void on_decode_paused(bool is_paused);
    /**
     * @brief 初始化槽
     */
    void on_init();
    /**
     * @brief 响应解码媒体文件
     * @param session_id 会话ID
     * @param video_file_path 视频文件路径
     */
    void on_decode_media_file(int64_t session_id, QString video_file_path);
private:
    /// @brief 编解码上下文表
    std::unordered_map<unsigned int, AVCodecContextPtrPending> m_codec_contexts;
    /// @brief 会话ID
    int64_t m_session_id = 0;

    /// @brief 是否更新文件路径
    bool is_update_file_path = true;
    /// @brief 是否暂停
    bool m_is_paused = false;
    /// @brief 视频文件路径
    QString m_video_file_path;
    /// @brief 解码定时器
    QTimer* m_decode_timer = nullptr;

    /// @brief 格式上下文
    AVFormatContextPtr m_format_context;

    /// @brief 默认视频流索引
    std::optional<unsigned int> m_default_video_stream_index = std::nullopt;
    /// @brief 默认音频流索引
    std::optional<unsigned int> m_default_audio_stream_index = std::nullopt;
};