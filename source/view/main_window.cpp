#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>

#include <danejoe/logger/logger_manager.hpp>

#include "view/main_window.hpp"
#include "view/sdl_frame_widget.hpp"

MainWindow::MainWindow(QWidget* parent) :QMainWindow(parent)
{}
MainWindow::~MainWindow()
{
    if (m_frame_widget)
    {
        delete m_frame_widget;
        m_frame_widget = nullptr;
    }
}

void MainWindow::init()
{
    setWindowIcon(QIcon(":/image/app_icon.png"));
    setGeometry(200, 200, 640, 480);
    setWindowTitle("DaneJoe MultiMedia Player");
    m_frame_widget = new SDLFrameWidget(this);
    m_frame_widget->init();
    setCentralWidget(m_frame_widget);

    m_menu_bar = new QMenuBar(this);
    this->setMenuBar(m_menu_bar);
    m_file_menu = m_menu_bar->addMenu("File");
    m_open_action = m_file_menu->addAction("Open");

    connect(m_open_action, &QAction::triggered, this, &MainWindow::on_open_action_triggered);

}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMainWindow::closeEvent(event);
    emit window_close();
}

void MainWindow::on_frame_ready(SessionFrame frame)
{
    m_frame_widget->draw(frame.frame);
}

void MainWindow::on_open_action_triggered()
{
    QString media_file_path = QFileDialog::getOpenFileName(this, "Open Media File", ".", "Media Files (*.mp4 *.avi *.mkv *.mp3 *.ts)");
    emit open_media_file(media_file_path);
}