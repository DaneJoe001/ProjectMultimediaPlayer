#pragma once

#include <string>
#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

class AVError
{
public:
    AVError(int error_code);

    std::string error_msg();

    int error_code()const;
    operator bool()const;
    bool operator==(int error_code);

private:
    int m_error_code;
};