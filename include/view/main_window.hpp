#pragma once

#include <QMainWindow>

#include "model/session_frame.hpp"

class SDLFrameWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void init();
public slots:
    void on_frame_ready(SessionFrame frame);
private:
    SDLFrameWidget* m_frame_widget = nullptr;
};