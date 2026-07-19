if(BUILD_GUI_APP AND NOT USE_QT)
    message(FATAL_ERROR "BUILD_GUI_APP=ON requires USE_QT=ON for the Player GUI")
endif()

set(CONSOLE_EXECUTABLE_SOURCE "${CMAKE_SOURCE_DIR}/source/main/console_main.cpp")
set(GUI_EXECUTABLE_SOURCE "${CMAKE_SOURCE_DIR}/source/main/widget_main.cpp")
