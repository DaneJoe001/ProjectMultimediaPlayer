/**
 * @file session_entity.hpp
 * @brief 会话实体
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <cstdint>

/**
 * @struct SessionEntity
 * @brief 会话实体
 */
struct SessionEntity
{
    /// @brief 会话ID
    int64_t session_id;
    /// @brief 视频时长（us）
    int64_t video_duration_us;
    /// @brief 起始时间（us）
    int64_t start_time_us;
    /// @brief 是否可seek
    bool is_seekable;
    /// @brief 视频码率
    int video_bitrate;
};