#include <QApplication>

#include <SDL2/SDL_main.h>
#include <danejoe/logger/logger_manager.hpp>

#include "main/player_app.hpp"

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);
    PlayerApp app;
    app.init();
    app.show_main_window();
    return a.exec();
}