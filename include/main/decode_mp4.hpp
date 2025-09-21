#pragma once

#define FFMPEG_VERSION 771

#include <string>
#include <deque>
#include <mutex>
#include <thread>

#include "codec/av_frame_ptr.hpp"
#include "mt_queue/mt_queue.hpp"

int decode_mp4(const std::string& file_path, std::weak_ptr<DaneJoe::MTQueue<AVFramePtr>> frame_queue);