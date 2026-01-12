/**
 * @file decode_mp4.hpp
 * @brief mp4解码接口
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

/// @brief ffmpeg版本
#define FFMPEG_VERSION 771

#include <string>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "codec/av_frame_ptr.hpp"

/**
 * @brief 解码mp4文件
 * @param file_path 文件路径
 * @param frame_queue 输出帧队列
 * @return 解码结果，0 表示成功，非0表示失败
 */
int decode_mp4(const std::string& file_path,
    std::weak_ptr<DaneJoe::MpmcBoundedQueue<AVFramePtr>> frame_queue);