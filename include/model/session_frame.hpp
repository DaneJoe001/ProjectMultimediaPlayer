/**
 * @file session_frame.hpp
 * @brief 会话帧
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <cstdint>

#include "codec/av_frame_ptr.hpp"

/**
 * @struct SessionFrame
 * @brief 会话帧
 */
struct SessionFrame
{
    /// @brief 会话ID
    int64_t session_id;
    /// @brief 帧ID
    int64_t frame_id;
    /// @brief 相对时间（us）
    int64_t relative_time_us;
    /// @brief ffmpeg帧
    AVFramePtr frame;
};