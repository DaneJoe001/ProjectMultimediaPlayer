#pragma once

#include <cstdint>

#include "codec/av_frame_ptr.hpp"

struct SessionFrame
{
    int64_t session_id;
    int64_t frame_id;
    AVFramePtr frame;
};