/**
 * @file main_window.hpp
 * @brief 主窗口
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <QMainWindow>

#include "model/session_frame.hpp"

class QMenuBar;
class QMenu;
class QAction;
class SDLFrameWidget;

/**
 * @class MainWindow
 * @brief 主窗口
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent Qt父对象
     */
    MainWindow(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~MainWindow();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 关闭事件
     * @param event 事件
     */
    void closeEvent(QCloseEvent* event) override;
signals:
    /**
     * @brief 窗口关闭信号
     */
    void window_close();
    /**
     * @brief 打开媒体文件信号
     * @param media_file_path 媒体文件路径
     */
    void open_media_file(QString media_file_path);
public slots:
    /**
     * @brief 响应帧就绪
     * @param frame 帧
     */
    void on_frame_ready(SessionFrame frame);
    /**
     * @brief 响应打开动作触发
     */
    void on_open_action_triggered();
private:
    /// @brief 帧组件
    SDLFrameWidget* m_frame_widget = nullptr;
    /// @brief 菜单栏
    QMenuBar* m_menu_bar = nullptr;
    /// @brief 文件菜单
    QMenu* m_file_menu = nullptr;
    /// @brief 打开动作
    QAction* m_open_action = nullptr;
};