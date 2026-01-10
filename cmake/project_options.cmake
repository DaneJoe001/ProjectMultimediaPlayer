set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# @brief 仅在会生成 compile_commands.json 的生成器下复制（Ninja/Makefiles）
if(CMAKE_EXPORT_COMPILE_COMMANDS AND CMAKE_GENERATOR MATCHES "Ninja|Makefiles")
  add_custom_target(update_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json")
endif()

if(MSVC)
    # @brief 启用 UTF-8 编码
    add_compile_options(/utf-8)
    # @brief 禁用 CRT 安全警告
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()
