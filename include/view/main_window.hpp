#pragma once

#include <thread>

#include <QMainWindow>

class SDLVideoWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void init();
private:
    std::jthread m_decode_thread;
    SDLVideoWidget* m_video_widget;
};