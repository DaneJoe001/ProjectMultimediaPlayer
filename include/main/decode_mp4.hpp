#pragma once

#define FFMPEG_VERSION 771

#include <string>
#include <deque>
#include <mutex>
#include <thread>

#include "codec/av_frame_ptr.hpp"

int decode_mp4(const std::string& file_path);