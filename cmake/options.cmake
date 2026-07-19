# @brief 构建测试
option(BUILD_TEST "Build tests" OFF)
# @brief 构建 benchmark
option(BUILD_BENCHMARK "Build benchmarks" OFF)
# @brief 构建 console 入口
option(BUILD_CONSOLE_APP "Build console application entry" ON)
# @brief 构建 GUI 入口
option(BUILD_GUI_APP "Build GUI application entry" OFF)
# @brief 构建示例入口
option(BUILD_EXAMPLE "Build example targets" OFF)
# @brief 启用编译器警告
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
# @brief 将警告视为错误
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
# @brief 启用 MSVC cl.exe 静态分析
option(ENABLE_MSVC_ANALYZE "Enable MSVC cl.exe /analyze static analysis" OFF)
# @brief 启用 Qt6 集成
option(USE_QT "Enable Qt6 integration" OFF)
# @brief 启用 DaneJoe 集成
option(USE_DANEJOE "Enable DaneJoe integration" OFF)
