#pragma once

#include <memory>

#include <QObject>
#include <QThread>
#include <QPointer>

#include "service/media_decode_service.hpp"
#include "worker/sdl_audio_worker.hpp"
#include "controller/media_controller.hpp"
#include "service/time_service.hpp"

class MainWindow;

class PlayerApp : public QObject
{
    Q_OBJECT

public:
    PlayerApp(QObject* parent = nullptr);
    ~PlayerApp();
    void init();
    void show_main_window();
signals:
    void stop_audio();
private:
    void clear_logger();
    void init_logger();
    void clear_database();
    void init_database();
public slots:
    void on_open_midea_file(QString video_file_path);
    void on_window_close();
private:
    MediaDecodeService* m_media_decode_service = nullptr;
    MainWindow* m_main_window = nullptr;
    SDLAudioWorker* m_sdl_audio_worker = nullptr;
    QThread* m_audio_thread = nullptr;
    QPointer<MediaController> m_media_controller = nullptr;
    std::shared_ptr<TimeService> m_time_service = nullptr;
};