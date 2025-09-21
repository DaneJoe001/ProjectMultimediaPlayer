#include "main/decode_mp4.hpp"
#include "view/main_window.hpp"
#include "view/sdl_video_widget.hpp"
#include "log/manage_logger.hpp"

MainWindow::MainWindow(QWidget* parent) :QMainWindow(parent)
{
}
MainWindow::~MainWindow()
{
    if (m_video_widget)
    {
        delete m_video_widget;
        m_video_widget = nullptr;
    }
}

void MainWindow::init()
{
    m_video_widget = new SDLVideoWidget(this);
    m_video_widget->init();
    auto frame_queue = m_video_widget->get_frame_queue();
    m_decode_thread = std::move(std::jthread(decode_mp4, "/home/danejoe001/personal_code/code_cpp_project/cpp_project_multimedia/resource/400_300_25.mp4", frame_queue));
    DANEJOE_LOG_TRACE("default", "MainWindow", "init");
    setCentralWidget(m_video_widget);
}