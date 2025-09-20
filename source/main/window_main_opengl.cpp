#include <iostream>
#include <type_traits>

#include <QDebug>
#include <cstdint>
#include <string>
#include <QMessageBox>
#include <QDebug>

#include "log/i_logger.hpp"
#include "log/manage_logger.hpp"

#include "main/window_main_opengl.hpp"

WindowMainOpenGL::WindowMainOpenGL(QWidget* parent) :QMainWindow(parent)
{
    m_video_widget = new QtVideoWidget(this);
    setGeometry(400, 400, 400, 300);
    m_video_widget->setGeometry(0, 0, 400, 300);
    setCentralWidget(m_video_widget);
}

WindowMainOpenGL::~WindowMainOpenGL()
{
}