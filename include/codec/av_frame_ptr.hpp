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

/**
 * @class AVFramePtr
 * @brief AVFrame的RAII封装
 */
class AVFramePtr
{
public:
    AVFramePtr();
    /**
     * @brief 构造函数
     * @param width 宽度
     * @param height 高度
     * @param format 像素格式
     * @param align 对齐方式
     */
    AVFramePtr(int width, int height, AVPixelFormat format, int align = 0);
    /**
     * @brief 拷贝构造函数
     * @param frame
     * @note 拷贝构造函数
     */
    AVFramePtr(const AVFramePtr& frame);
    /**
     * @brief 移动构造函数
     * @param frame
     * @note 移动构造函数
     */
    AVFramePtr(AVFramePtr&& frame);
    /**
     * @brief 拷贝赋值运算符
     * @param frame
     */
    AVFramePtr& operator=(const AVFramePtr& frame);
    /**
     * @brief 移动赋值运算符
     */
    AVFramePtr& operator=(AVFramePtr&& frame);
    /**
     * @brief 获取AVFrame指针
     * @return AVFrame*
     */
    AVFrame* get()const;
    bool init(int width, int height, AVPixelFormat format, int align = 0);
    /**
     * @brief 获取AVFrame指针
     * @return AVFrame*
     */
    AVFrame* operator->()const;
    /**
     * @brief 获取错误码
     * @return int
     */
    int error_code()const;
    /**
     * @brief 获取错误信息
     * @return std::string
     */
    std::string error_msg();
    /**
     * @brief 判断是否为空
     */
    operator bool()const;
    /**
     * @brief 获取引用计数
     * @return std::size_t
     */
    std::size_t use_count()const;
    /**
     * @brief 析构函数
     */
    ~AVFramePtr();
private:
    AVFrame* m_frame = nullptr;
    int m_error_code = 0;
    std::vector<char> m_error_buffer = std::vector<char>(1024);
};