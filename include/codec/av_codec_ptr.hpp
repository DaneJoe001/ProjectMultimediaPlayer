/**
 * @file av_codec_ptr.hpp
 * @brief AVCodec指针封装
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

struct AVCodec;

/**
 * @class AVCodecPtr
 * @brief AVCodec指针封装
 */
class AVCodecPtr
{
public:
    /**
     * @brief 构造函数
     * @param codec AVCodec指针
     */
    AVCodecPtr(AVCodec* codec);
private:
    /// @brief AVCodec指针
    AVCodec* m_codec = nullptr;
};