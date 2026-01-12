/**
 * @file av_codec_context_ptr.hpp
 * @brief AVCodecContext智能指针封装
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include "status/ffmpeg_status_detail.hpp"
#include "codec/av_packet_ptr.hpp"
#include "codec/av_frame_ptr.hpp"

struct AVCodec;
struct AVDictionary;
struct AVCodecContext;
struct AVCodecParameters;

/**
 * @class AVCodecContextPtr
 * @brief AVCodecContext智能指针封装
 */
class AVCodecContextPtr
{
public:
    /**
     * @brief 构造函数
     */
    AVCodecContextPtr();
    /**
     * @brief 析构函数
     */
    ~AVCodecContextPtr();
    /**
     * @brief 获取内部指针
     * @return AVCodecContext指针，若未初始化则为nullptr
     */
    AVCodecContext* get()const;
    /**
     * @brief 分配AVCodecContext
     * @param codec 解码器
     * @return true 表示分配成功，false 表示分配失败
     */
    bool alloc_context3(const AVCodec* codec);
    /**
     * @brief 发送packet到解码器
     * @param packet 待发送的packet
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail send_packet(AVPacketPtr& packet);
    /**
     * @brief 从解码器接收frame
     * @param frame 输出frame
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail receive_frame(AVFramePtr& frame);
    /**
     * @brief 将参数拷贝到AVCodecContext
     * @param parameters 编解码参数
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail parameters_to_context(const AVCodecParameters* parameters);
    /**
     * @brief 打开编解码器
     * @param codec 编解码器
     * @param options 额外选项
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail open2(
        const AVCodec* codec,
        AVDictionary** options);
    /**
     * @brief 指针访问运算符
     * @return AVCodecContext指针，若未初始化则为nullptr
     */
    AVCodecContext* operator->()const;
private:
    /// @brief AVCodecContext指针
    AVCodecContext* m_codec_context = nullptr;
};