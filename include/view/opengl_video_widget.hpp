#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "log/manage_logger.hpp"

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