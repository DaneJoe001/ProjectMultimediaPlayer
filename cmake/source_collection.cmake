file(GLOB MAIN_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/main/*.cpp"
)

list(FILTER MAIN_DIR_SOURCES EXCLUDE REGEX ".*_main\\.cpp$")

set(CORE_SOURCES
    ${MAIN_DIR_SOURCES}
)

file(GLOB NORMAL_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/renderer/*.cpp" 
    "${CMAKE_SOURCE_DIR}/source/util/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/codec/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/status/*.cpp"
)

file(GLOB QOBJECT_DIR_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/include/view/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/service/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/worker/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/main/player_app.hpp"
)

file(GLOB QOBJECT_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/service/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/worker/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/main/player_app.cpp"
)

file(GLOB QRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/resource/*.qrc"
)

list(APPEND CORE_SOURCES
    ${NORMAL_DIR_SOURCES}
)

if(ADD_QT_LIB)
    list(APPEND CORE_SOURCES
        ${QOBJECT_DIR_SOURCES}
    )
    list(APPEND CORE_SOURCES
        ${QOBJECT_DIR_HEADERS}
    )
endif()
