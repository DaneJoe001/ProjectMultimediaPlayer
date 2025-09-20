#include <iostream>
#include <type_traits>
#include <cstdint>
#include <string>

#include <QDebug>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include "log/manage_logger.hpp"
#include "view/sdl_video_widget.hpp"
#include "renderer/sdl_frame_renderer.hpp"
#include "util/util_vector_2d.hpp"
#include "codec/av_frame_ptr.hpp"

SDLVideoWidget::SDLVideoWidget(QWidget* parent) :QWidget(parent)
{

}

void SDLVideoWidget::init()
{
    // 初始化帧队列
    m_frame_queue = std::make_shared<DaneJoe::MTQueue<AVFramePtr>>(512);
    // 创建一个QLabel，用于显示SDL渲染的图像
    m_sdl_label = new QLabel("sdl_label", this);
    m_sdl_label->setStyleSheet("background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);");
    m_sdl_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_main_layout = new QVBoxLayout(this);
    m_main_layout->addWidget(m_sdl_label);

    // 设置窗口的尺寸
    this->setGeometry(400, 400, m_video_frame_size.x, m_video_frame_size.y);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    (void)m_sdl_label->winId();
    startTimer(1000 / 25);
}

std::shared_ptr<DaneJoe::MTQueue<AVFramePtr>> SDLVideoWidget::get_frame_queue()
{
    return m_frame_queue;
}

void SDLVideoWidget::closeEvent(QCloseEvent* event)
{
    m_frame_queue->close();
}

void SDLVideoWidget::resizeEvent(QResizeEvent* event)
{
    auto s1 = m_sdl_label->contentsRect().size();
    if (!m_renderer)
    {
        DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Renderer is invalid");
        return;
    }
    m_renderer->update_window_size({ s1.width(), s1.height() });
    m_renderer->set_dest_pos({ 0, 0 });
    // m_renderer->set_dest_size({ s1.width(), s1.height() });
    m_renderer->set_dest_size({ -1, -1 });
    m_sdl_label->update();
}

void SDLVideoWidget::showEvent(QShowEvent* event)
{
    // 延后创建 SDL 渲染器到窗口显示后（避免在控件未显示时使用不稳定的 winId()）
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Into showEvent");
    QWidget::showEvent(event);
    // 只初始化一次
    if (m_renderer) return;
    m_renderer = std::make_shared<SDLFrameRenderer>();
    if (!m_renderer)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "Failed to create SDL renderer");
        return;
    }
    DaneJoe::Size<int> size = { m_sdl_label->size().width(), m_sdl_label->size().height() };
    bool is_set_window = m_renderer->set_window("sdl_1", size, (void*)m_sdl_label->winId());
    if (!is_set_window)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "init sdl_1 failed");
    }
    bool is_set_dest_area = m_renderer->set_dest_area({ 0,0 }, size);
    if (!is_set_dest_area)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "set dest area failed");
    }
    // 初始化原始帧大小
    bool is_set_raw_frame_size = m_renderer->set_raw_frame_size(size);
    if (!is_set_raw_frame_size)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "set raw frame size failed");
    }
    bool is_renderer_init = m_renderer->init(IFrameRenderer::FrameFmt::YUV420P);
    if (!is_renderer_init)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "init renderer failed");
    }
}

void SDLVideoWidget::timerEvent(QTimerEvent* event)
{
    if (!m_renderer)
    {
        DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Renderer is invalid");
        return;
    }
    if (m_renderer->is_exit())
    {
        DANEJOE_LOG_INFO("default", "SDLVideoWidget", "Renderer is exit");
        this->close();
    }
    if (m_frame_queue)
    {
        auto data = m_frame_queue->try_pop();
        if (data.has_value())
        {
            auto frame = data.value();
            bool is_draw = m_renderer->draw(frame);
            if (!is_draw)
            {
                DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "Faield to draw: {}", frame.get_error().message());
            }
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "Invalid frame");
        }
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "Frame queue is invalid");
    }
}

SDLVideoWidget::~SDLVideoWidget()
{
}

void SDLVideoWidget::sleep(std::chrono::milliseconds ms)
{
    auto time = std::chrono::high_resolution_clock::now();
    std::chrono::_V2::system_clock::time_point end_time = time + ms;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        time = std::chrono::high_resolution_clock::now();
    } while (time < end_time);
}