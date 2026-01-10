cmake_minimum_required(VERSION 3.20)

# @brief 项目名称
set(PROJECT_NAME TemplateProject)
# @brief 核心库名称
set(CORE_LIB_NAME TemplateCore)
# @brief 可执行文件名称
set(EXECUTABLE_NAME executable)

project(${PROJECT_NAME} LANGUAGES CXX)

include("${CMAKE_SOURCE_DIR}/cmake/options.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/project_options.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/warnings.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/source_collection.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/entry_points.cmake")

add_library(${CORE_LIB_NAME} ${CORE_SOURCES})
target_include_directories(${CORE_LIB_NAME} PUBLIC "include")
apply_warnings(${CORE_LIB_NAME})

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCE} ${QRC_SOURCES})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${CORE_LIB_NAME})
apply_warnings(${EXECUTABLE_NAME})

include("${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake")

if(BUILD_TEST)
    # @brief 启用测试
    include(CTest)
    enable_testing()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
      # @brief 添加测试子目录
      add_subdirectory(test)
    endif()
endif()
