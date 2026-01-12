/**
 * @file av_format_context_ptr.hpp
 * @brief AVFormatContext智能指针封装
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include "status/ffmpeg_status_detail.hpp"
#include "codec/av_packet_ptr.hpp"

struct AVInputFormat;
struct AVDictionary;
struct AVFormatContext;

/**
 * @class AVFormatContextPtr
 * @brief AVFormatContext智能指针封装
 */
class AVFormatContextPtr
{
public:
    /**
     * @brief 构造函数
     */
    AVFormatContextPtr();
    /**
     * @brief 构造函数
     * @param av_format_context AVFormatContext指针
     */
    AVFormatContextPtr(AVFormatContext* av_format_context);
    /**
     * @brief 打开输入
     * @param file_path 文件路径
     * @param fmt 输入格式
     * @param options 额外选项
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail open_input(
        const std::string& file_path,
        AVInputFormat* fmt,
        AVDictionary** options);
    /**
     * @brief 关闭输入
     */
    void close_input();
    /**
     * @brief 查找流信息
     * @param options 额外选项
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail find_stream_info(AVDictionary** options);
    /**
     * @brief 读取一帧数据
     * @param packet 输出packet
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail read_frame(AVPacketPtr& packet);
    /**
     * @brief 获取内部指针
     * @return AVFormatContext指针，若未初始化则为nullptr
     */
    AVFormatContext* get()const;
    /**
     * @brief 是否有效
     * @return true 表示有效，false 表示无效
     */
    bool is_valid()const;
    /**
     * @brief 是否已打开输入
     * @return true 表示已打开输入，false 表示未打开输入
     */
    bool is_open_input()const;
    /**
     * @brief 指针访问运算符
     * @return AVFormatContext指针，若未初始化则为nullptr
     */
    AVFormatContext* operator->()const;
    /**
     * @brief bool转换运算符
     * @return true 表示有效，false 表示无效
     */
    operator bool()const;
    /**
     * @brief 析构函数
     */
    ~AVFormatContextPtr();
private:
    /// @brief 是否已打开输入
    bool m_is_open_input = false;
    /// @brief AVFormatContext指针
    AVFormatContext* m_av_format_context = nullptr;
};