#include <iostream>
#include <memory>
#include <cstdint>
#include <thread>

#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>
#include <SDL2/SDL.h>

#include "main/window_main.h"
#include "log/manage_logger.hpp"
#include "main/decode_mp4.hpp"

int main(int argc, char* argv[])
{

    std::jthread decode_thread(decode_mp4, "/home/danejoe001/personal_code/code_cpp_project/cpp_project_multimedia/resource/400_300_25.mp4");
    QApplication a(argc, argv);
    WindowMain w;
    w.show();
    return a.exec();
}
