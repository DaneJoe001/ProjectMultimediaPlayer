/**
 * @file ffmpeg_status_detail.hpp
 * @brief FFmpeg状态详情
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <string>
#include <string_view>
#include <optional>

#include "danejoe/common/status/i_status_detail.hpp"

/**
 * @class FFmpegStatusDetail
 * @brief FFmpeg状态详情
 */
class FFmpegStatusDetail : public DaneJoe::IStatusDetail
{
public:
    /**
     * @brief 构造函数
     * @param status_code 状态码
     * @param message 消息
     */
    FFmpegStatusDetail(
        int status_code,
        std::optional<std::string> message = std::nullopt);
    /**
     * @brief 构造函数
     * @param status_level 状态等级
     * @param message 消息
     */
    FFmpegStatusDetail(
        const DaneJoe::StatusLevel& status_level,
        std::optional<std::string> message = std::nullopt);
    /**
     * @brief 获取消息
     * @return 消息
     */
    std::string message() const override;
    /**
     * @brief 获取域
     * @return 域
     */
    std::string_view domain()const override;
    /**
     * @brief 相等比较
     * @param rhs 另一对象
     * @return true 表示相等，false 表示不相等
     */
    bool operator==(const FFmpegStatusDetail& rhs)const;
    /**
     * @brief 相等比较
     * @param rhs 另一对象
     * @return true 表示相等，false 表示不相等
     */
    bool operator==(const IStatusDetail& rhs) const override;
private:
    /**
     * @brief 状态码转换为状态等级
     * @param status_code 状态码
     * @return 状态等级
     */
    DaneJoe::StatusLevel to_status_level(int status_code) const;
private:
    /// @brief 域
    const char* m_domain = "FFmpegStatusDetail";
    /// @brief 消息
    std::optional<std::string> m_message;
    /// @brief 状态码
    int m_status_code = 0;
};
