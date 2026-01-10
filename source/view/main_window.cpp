#include <danejoe/logger/logger_manager.hpp>

#include "view/main_window.hpp"
#include "view/sdl_frame_widget.hpp"

MainWindow::MainWindow(QWidget* parent) :QMainWindow(parent)
{}
MainWindow::~MainWindow()
{
    if (m_frame_widget)
    {
        delete m_frame_widget;
        m_frame_widget = nullptr;
    }
}

void MainWindow::init()
{
    setWindowIcon(QIcon(":/image/profile10.jpg"));
    // setGeometry(200, 200, 1920, 1080);
    m_frame_widget = new SDLFrameWidget(this);
    m_frame_widget->init();
    setCentralWidget(m_frame_widget);
}

void MainWindow::on_frame_ready(SessionFrame frame)
{
    m_frame_widget->draw(frame.frame);
}