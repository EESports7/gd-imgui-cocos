cmake_minimum_required(VERSION 3.31.6)

project(imgui-cocos)

set(FT_DISABLE_VERSIONING ON CACHE BOOL "" FORCE)

add_library(${PROJECT_NAME} STATIC)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hooks.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

if(IMGUI_COCOS_MODULE)
    target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
            ${PROJECT_NAME}/include/imgui-cocos.cppm
    )
endif()

option(HAS_IMGUI "If imgui is already provided externally" OFF)
set(IMGUI_VERSION "v1.92.5" CACHE STRING "What imgui version to download and use")
option(IMGUI_HOOK_EARLY "Hook swapBuffers early, as to show imgui under mod menus. Disable this if you're a mod menu" ON)

if (IMGUI_HOOK_EARLY)
    target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_COCOS_HOOK_EARLY=1)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
#    IMGUI_ENABLE_FREETYPE breaks macos
    IMGUI_USE_STB_SPRINTF
    IMGUI_STB_SPRINTF_FILENAME="stb_sprintf.h"
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

    target_link_libraries(${PROJECT_NAME} PUBLIC freetype)

    target_include_directories(${PROJECT_NAME} PUBLIC
        ${imgui_SOURCE_DIR}
        ${stb_SOURCE_DIR}
    )

    target_sources(${PROJECT_NAME} PUBLIC
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
