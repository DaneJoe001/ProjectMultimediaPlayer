/**
 * @file sdl_audio_renderer.hpp
 * @brief SDL音频渲染器
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <string>
#include <atomic>
#include <memory>

#include <SDL2/SDL.h>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "codec/av_frame_ptr.hpp"
#include "service/time_service.hpp"

/**
 * @class SDLAudioSystem
 * @brief SDL音频子系统初始化器
 */
class SDLAudioSystem
{
public:
    /**
     * @brief 构造函数
     */
    SDLAudioSystem();
    /**
     * @brief 析构函数
     */
    ~SDLAudioSystem();
private:
    /// @brief SDL初始化计数
    static std::atomic<int> m_init_times;
};

/**
 * @class SDLAudioRenderer
 * @brief SDL音频渲染器
 */
class SDLAudioRenderer
{
public:
    /**
     * @brief 构造函数
     */
    SDLAudioRenderer();
    /**
     * @brief 析构函数
     */
    ~SDLAudioRenderer();
    /**
     * @brief 播放指定文件
     * @param file_path 文件路径
     */
    void play(const std::string& file_path);
    /**
     * @brief 启动播放
     */
    void start();
    /**
     * @brief 停止播放
     */
    void stop();
    /**
     * @brief 追加PCM数据
     * @param frame 音频帧
     */
    void append_pcm(AVFramePtr frame);
    /**
     * @brief 追加PCM数据
     * @param pcm PCM数据指针
     * @param size PCM数据长度（字节）
     */
    void append_pcm(uint8_t* pcm, int size);
    /**
     * @brief 设置时间服务
     * @param time_service 时间服务
     */
    void set_time_service(std::shared_ptr<TimeService> time_service);
    /**
     * @brief 获取每秒字节数
     * @return 每秒字节数
     */
    int64_t get_bytes_per_second()const;
private:
    /// @brief 时间服务
    std::shared_ptr<TimeService> m_time_service = nullptr;
    /// @brief SDL音频子系统
    SDLAudioSystem m_audio_system;
    /// @brief PCM缓冲队列
    DaneJoe::MpmcBoundedQueue<uint8_t> m_pcm_buffer = DaneJoe::MpmcBoundedQueue<uint8_t>(8600);;
    /// @brief SDL音频设备ID
    SDL_AudioDeviceID m_device_id = 0;

    /// @brief 音频采样率
    int m_audio_frequency = 48000;
    /// @brief 音频采样格式
    int m_audio_format = AUDIO_S16SYS;
    /// @brief 声道数
    int m_audio_channels = 2;

    /// @brief 是否正在播放
    std::atomic<bool> m_is_playing = false;
};