#include <fstream>

#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/logger/logger_config.hpp>
#include <QCoreApplication>

#include "view/main_window.hpp"
#include "main/player_app.hpp"

PlayerApp::PlayerApp(QObject* parent) :QObject(parent)
{}
PlayerApp::~PlayerApp()
{
    m_audio_thread->quit();
    m_audio_thread->wait();
    if (m_main_window)
    {
        m_main_window->deleteLater();
    }
    if (m_sdl_audio_worker)
    {
        delete m_sdl_audio_worker;
    }
}
void PlayerApp::init()
{
    clear_logger();
    init_logger();
    m_main_window = new MainWindow();
    m_main_window->init();
    m_time_service = std::make_shared<TimeService>();
    m_media_controller = new MediaController(m_time_service, this);
    m_media_controller->init();
    m_media_decode_service = new MediaDecodeService(m_media_controller, this);
    m_media_decode_service->init();
    m_sdl_audio_worker = new SDLAudioWorker(m_time_service);
    m_audio_thread = new QThread(this);
    m_sdl_audio_worker->moveToThread(m_audio_thread);

    connect(m_audio_thread, &QThread::started, m_sdl_audio_worker, &SDLAudioWorker::on_init);
    connect(m_main_window, &MainWindow::window_close, this, &PlayerApp::on_window_close);
    m_audio_thread->start();

    connect(m_media_controller, &MediaController::renderer_audio_frame, m_sdl_audio_worker, &SDLAudioWorker::on_frame_ready);
    connect(m_media_controller, &MediaController::renderer_video_frame,
        m_main_window, &MainWindow::on_frame_ready);

    connect(m_main_window, &MainWindow::open_media_file, this,
        &PlayerApp::on_open_midea_file);
    connect(m_media_controller, &MediaController::paused_decode, m_media_decode_service, &MediaDecodeService::on_paused_decode);


    // on_open_midea_file("E:/personal_code/code_cpp_project/cpp_project_multimedia/resource/demon_slayer_brother_sister_bond.mp4");
}

void PlayerApp::on_window_close()
{
    if (m_media_controller && m_sdl_audio_worker)
    {
        disconnect(m_media_controller, &MediaController::renderer_audio_frame,
            m_sdl_audio_worker, &SDLAudioWorker::on_frame_ready);
    }
    if (m_sdl_audio_worker)
    {
        m_sdl_audio_worker->request_stop();
        QMetaObject::invokeMethod(
            m_sdl_audio_worker,
            "on_stop_audio",
            Qt::BlockingQueuedConnection);
    }
    QCoreApplication::quit();
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
    // output_setting.enable_function_name = true;
    output_setting.enable_line_num = true;
    output_setting.enable_thread_id = true;
    logger->set_output_settings(output_setting);
}
void PlayerApp::clear_database()
{

}
void PlayerApp::init_database()
{

}

void PlayerApp::on_open_midea_file(QString video_file_path)
{
    if (m_sdl_audio_worker)
    {
        m_sdl_audio_worker->request_stop();
        QMetaObject::invokeMethod(
            m_sdl_audio_worker,
            "on_stop_audio",
            Qt::BlockingQueuedConnection);
        QMetaObject::invokeMethod(
            m_sdl_audio_worker,
            "on_start_audio",
            Qt::BlockingQueuedConnection);
    }
    m_media_decode_service->on_decode_media_file(video_file_path);
    m_time_service->reset_audio_time();
}