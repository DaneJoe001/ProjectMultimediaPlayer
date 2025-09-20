#include <iostream>
#include <memory>
#include <cstdint>
#include <thread>

#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>
#include <QDebug>

#include <SDL2/SDL.h>

#include "view/sdl_video_widget.hpp"
#include "log/manage_logger.hpp"
#include "main/decode_mp4.hpp"

#define LOG_LEVEL 0
#define CLEAR_LOG_FILE 1

void init_logger();

int main(int argc, char* argv[])
{
    init_logger();

    QApplication a(argc, argv);
    SDLVideoWidget w;
    w.init();
    auto frame_queue = w.get_frame_queue();
    std::jthread decode_thread(decode_mp4, "/home/danejoe001/personal_code/code_cpp_project/cpp_project_multimedia/resource/400_300_25.mp4", frame_queue);
    w.show();
    return a.exec();
}


void init_logger()
{
    DaneJoe::ILogger::LoggerConfig config;
#if LOG_LEVEL == 0
    config.file_level = DaneJoe::ILogger::LogLevel::TRACE;
    config.console_level = DaneJoe::ILogger::LogLevel::TRACE;
#elif LOG_LEVEL == 1
    config.file_level = DaneJoe::ILogger::LogLevel::DEBUG;
    config.console_level = DaneJoe::ILogger::LogLevel::DEBUG;
#elif LOG_LEVEL == 2
    config.file_level = DaneJoe::ILogger::LogLevel::INFO;
    config.console_level = DaneJoe::ILogger::LogLevel::INFO;
#elif LOG_LEVEL == 3
    config.file_level = DaneJoe::ILogger::LogLevel::WARN;
    config.console_level = DaneJoe::ILogger::LogLevel::WARN;
#elif LOG_LEVEL == 4
    config.file_level = DaneJoe::ILogger::LogLevel::ERROR;
    config.console_level = DaneJoe::ILogger::LogLevel::ERROR;
#elif LOG_LEVEL == 5
    config.file_level = DaneJoe::ILogger::LogLevel::FATAL;
    config.console_level = DaneJoe::ILogger::LogLevel::FATAL;
#endif

    DaneJoe::ManageLogger::get_instance().get_logger("default")->set_config(config);

#if CLEAR_LOG_FILE==1
    std::ofstream log_file("./log/default.log");
    log_file.clear();
    log_file.close();
#endif
}