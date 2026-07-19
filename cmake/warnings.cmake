# @brief 对指定 target 应用统一的告警策略
# @param target_name 需要应用编译告警选项的目标名（target）
function(apply_warnings target_name)
    # @note 如果未启用警告，则返回
    if(NOT ENABLE_WARNINGS)
        return()
    endif()
    if(MSVC)
        # @brief 设置编译选项为 /W4
        target_compile_options(${target_name} PRIVATE /W4)
        # @brief 如果将警告视为错误，则设置编译选项为 /WX
        if(ENABLE_WARNINGS_AS_ERRORS)
            # @brief 设置编译选项为 /WX
            target_compile_options(${target_name} PRIVATE /WX)
        endif()
    else()
        # @brief 设置编译选项为 -Wall -Wextra -Wpedantic
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(ENABLE_WARNINGS_AS_ERRORS)
            # @brief 设置编译选项为 -Werror
            target_compile_options(${target_name} PRIVATE -Werror)
        endif()
    endif()
endfunction()

# @brief 对指定 target 应用编译器原生静态分析选项
# @param target_name 需要应用静态分析选项的目标名（target）
function(apply_static_analysis target_name)
    if(ENABLE_MSVC_ANALYZE AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target_name} PRIVATE /analyze)
    endif()
endfunction()
