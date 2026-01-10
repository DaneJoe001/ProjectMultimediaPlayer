extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
}

#include "status/ffmpeg_status_detail.hpp"

FFmpegStatusDetail::FFmpegStatusDetail(
    int status_code,
    std::optional<std::string> message) :
    m_message(message),
    m_status_code(status_code)
{
    m_status_level = to_status_level(status_code);
}

FFmpegStatusDetail::FFmpegStatusDetail(
    const DaneJoe::StatusLevel& status_level,
    std::optional<std::string> message) :
    m_message(message)
{
    m_status_level = status_level;
}
std::string FFmpegStatusDetail::message() const
{
    if (m_status_level == DaneJoe::StatusLevel::Ok)
    {
        return m_message.value_or("Have no error!");
    }
    if (m_status_level == DaneJoe::StatusLevel::Error &&
        m_status_code >= 0)
    {
        return m_message.value_or("Unknow error!");
    }
    char buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    av_strerror(m_status_code, buffer, sizeof(buffer));
    return std::string(buffer);
}
std::string_view FFmpegStatusDetail::domain()const
{
    return m_domain;
}

bool FFmpegStatusDetail::operator==(const FFmpegStatusDetail& rhs)const
{
    return rhs.m_status_code == m_status_code;
}
bool FFmpegStatusDetail::operator==(const IStatusDetail& rhs) const
{
    auto rhs_detail =
        dynamic_cast<const FFmpegStatusDetail*>(&rhs);
    if (!rhs_detail)
    {
        return false;
    }
    return rhs_detail->m_status_code == m_status_code;
}

DaneJoe::StatusLevel FFmpegStatusDetail::to_status_level(int status_code) const
{
    if (status_code >= 0)
    {
        return DaneJoe::StatusLevel::Ok;
    }
    else if (status_code == AVERROR(EAGAIN) ||
        status_code == AVERROR_EOF)
    {
        return DaneJoe::StatusLevel::Branch;
    }
    else
    {
        return DaneJoe::StatusLevel::Error;
    }
}