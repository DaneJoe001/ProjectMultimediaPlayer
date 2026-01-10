extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
}
#include "codec/av_codec_ptr.hpp"

AVCodecPtr::AVCodecPtr(AVCodec* codec) :m_codec(codec) {}