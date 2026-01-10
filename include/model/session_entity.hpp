#pragma once

#include <cstdint>

struct SessionEntity
{
    int64_t session_id;
    int64_t video_duration_us;
    int64_t start_time_us;
    bool is_seekable;
    int video_bitrate;
};