cmake_minimum_required(VERSION 3.14)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(imgui-cocos)

set(FT_DISABLE_VERSIONING ON CACHE BOOL "" FORCE)

add_library(imgui-cocos INTERFACE)

set_target_properties(imgui-cocos PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_sources(imgui-cocos INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hooks.cpp
)

target_include_directories(imgui-cocos INTERFACE include)

option(HAS_IMGUI "If imgui is already provided externally" OFF)
set(IMGUI_VERSION "v1.92.4" CACHE STRING "What imgui version to download and use")
option(IMGUI_HOOK_EARLY "Hook swapBuffers early, as to show imgui under mod menus. Disable this if you're a mod menu" ON)

if (IMGUI_HOOK_EARLY)
    target_compile_definitions(imgui-cocos INTERFACE IMGUI_COCOS_HOOK_EARLY=1)
endif()

target_compile_definitions(imgui-cocos INTERFACE
    IMGUI_ENABLE_FREETYPE
    IMGUI_USE_STB_SPRINTF
    IMGUI_STB_SPRINTF_FILENAME="stb/stb_sprintf.h"
)

if (NOT HAS_IMGUI)
    if (NOT COMMAND CPMAddPackage)
        include(cmake/CPM.cmake)
    endif()

    CPMAddPackage("gh:ocornut/imgui#${IMGUI_VERSION}")

    CPMAddPackage(
        NAME freetype
        GITHUB_REPOSITORY freetype/freetype
        GIT_TAG VER-2-14-1
    )

    CPMAddPackage(
        NAME stb
        GITHUB_REPOSITORY nothings/stb
        GIT_TAG 1ee679c
    )

    target_link_libraries(imgui-cocos INTERFACE freetype)

    target_include_directories(imgui-cocos INTERFACE
        ${imgui_SOURCE_DIR}
        ${stb_SOURCE_DIR}
    )

    target_sources(imgui-cocos INTERFACE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
    )
endif()

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(test)
endif()
