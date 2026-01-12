/**
 * @file av_common.hpp
 * @brief ffmpeg通用类型封装
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once
extern "C"
{
#include "libavutil/rational.h"
}

/**
 * @struct AVRationalInfo
 * @brief AVRational信息封装
 */
struct AVRationalInfo
{
    /**
     * @brief 构造函数
     * @param av_rational ffmpeg AVRational
     */
    AVRationalInfo(AVRational av_rational);
    /**
     * @brief 获取ffmpeg AVRational
     * @return ffmpeg AVRational
     */
    AVRational get_av_rational()const;
    /**
     * @brief 获取double数值
     * @return 转换后的double数值
     */
    double get_double();
    /// @brief 分子
    int num;
    /// @brief 分母
    int den;
};