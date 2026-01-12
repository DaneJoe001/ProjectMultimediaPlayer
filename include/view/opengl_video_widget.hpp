/**
 * @file opengl_video_widget.hpp
 * @brief OpenGL视频组件
 * @author DaneJoe001
 * @date 2026-01-12
 */

#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>

#include <danejoe/logger/logger_manager.hpp>

/**
 * @class OpenGLVideoWidget
 * @brief OpenGL视频组件
 */
class OpenGLVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent Qt父对象
     */
    OpenGLVideoWidget(QWidget* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~OpenGLVideoWidget();
protected:
    /**
     * @brief 初始化OpenGL
     */
    void initializeGL() override;
    /**
     * @brief 绘制
     */
    void paintGL() override;
    /**
     * @brief 尺寸改变事件
     * @param w 宽度
     * @param h 高度
     */
    void resizeGL(int w, int h) override;
private:
    /// @brief shader program
    QOpenGLShaderProgram m_program;
};