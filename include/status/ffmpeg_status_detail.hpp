#pragma once

#include <string>
#include <string_view>
#include <optional>

#include "danejoe/common/status/i_status_detail.hpp"

class FFmpegStatusDetail : public DaneJoe::IStatusDetail
{
public:
    FFmpegStatusDetail(
        int status_code,
        std::optional<std::string> message = std::nullopt);
    FFmpegStatusDetail(
        const DaneJoe::StatusLevel& status_level,
        std::optional<std::string> message = std::nullopt);
    std::string message() const override;
    std::string_view domain()const override;
    bool operator==(const FFmpegStatusDetail& rhs)const;
    bool operator==(const IStatusDetail& rhs) const override;
private:
    DaneJoe::StatusLevel to_status_level(int status_code) const;
private:
    const char* m_domain = "FFmpegStatusDetail";
    std::optional<std::string> m_message;
    int m_status_code = 0;
};
