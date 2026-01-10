extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}
#include "codec/av_stream_ptr.hpp"

AVStreamPtr::AVStreamPtr(AVStream* stream)
    :m_stream(stream)
{}