# @brief 构建测试
option(BUILD_TEST "Build tests" OFF)
# @brief 构建 GUI 入口
option(BUILD_GUI_APP "Build GUI application entry" ON)
# @brief 启用编译器警告
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
# @brief 将警告视为错误
option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
# @brief 启用 Qt6 集成
option(ADD_QT_LIB "Enable Qt6 integration" ON)
# @brief 启用 DaneJoe 集成
option(ADD_DANEJOE_LIB "Enable DaneJoe integration" ON)