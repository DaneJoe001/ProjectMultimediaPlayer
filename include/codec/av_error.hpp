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
    AVError()noexcept;
    AVError(int error_code)noexcept;
    std::string message()const;
    int code()const noexcept;
    bool failed()const noexcept;
    bool ok()const noexcept;
    explicit operator bool()const noexcept;
    AVError& operator=(int error_code)noexcept;
    bool operator==(int error_code)const noexcept;

private:
    int m_error_code;
};