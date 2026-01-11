#include <string>

#include <QDebug>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include <danejoe/logger/logger_manager.hpp>

#include "view/sdl_frame_widget.hpp"
#include "renderer/sdl_frame_renderer.hpp"
#include "util/util_vector_2d.hpp"
#include "codec/av_frame_ptr.hpp"

SDLFrameWidget::SDLFrameWidget(QWidget* parent) :QWidget(parent)
{}
SDLFrameWidget::~SDLFrameWidget()
{
    DANEJOE_LOG_TRACE("default", "SDLFrameWidget", "Begin destructor");
    close();
    DANEJOE_LOG_TRACE("default", "SDLFrameWidget", "m_frame_queue closed after destructor");
}

void SDLFrameWidget::init()
{
    if (m_is_init)
    {
        DANEJOE_LOG_WARN("default", "SDLFrameWidget", "Already initialized");
        return;
    }
    m_is_init = true;
    // 创建一个QLabel，用于显示SDL渲染的图像
    // m_sdl_label = new QLabel("sdl_label", this);
    m_sdl_label = new QLabel(this);
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
}

void SDLFrameWidget::init_renderer()
{
    if (m_renderer) return;
    m_renderer = std::make_shared<SDLFrameRenderer>();
    auto s1 = m_sdl_label->contentsRect().size();
    auto dpr = m_sdl_label->devicePixelRatioF();
    auto pixel_size = QSize(s1.width() * dpr, s1.height() * dpr);
    DaneJoe::Size<int> size = { pixel_size.width(), pixel_size.height() };

    DANEJOE_LOG_TRACE("default", "SDLFrameWidget", "Label size: {}, {}", size.x, size.y);
    bool is_set_window = m_renderer->set_window("sdl_window", size, (void*)m_sdl_label->winId());
    if (!is_set_window)
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameWidget", "Failed to set window");
        return;
    }
    bool is_renderer_init = m_renderer->init();
    if (!is_renderer_init)
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameWidget", "init renderer failed");
        return;
    }
}

void SDLFrameWidget::draw(AVFramePtr frame)
{
    if (!m_renderer)
    {
        DANEJOE_LOG_TRACE("default", "SDLFrameWidget", "Renderer is invalid");
        return;
    }
    if (m_renderer->is_exit())
    {
        DANEJOE_LOG_INFO("default", "SDLFrameWidget", "Renderer is exit");
        this->close();
        return;
    }
    bool is_draw = m_renderer->draw(frame);
    if (!is_draw)
    {
        DANEJOE_LOG_ERROR("default", "SDLFrameWidget", "Faield to draw");
    }
}

void SDLFrameWidget::resizeEvent(QResizeEvent* event)
{

    QWidget::resizeEvent(event);
    if (!m_renderer)
    {
        DANEJOE_LOG_TRACE("default", "SDLFrameWidget", "Renderer is invalid");
        return;
    }
    auto s1 = m_sdl_label->contentsRect().size();
    auto dpr = m_sdl_label->devicePixelRatioF();
    auto pixel_size = QSize(s1.width() * dpr, s1.height() * dpr);
    m_renderer->update_window_size({ pixel_size.width(), pixel_size.height() });
    m_sdl_label->update();
}

void SDLFrameWidget::showEvent(QShowEvent* event)
{
    // 延后创建 SDL 渲染器到窗口显示后（避免在控件未显示时使用不稳定的 winId()）
    DANEJOE_LOG_TRACE("default", "SDLFrameWidget", "Into showEvent");
    QWidget::showEvent(event);
    // 只初始化一次
    init_renderer();
}