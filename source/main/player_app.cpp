#include <fstream>

#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/logger/logger_config.hpp>

#include "view/main_window.hpp"
#include "main/player_app.hpp"

PlayerApp::PlayerApp(QObject* parent) :QObject(parent)
{}
PlayerApp::~PlayerApp()
{
    if (m_main_window)
    {
        m_main_window->deleteLater();
    }
}
void PlayerApp::init()
{
    clear_logger();
    init_logger();
    m_main_window = new MainWindow();
    m_main_window->init();
    m_media_decode_service = new MediaDecodeService(this);
    m_media_decode_service->init();
    connect(m_media_decode_service, &MediaDecodeService::video_frame_ready,
        m_main_window, &MainWindow::on_frame_ready);
    m_media_decode_service->on_decode_media_file("E:/personal_code/code_cpp_project/cpp_project_multimedia/resource/demon_slayer_brother_sister_bond.mp4");
}

void PlayerApp::show_main_window()
{
    m_main_window->show();
}

void PlayerApp::clear_logger()
{
    std::ofstream log_file("./log/default.log");
    if (log_file.is_open())
    {
        log_file.clear();
        log_file.close();
    }
}
void PlayerApp::init_logger()
{
    DaneJoe::LoggerConfig config;
    config.file_level = DaneJoe::LogLevel::TRACE;
    config.console_level = DaneJoe::LogLevel::TRACE;
    auto logger = DaneJoe::LoggerManager::get_instance().get_logger("default");
    logger->set_config(config);
    DaneJoe::LogOutputSetting output_setting;
    output_setting.enable_function_name = true;
    output_setting.enable_line_num = true;
    logger->set_output_settings(output_setting);
}
void PlayerApp::clear_database()
{

}
void PlayerApp::init_database()
{

}