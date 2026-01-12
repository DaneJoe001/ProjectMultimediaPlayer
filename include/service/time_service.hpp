/**
 * @file time_service.hpp
 * @brief 时间服务
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <cstdint>
#include <atomic>

/**
 * @class TimeService
 * @brief 时间服务
 */
class TimeService
{
public:
    /**
     * @brief 构造函数
     */
    TimeService();
    /**
     * @brief 增加音频时间
     * @param us 时间（us）
     */
    void add_audio_time_us(int64_t us);
    /**
     * @brief 获取音频时间
     * @return 音频时间（us）
     */
    int64_t get_audio_time_us() const;
    /**
     * @brief 重置音频时间
     */
    void reset_audio_time();
private:
    /// @brief 音频时间（us）
    std::atomic<int64_t> m_audio_time_us = 0;
};