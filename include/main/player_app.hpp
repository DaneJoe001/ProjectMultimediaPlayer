#pragma once

#include <QObject>

#include "service/media_decode_service.hpp"

class MainWindow;

class PlayerApp : public QObject
{
    Q_OBJECT

public:
    PlayerApp(QObject* parent = nullptr);
    ~PlayerApp();
    void init();
    void show_main_window();
private:
    void clear_logger();
    void init_logger();
    void clear_database();
    void init_database();
private:
    MainWindow* m_main_window = nullptr;
    MediaDecodeService* m_media_decode_service = nullptr;
};