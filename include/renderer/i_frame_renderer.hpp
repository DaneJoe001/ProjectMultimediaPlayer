#pragma once

#include <queue>
#include <memory>
#include <cstdint>
#include <vector>
#include <mutex>
#include <atomic>
#include <string>

#include "util/util_vector_2d.hpp"
#include "log/manage_logger.hpp"
#include "codec/av_frame_ptr.hpp"

/**
 * @class IFrameRenderer
 * @brief 帧渲染接口
 */
class IFrameRenderer
{
public:
    /**
     * @enum FrameFmt
     * @brief 帧格式枚举
     */
    enum class FrameFmt
    {
        RGB888,
        RGBA8888,
        ARGB8888,
        YUV420P,
    };
    /**
     * @struct Frame
     * @brief 帧结构体
     */
    struct Frame
    {
        /// @brief 是否有效
        bool is_valid;
        /// @brief 帧格式
        FrameFmt fmt;
        /// @brief 帧数据
        std::vector<uint8_t> data;
        /// @brief 帧大小
        DaneJoe::Size<int> size;
        /// @brief 帧行像素字节数
        int pitch;
        /// @brief 像素字节数
        int pixel_size;
        /**
         * @brief 初始化pitch等帧信息
         */
        void init_info();
    };
public:
    IFrameRenderer();
    /**
     * @brief 获取错误码
     */
    virtual int error_code();
    /**
     * @brief 获取错误信息
     */
    virtual std::string error_msg(int error_code);
    /**
     * @brief 设置帧格式
     */
    virtual void set_fmt(FrameFmt fmt) = 0;
    /**
     * @brief 初始化
     */
    virtual bool init(FrameFmt fmt) = 0;
    /**
     * @brief 退出
     */
    virtual bool is_exit() = 0;
    /**
     * @brief 绘制
     * @param frame 帧数据
     */
    virtual bool draw(std::shared_ptr<Frame> frame) = 0;
    virtual bool draw(AVFramePtr frame) = 0;
    /**
     * @brief 设置窗口
     * @param window_name 窗口名
     * @param window_size 窗口大小
     * @param window 窗口指针
     */
    virtual bool set_window(std::string window_name, DaneJoe::Size<int> window_size, void* window) = 0;
    /**
     * @brief 更新窗口大小
     * @param window_size 窗口大小
     */
    virtual bool update_window_size(DaneJoe::Size<int> window_size) = 0;
    /**
     * @brief 析构函数
     */
    virtual ~IFrameRenderer();
    /**
     * @brief 设置渲染目标尺寸
     * @param size 目标尺寸
     */
    bool set_dest_area(const DaneJoe::Pos<int>& pos, const DaneJoe::Size<int>& size);
    /**
     * @brief 设置渲染目标位置
     */
    void set_dest_pos(const DaneJoe::Pos<int>& pos);
    /**
     * @brief 设置渲染目标尺寸
     */
    bool set_dest_size(const DaneJoe::Size<int>& size);
    /**
     * @brief 获取渲染目标尺寸
     */
    DaneJoe::Size<int> get_dest_size()const;
    /**
     * @brief 获取渲染目标位置
     */
    DaneJoe::Pos<int> get_dest_pos()const;
    /**
     * @brief 设置原始帧尺寸
     * @param size
     */
    bool set_raw_frame_size(const DaneJoe::Size<int>& size);
    /**
     * @brief 设置窗口尺寸
     * @param size 窗口尺寸
     */
    bool set_window_size(const DaneJoe::Size<int>& size);

protected:
    const int BASE_ERROR_CODE_QUANTITY = 2;
protected:
    /// @brief 窗口尺寸
    DaneJoe::Size<int> m_window_size = { 0,0 };
    /// @brief 原始帧尺寸
    DaneJoe::Size<int> m_raw_frame_size = { 0,0 };
    /// @brief 帧绘制目标尺寸
    DaneJoe::Size<int> m_dest_size = { 0,0 };
    /// @brief 帧绘制目标位置
    DaneJoe::Pos<int> m_dest_pos = { 0,0 };
    /// @brief 窗口名称
    std::string m_window_name;
    /// @brief 窗口尺寸互斥锁
    std::mutex m_window_size_mutex;
    /// @brief 帧绘制互斥锁
    std::mutex m_draw_mutex;
    /// @brief 帧绘制目标尺寸互斥锁
    std::mutex m_dest_size_mutex;
    /// @brief 帧绘制目标位置互斥锁
    std::mutex m_dest_pos_mutex;
    /// @brief 帧原始尺寸互斥锁
    std::mutex m_raw_frame_size_mutex;
    /// @brief 错误码
    std::atomic<int> m_error_code = 0;
};