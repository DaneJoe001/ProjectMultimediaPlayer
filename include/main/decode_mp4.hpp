#pragma once

#define FFMPEG_VERSION 771

#include <string>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>

#include "codec/av_frame_ptr.hpp"

int decode_mp4(const std::string& file_path,
    std::weak_ptr<DaneJoe::MpmcBoundedQueue<AVFramePtr>> frame_queue);