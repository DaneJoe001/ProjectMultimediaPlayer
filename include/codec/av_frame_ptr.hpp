#pragma once

#include <string>
#include <vector>
#include <cstdint>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

#include "av_error.hpp"

/**
 * @class AVFramePtr
 * @brief AVFrame的RAII封装
 */
class AVFramePtr
{
public:
    /**
     * @brief 默认构造：不分配底层 AVFrame 结构（惰性分配）。
     * @why 避免无谓的分配/释放；允许在容器中大规模创建占位对象。
     */
    AVFramePtr() noexcept;
    /**
     * @brief 带参数构造：分配并初始化底层缓冲。
     * @param width 宽度
     * @param height 高度
     * @param format 像素格式
     * @param align 对齐方式（0 表示使用默认对齐）
     */
    AVFramePtr(int width, int height, AVPixelFormat format, int align = 0);
    /**
     * @brief 拷贝构造：共享底层缓冲（`av_frame_ref`）。
     * @note 当源为空时保持为空，不分配。
     */
    AVFramePtr(const AVFramePtr& frame);
    /**
     * @brief 移动构造：接管源的底层指针与错误码，源置空。
     */
    AVFramePtr(AVFramePtr&& frame) noexcept;
    /**
     * @brief 拷贝赋值：强异常安全，失败不修改当前对象。
     */
    AVFramePtr& operator=(const AVFramePtr& frame);
    /**
     * @brief 移动赋值：释放当前，接管源（noexcept）。
     */
    AVFramePtr& operator=(AVFramePtr&& frame) noexcept;
    /**
     * @brief 获取 AVFrame 指针（可能为 nullptr）。
     */
    AVFrame* get() noexcept;
    /**
     * @brief 获取 AVFrame 指针（const 版本，可能为 nullptr）。
     */
    const AVFrame* get() const noexcept;
    /**
     * @brief 最近一次操作的错误码（0 成功，非 0 失败）。
     */
    AVError get_error() const noexcept;
    /**
     * @brief 分配并初始化底层缓冲；失败返回错误码。
     */
    AVError init(int width, int height, AVPixelFormat format, int align = 0);
    /**
     * @brief 获取 AVFrame 指针（语法糖）。
     */
    AVFrame* operator->() noexcept;
    /**
     * @brief 获取 AVFrame 指针（const 语法糖）。
     */
    const AVFrame* operator->() const noexcept;
    /**
     * @brief 解引用（未分配时解引用 UB，由调用方保证非空）。
     */
    AVFrame& operator*() noexcept;
    /**
     * @brief 解引用（const）。
     */
    const AVFrame& operator*() const noexcept;
    /**
     * @brief 是否持有底层 AVFrame 指针。
     */
    explicit operator bool() const noexcept;
    /**
     * @brief 返回底层缓冲的引用计数（当存在 `buf[0]` 时）。
     */
    std::size_t use_count() const noexcept;
    /**
     * @brief 释放结构体与缓冲并置空（等价 `av_frame_free`）。
     */
    void reset() noexcept;
    /**
     * @brief 清空缓冲但保留结构体（等价 `av_frame_unref`）。
     */
    void unref() noexcept;
    /**
     * @brief 确保已分配结构体（惰性分配）；失败时设置错误码并返回。
     */
    AVError ensure_allocated() noexcept;
    /**
     * @brief 释放所有权并返回裸指针（调用者负责 `av_frame_free`）。
     */
    AVFrame* release() noexcept;
    /**
     * @brief 与另一对象交换内部状态（noexcept）。
     */
    void swap(AVFramePtr& other) noexcept;
    /**
     * @brief 析构：释放持有的资源（noexcept）。
     */
    ~AVFramePtr() noexcept;
private:
    AVFrame* m_frame = nullptr;   ///< 持有的 AVFrame 指针（可能为 nullptr）
    AVError m_error;              ///< 最近一次操作的错误码（0 成功，非 0 失败）
};

/// 非成员 swap，利于 ADL 与泛型算法
inline void swap(AVFramePtr& a, AVFramePtr& b) noexcept { a.swap(b); }