cmake_minimum_required(VERSION 4.0.1)

project(cpp_exercise_qt_sdl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB SOURCES 
    "source/renderer/*.cpp" 
    "source/util/*.cpp"
    "source/codec/*.cpp"
    "source/view/*.cpp"
    "source/main/*.cpp"  
    "resource/*.qrc")

find_package(Qt6 COMPONENTS Core Widgets OpenGL OpenGLWidgets REQUIRED)
message(STATUS "Qt6 found:" ${Qt6_FOUND})
find_package(SDL2 CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCES})

set(MOC_HEADERS
    include/main/window_main.hpp
    include/main/window_main_opengl.hpp
    include/view/qt_video_widget.hpp
)

qt6_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${MOC_SOURCES})

# 指定二进制目录
add_subdirectory(/home/danejoe001/personal_code/code_cpp_library/library_danejoe_log ${CMAKE_BINARY_DIR}/external/danejoe_log)

add_subdirectory(/home/danejoe001/personal_code/code_cpp_library/library_danejoe_mt_queue ${CMAKE_BINARY_DIR}/external/danejoe_mt_queue)


target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Core 
    Qt6::Widgets
    Qt6::OpenGL 
    Qt6::OpenGLWidgets
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})

# 我的日志库
target_link_libraries(${PROJECT_NAME} PRIVATE DaneJoeLog)
target_link_libraries(${PROJECT_NAME} PRIVATE DaneJoeMTQueue)

