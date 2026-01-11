#pragma once

#include <QMainWindow>

#include "model/session_frame.hpp"

class QMenuBar;
class QMenu;
class QAction;
class SDLFrameWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void init();
    void closeEvent(QCloseEvent* event) override;
signals:
    void window_close();
    void open_media_file(QString media_file_path);
public slots:
    void on_frame_ready(SessionFrame frame);
    void on_open_action_triggered();
private:
    SDLFrameWidget* m_frame_widget = nullptr;
    QMenuBar* m_menu_bar = nullptr;
    QMenu* m_file_menu = nullptr;
    QAction* m_open_action = nullptr;
};