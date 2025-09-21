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
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "SDLVideoWidget", "Already initialized");
        return;
    }
    m_is_init = true;
    // 初始化帧队列
    m_frame_queue = std::make_shared<DaneJoe::MTQueue<AVFramePtr>>(512);
    // 创建一个QLabel，用于显示SDL渲染的图像
    m_sdl_label = new QLabel("sdl_label", this);
    m_sdl_label->setStyleSheet("background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);");
    this->setStyleSheet("background-color: rgb(255, 0, 0);color: rgba(44, 255, 58, 1);");
    m_sdl_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setGeometry(400, 400, 800, 600);
    m_main_layout = new QVBoxLayout(this);
    m_main_layout->addWidget(m_sdl_label);
    m_main_layout->setStretch(0, 1);
    m_main_layout->setSpacing(0);
    m_main_layout->setContentsMargins(0, 0, 0, 0);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    (void)m_sdl_label->winId();
    m_timer_id = startTimer(1000 / 25);
}

void SDLVideoWidget::init_renderer()
{
    if (m_renderer) return;
    m_renderer = std::make_shared<SDLFrameRenderer>();
    DaneJoe::Size<int> size = { m_sdl_label->size().width(), m_sdl_label->size().height() };
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Label size: {}, {}", size.x, size.y);
    bool is_set_window = m_renderer->set_window("sdl_window", size, (void*)m_sdl_label->winId());
    if (!is_set_window)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "Failed to set window");
        return;
    }
    bool is_renderer_init = m_renderer->init();
    if (!is_renderer_init)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "init renderer failed");
        return;
    }
}

std::weak_ptr<DaneJoe::MTQueue<AVFramePtr>> SDLVideoWidget::get_frame_queue()
{
    return m_frame_queue;
}

void SDLVideoWidget::closeEvent(QCloseEvent* event)
{
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Into closeEvent");
    close();
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "m_frame_queue closed after closeEvent");
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
    m_sdl_label->update();
}

void SDLVideoWidget::showEvent(QShowEvent* event)
{
    // 延后创建 SDL 渲染器到窗口显示后（避免在控件未显示时使用不稳定的 winId()）
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Into showEvent");
    QWidget::showEvent(event);
    // 只初始化一次
    init_renderer();
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
        return;
    }
    if (!m_frame_queue)
    {
        DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Frame queue is invalid");
        return;
    }
    auto data = m_frame_queue->try_pop();
    if (!data.has_value())
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "Frame queue is empty");
        return;
    }
    AVFramePtr frame = data.value();
    // DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "frame size: {}x{}", frame->width, frame->height);
    bool is_draw = m_renderer->draw(frame);
    if (!is_draw)
    {
        DANEJOE_LOG_ERROR("default", "SDLVideoWidget", "Faield to draw");
    }
}

SDLVideoWidget::~SDLVideoWidget()
{
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Begin destructor");
    close();
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "m_frame_queue closed after destructor");
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

void SDLVideoWidget::close()
{
    DANEJOE_LOG_TRACE("default", "SDLVideoWidget", "Into close");
    // 1. 先停止定时器，防止timerEvent继续执行
    if (m_timer_id > -1)
    {
        killTimer(m_timer_id);
    }
    if (m_frame_queue)
    {
        m_frame_queue->close();
    }
}