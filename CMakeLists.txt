cmake_minimum_required(VERSION 3.20)

# @brief 项目名称
set(PROJECT_NAME TemplateProject)
# @brief 核心库名称
set(CORE_LIB_NAME TemplateCore)
# @brief console 可执行文件名称
set(CONSOLE_EXECUTABLE_NAME executable_console)
# @brief GUI 可执行文件名称
set(GUI_EXECUTABLE_NAME executable_gui)

project(${PROJECT_NAME} LANGUAGES CXX)

include("${CMAKE_SOURCE_DIR}/cmake/options.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/project_options.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/warnings.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/source_collection.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/entry_points.cmake")

add_library(${CORE_LIB_NAME} ${CORE_SOURCES})
target_include_directories(${CORE_LIB_NAME} PUBLIC "include")
apply_warnings(${CORE_LIB_NAME})
apply_static_analysis(${CORE_LIB_NAME})

if(BUILD_CONSOLE_APP)
    if(EXISTS "${CONSOLE_EXECUTABLE_SOURCE}")
        add_executable(${CONSOLE_EXECUTABLE_NAME} ${CONSOLE_EXECUTABLE_SOURCE})
        target_link_libraries(${CONSOLE_EXECUTABLE_NAME} PRIVATE ${CORE_LIB_NAME})
        apply_warnings(${CONSOLE_EXECUTABLE_NAME})
        apply_static_analysis(${CONSOLE_EXECUTABLE_NAME})
    else()
        message(STATUS "Console entry is enabled but not provided by the Player project")
    endif()
endif()

if(BUILD_GUI_APP)
    add_executable(${GUI_EXECUTABLE_NAME} ${GUI_EXECUTABLE_SOURCE} ${QRC_SOURCES})
    target_link_libraries(${GUI_EXECUTABLE_NAME} PRIVATE ${CORE_LIB_NAME})
    apply_warnings(${GUI_EXECUTABLE_NAME})
    apply_static_analysis(${GUI_EXECUTABLE_NAME})
endif()

include("${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake")

if(BUILD_TEST)
    # @brief 启用测试
    include(CTest)
    enable_testing()
endif()

if(BUILD_TEST OR BUILD_BENCHMARK)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
      # @brief 添加测试子目录
      add_subdirectory(test)
    endif()
endif()
