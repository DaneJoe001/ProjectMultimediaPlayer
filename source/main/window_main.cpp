#include <iostream>
#include <type_traits>

#include <QDebug>
#include <cstdint>
#include <string>
#include <QMessageBox>
#include <QDebug>

#include "log/i_logger.hpp"
#include "log/manage_logger.hpp"

#include "main/window_main.hpp"
#include "renderer/i_frame_renderer.hpp"
#include "renderer/sdl_frame_renderer.hpp"
#include "util/util_vector_2d.hpp"
#include "codec/av_frame_ptr.hpp"
#include "main/decode_mp4.hpp"
#include "mt_queue/mt_queue.hpp"

extern DaneJoe::MTQueue<AVFramePtr> frame_queue;
WindowMain::WindowMain(QWidget* parent) :QMainWindow(parent)
{
    m_video_frame_size = { 400, 300 };

    /// @brief 创建一个QLabel，用于显示SDL渲染的图像
    m_label_sdl_1 = new QLabel("label_sdl_1", this);

    m_label_sdl_1->setStyleSheet("background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);");

    m_labels_container = new QWidget(this);
    this->setCentralWidget(m_labels_container);

    /// @brief 把容器与两个 QLabel 变为原生窗口，并清零边距
    m_labels_container->setAttribute(Qt::WA_NativeWindow);
    m_label_sdl_1->setAttribute(Qt::WA_NativeWindow);

    m_labels_layout = new QHBoxLayout(m_labels_container);
    m_labels_layout->addWidget(m_label_sdl_1);
    m_labels_layout->setSpacing(0);
    m_labels_layout->setContentsMargins(0, 0, 0, 0);
    m_labels_layout->setStretch(0, 1);

    m_labels_container->setLayout(m_labels_layout);

    /// @brief 设置窗口的尺寸
    this->setGeometry(400, 400, m_video_frame_size.x, m_video_frame_size.y);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->m_label_sdl_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /// @brief 设置QLabel为窗口的中心部件
    // this->setCentralWidget(m_label_sdl_1);


    /// @brief 获取QLabel的尺寸
    // DaneJoe::Size<int> size{ m_label_sdl_1->size().width(), m_label_sdl_1->size().height() };

    /// @brief 创建一个SDLFrameRenderer，用于渲染图像

    (void)m_label_sdl_1->winId();

    m_frame = std::make_shared<IFrameRenderer::Frame>();

    std::string path = "/home/danejoe001/personal_code/code_cpp_project/cpp_project_multimedia/resource/400_300_25.yuv";

    m_video_file = std::ifstream(path, std::ios::binary);
    if (!m_video_file.is_open())
    {
        DANEJOE_LOG_ERROR("default", "WindowMain", "Failed to open video file : {}", path);
    }

    m_frame_ptr.init(400, 300, AV_PIX_FMT_YUV420P);

    /// @brief 初始化帧
    m_frame->size = m_video_frame_size;
    m_frame->data = std::vector<uint8_t>(m_frame->size.x * m_frame->size.y * 4);
    m_frame->is_valid = true;

    startTimer(1000 / 25);
}

void WindowMain::closeEvent(QCloseEvent* event)
{
    frame_queue.close();
}

void WindowMain::resizeEvent(QResizeEvent* event)
{
    auto s1 = m_label_sdl_1->contentsRect().size();

    if (!m_renderer_1)
    {
        return;
    }

    m_renderer_1->update_window_size({ s1.width(), s1.height() });
    m_renderer_1->set_dest_pos({ 0, 0 });
    // m_renderer_1->set_dest_size({ s1.width(), s1.height() });
    m_renderer_1->set_dest_size({ -1, -1 });

    m_label_sdl_1->update();
}

void WindowMain::showEvent(QShowEvent* event)
{
    /// @brief 延后创建 SDL 渲染器到窗口显示后（避免在控件未显示时使用不稳定的 winId()）
    DANEJOE_LOG_TRACE("default", "MainWindow", "Into showEvent");
    QMainWindow::showEvent(event);
    if (m_renderer_1) return; // 只初始化一次

    DaneJoe::ILogger::LoggerConfig config;
    config.log_name = "sdl_renderer_1";
    config.log_path = "./log/renderer_1.log";

    m_renderer_1 = std::make_shared <SDLFrameRenderer>(config);

    DaneJoe::Size<int> size = {
        m_label_sdl_1->size().width(), m_label_sdl_1->size().height() };

    if (m_renderer_1->set_window(
        "sdl_1",
        size,
        (void*)m_label_sdl_1->winId()
        // nullptr
    ))
    {
        DANEJOE_LOG_TRACE("default", "MainWindow", "init sdl_1 success");
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init sdl_1 failed");
    }

    if (m_renderer_1->set_dest_area({ 0,0 }, size))
    {
        DANEJOE_LOG_TRACE("default", "MainWindow", "set dest area success");
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "set dest area failed");
    }

    /// @brief 初始化原始帧大小
    m_renderer_1->set_raw_frame_size(size);
    if (m_renderer_1->init(IFrameRenderer::FrameFmt::YUV420P))
    {
        DANEJOE_LOG_TRACE("default", "MainWindow", "init renderer success");
    }
    else
    {
        DANEJOE_LOG_ERROR("default", "MainWindow", "init renderer failed");
    }
}

void WindowMain::timerEvent(QTimerEvent* event)
{
    if (!m_renderer_1)
    {
        return;
    }
    if (m_renderer_1->is_exit())
    {
        this->close();
    }
    auto yuv_file_test = [this]()
        {
            /// @note YUV420P Y:U:V = 4:1:1 
            /// @note frame_size = width * height * (4+1+1)/4
#define USE_AVFRAME
#ifndef USE_AVFRAME
            m_frame->fmt = IFrameRenderer::FrameFmt::YUV420P;
            m_frame->data.resize(m_video_frame_size.x * m_video_frame_size.y * 3 / 2);
            m_frame->pitch = m_video_frame_size.x;
            m_video_file.read(
                /// @brief 帧内数据
                (char*)m_frame->data.data(),
                /// @brief 帧大小
                m_video_frame_size.x * m_video_frame_size.y * 3 / 2
            );
#else
            // m_video_file.read(
            //     /// @brief 帧内数据
            //     (char*)m_frame_ptr->data[0],
            //     /// @brief 帧大小
            //     m_video_frame_size.x * m_video_frame_size.y * 3 / 2
            // );


            m_video_file.read(
                /// @brief 帧内数据
                (char*)(m_frame_ptr->data[0]),
                /// @brief 帧大小
                m_video_frame_size.x * m_video_frame_size.y
            );
            m_video_file.read(
                /// @brief 帧内数据
                (char*)(m_frame_ptr->data[1]),
                /// @brief 帧大小
                m_video_frame_size.x * m_video_frame_size.y / 4
            );
            m_video_file.read(
                /// @brief 帧内数据
                (char*)(m_frame_ptr->data[2]),
                /// @brief 帧大小
                m_video_frame_size.x * m_video_frame_size.y / 4
            );
#endif
            if (m_video_file.eof())
            {
                DANEJOE_LOG_TRACE("default", "Play", "video file eof");
                m_video_file.seekg(0);
            }
            else
            {
                if (!m_video_file.good())
                {
                    m_video_file.clear(); // 清除 EOF 状态
                    m_video_file.seekg(0);
                }
            }
        };
    yuv_file_test();
    {
        auto data = frame_queue.try_pop();
        if (data.has_value())
        {
            auto frame = data.value();
            bool draw_1 = m_renderer_1->draw(frame);
            if (!draw_1)
            {
                DANEJOE_LOG_ERROR("default", "MainWindow", "{}", frame.get_error().message());
            }
        }
        else
        {
            DANEJOE_LOG_ERROR("default", "MainWindow", "Invalid frame");
        }
    }
}

WindowMain::~WindowMain()
{
}

void WindowMain::sleep(std::chrono::milliseconds ms)
{
    auto time = std::chrono::high_resolution_clock::now();
    std::chrono::_V2::system_clock::time_point end_time = time + ms;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        time = std::chrono::high_resolution_clock::now();
    } while (time < end_time);
}