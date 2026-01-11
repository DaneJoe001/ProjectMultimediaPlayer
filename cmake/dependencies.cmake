if(ADD_QT_LIB)
    # @brief Qt6 组件列表
    set(QT_COMPONENTS "Core;Gui;Widgets;Sql;Network;OpenGL;OpenGLWidgets" CACHE STRING "Qt6 components to find and link (semicolon-separated list)")
    find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})
    if(TARGET ${CORE_LIB_NAME})
        set_target_properties(${CORE_LIB_NAME} PROPERTIES
            AUTOMOC ON
            AUTORCC ON
            AUTOUIC ON
        )
    endif()
    if(TARGET ${EXECUTABLE_NAME})
        set_target_properties(${EXECUTABLE_NAME} PROPERTIES
            AUTOMOC ON
            AUTORCC ON
            AUTOUIC ON
        )
    endif()
    # @brief 链接 Qt6 组件
    foreach(qt_component IN LISTS QT_COMPONENTS)
        target_link_libraries(${CORE_LIB_NAME} PUBLIC Qt6::${qt_component})
    endforeach()
endif()

if(ADD_DANEJOE_LIB)
    find_package(DaneJoe 0.2.0 CONFIG REQUIRED COMPONENTS Common Logger Database Concurrent Stringify)
        target_link_libraries(${CORE_LIB_NAME} PUBLIC
        DaneJoe::Common
        DaneJoe::Logger
        DaneJoe::Database
        DaneJoe::Condition
        DaneJoe::Concurrent
        DaneJoe::Stringify
    )
endif()

find_package(SDL2 CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)

if(MSVC AND DEFINED FFMPEG_LIBRARIES)
    set(_ffmpeg_libraries_tmp "${FFMPEG_LIBRARIES}")
    list(FILTER _ffmpeg_libraries_tmp EXCLUDE REGEX "^-l")
    list(FILTER _ffmpeg_libraries_tmp EXCLUDE REGEX "pthread")
    set(FFMPEG_LIBRARIES "${_ffmpeg_libraries_tmp}")
    unset(_ffmpeg_libraries_tmp)
endif()

if(DEFINED FFMPEG_LIBRARY_DIR)
    target_link_directories(${CORE_LIB_NAME} PUBLIC "${FFMPEG_LIBRARY_DIR}")
endif()

if(DEFINED FFMPEG_LIBRARY_DIRS)
    target_link_directories(${CORE_LIB_NAME} PUBLIC ${FFMPEG_LIBRARY_DIRS})
endif()

if(MSVC)
    set(_ffmpeg_library_search_dirs)
    if(DEFINED FFMPEG_LIBRARY_DIR)
        list(APPEND _ffmpeg_library_search_dirs "${FFMPEG_LIBRARY_DIR}")
    endif()
    if(DEFINED FFMPEG_LIBRARY_DIRS)
        list(APPEND _ffmpeg_library_search_dirs ${FFMPEG_LIBRARY_DIRS})
    endif()
    if(_ffmpeg_library_search_dirs)
        find_library(SWRESAMPLE_LIBRARY NAMES swresample PATHS ${_ffmpeg_library_search_dirs} NO_DEFAULT_PATH)
    endif()
    unset(_ffmpeg_library_search_dirs)
endif()

target_link_libraries(${CORE_LIB_NAME} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

if(MSVC AND DEFINED AVCODEC_LIBRARY AND DEFINED AVFORMAT_LIBRARY AND DEFINED AVUTIL_LIBRARY)
    target_link_libraries(${CORE_LIB_NAME} PRIVATE
        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
    )
    if(DEFINED SWRESAMPLE_LIBRARY)
        target_link_libraries(${CORE_LIB_NAME} PRIVATE
            ${SWRESAMPLE_LIBRARY}
        )
    endif()
else()
    target_link_libraries(${CORE_LIB_NAME} PRIVATE ${FFMPEG_LIBRARIES})
endif()