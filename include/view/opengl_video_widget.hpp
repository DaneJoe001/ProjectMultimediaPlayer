#pragma once

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>

#include <danejoe/logger/logger_manager.hpp>

class OpenGLVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLVideoWidget(QWidget* parent = nullptr);
    ~OpenGLVideoWidget();
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    /// @brief shader program
    QOpenGLShaderProgram m_program;
};