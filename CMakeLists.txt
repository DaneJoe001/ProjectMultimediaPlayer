cmake_minimum_required(VERSION 3.20)

project(cpp_exercise_qt_sdl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_target(update_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json")
endif()

file(GLOB SOURCES 
    "include/view/*.hpp"
    "source/renderer/*.cpp" 
    "source/util/*.cpp"
    "source/codec/*.cpp"
    "source/view/*.cpp"
    "source/main/*.cpp"  
    "resource/*.qrc")

# 添加编码设置
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include(FetchContent)
FetchContent_Declare(
    DaneJoeStringify
    GIT_REPOSITORY https://github.com/DaneJoe001/DaneJoeStringify.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(DaneJoeStringify)

FetchContent_Declare(
    DaneJoeLogger
    GIT_REPOSITORY https://github.com/DaneJoe001/DaneJoeLogger.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(DaneJoeLogger)

FetchContent_Declare(
    DaneJoeConcurrent
    GIT_REPOSITORY https://github.com/DaneJoe001/DaneJoeConcurrent.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(DaneJoeConcurrent)

find_package(Qt6 COMPONENTS Core Widgets OpenGL OpenGLWidgets REQUIRED)
message(STATUS "Qt6 found:" ${Qt6_FOUND})
find_package(SDL2 CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Core 
    Qt6::Widgets
    Qt6::OpenGL 
    Qt6::OpenGLWidgets
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    danejoe::stringify
    danejoe::logger
    danejoe::concurrent
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})


