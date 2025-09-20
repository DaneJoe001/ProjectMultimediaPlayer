#pragma once

#include <memory>
#include <cstdint>
#include <fstream>
#include <chrono>

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include "view/qt_video_widget.hpp"


/**
 * @class WindowMain
 * @brief 主窗口
 * @note 主窗口
  */
class WindowMainOpenGL :public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit WindowMainOpenGL(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     * @note 释放资源
     */
    ~WindowMainOpenGL();
private:
private:
    QtVideoWidget* m_video_widget;
};
