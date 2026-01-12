/**
 * @file av_stream_ptr.hpp
 * @brief AVStream指针封装
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

struct AVStream;

/**
 * @class AVStreamPtr
 * @brief AVStream指针封装
 */
class AVStreamPtr
{
public:
    /**
     * @brief 构造函数
     * @param stream AVStream指针
     */
    AVStreamPtr(AVStream* stream);
private:
    /// @brief AVStream指针
    AVStream* m_stream = nullptr;
};