/**
 * @file av_packet_ptr.hpp
 * @brief AVPacket的RAII封装
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include "status/ffmpeg_status_detail.hpp"

struct AVPacket;

/**
 * @class AVPacketPtr
 * @brief AVPacket的RAII封装
 */
class AVPacketPtr
{
public:
    /**
     * @brief 构造函数
     */
    AVPacketPtr() noexcept;
    /**
     * @brief 析构函数
     */
    ~AVPacketPtr();
    /**
     * @brief 拷贝构造
     * @param other 另一对象
     */
    AVPacketPtr(const AVPacketPtr& other);
    /**
     * @brief 移动构造
     * @param other 另一对象
     */
    AVPacketPtr(AVPacketPtr&& other)noexcept;
    /**
     * @brief 拷贝赋值
     * @param other 另一对象
     * @return 当前对象引用
     */
    AVPacketPtr& operator=(const AVPacketPtr& other);
    /**
     * @brief 移动赋值
     * @param other 另一对象
     * @return 当前对象引用
     */
    AVPacketPtr& operator=(AVPacketPtr&& other)noexcept;
    /**
     * @brief bool转换运算符
     * @return true 表示有效，false 表示无效
     */
    operator bool()const;
    /**
     * @brief 确保已分配结构体
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail ensure_allocated() noexcept;
    /**
     * @brief 获取内部指针
     * @return AVPacket指针，若未分配则为nullptr
     */
    AVPacket* get()noexcept;
    /**
     * @brief 获取内部指针
     * @return AVPacket指针，若未分配则为nullptr
     */
    const AVPacket* get() const noexcept;
    /**
     * @brief 释放所有权并返回裸指针
     * @return AVPacket指针，调用者负责释放
     */
    AVPacket* release()noexcept;
    /**
     * @brief 释放持有资源并置空
     */
    void reset();
    /**
     * @brief 引用另一packet
     * @param other 另一对象
     * @return FFmpeg状态详情
     */
    FFmpegStatusDetail ref(const AVPacketPtr& other);
    /**
     * @brief 解除引用
     */
    void unref()noexcept;
    /**
     * @brief 解引用
     * @return AVPacket引用
     */
    AVPacket& operator*();
    /**
     * @brief 指针访问运算符
     * @return AVPacket指针，若未分配则为nullptr
     */
    AVPacket* operator->()noexcept;
    /**
     * @brief 与另一对象交换内部状态
     * @param other 另一对象
     */
    void swap(AVPacketPtr& other)noexcept;
private:
    /// @brief AVPacket指针
    AVPacket* m_packet = nullptr;
};