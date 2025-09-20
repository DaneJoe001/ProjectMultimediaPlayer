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
    /**
     * @brief 默认构造：表示成功（code == 0）。
     */
    AVError() noexcept;
    /**
     * @brief 以 FFmpeg 错误码构造。
     * @param error_code FFmpeg 风格错误码（>=0 成功，<0 失败）。
     */
    AVError(int error_code) noexcept;

    /**
     * @brief 可读错误消息。
     * @return 成功时返回 "No error"；失败时使用 `av_strerror` 生成消息。
     */
    std::string message() const;

    /**
     * @brief 获取原始错误码。
     */
    int code() const noexcept;
    /**
     * @brief 是否成功（code >= 0）。
     */
    bool ok() const noexcept;      // code >= 0
    /**
     * @brief 是否失败（code < 0）。
     */
    bool failed() const noexcept;  // code < 0
    /**
     * @brief 显式布尔转换，等价于 `ok()`。
     * 重要：此操作符返回"成功"状态，与 FFmpeg 错误码语义相反！
     * - FFmpeg: 0 = 成功，非 0 = 失败
     * - operator bool(): true = 成功，false = 失败
     * - 在条件判断中应使用 ok()/failed() 而非 if(error) 以避免混淆
     */
    explicit operator bool() const noexcept; // 等价 ok()
    /**
     * @brief 以 FFmpeg 错误码赋值。
     */
    AVError& operator=(int error_code) noexcept;
    /**
     * @brief 与整数错误码比较相等。
     */
    bool operator==(int error_code) const noexcept;
    /**
     * @brief 与整数错误码比较不相等。
     */
    bool operator!=(int error_code) const noexcept { return !(*this == error_code); }

private:
    int m_error_code; ///< 原始错误码（>=0 成功，<0 失败）
};