#pragma once

#define FFMPEG_VERSION 771

#include <string>
#include <deque>
#include <mutex>
#include <thread>

#include "codec/av_frame_ptr.hpp"
#include "concurrent/blocking/mpmc_bounded_queue.hpp"

using namespace DaneJoe::Concurrent::Blocking;

int decode_mp4(const std::string& file_path, std::weak_ptr<MpmcBoundedQueue<AVFramePtr>> frame_queue);