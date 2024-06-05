# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(XComponent)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DOHOS_PLATFORM)

include_directories(
    ${NATIVERENDER_ROOT_PATH}
    ${NATIVERENDER_ROOT_PATH}/include
    common
    manager
    render
)

add_library(nativerender SHARED
    render/egl_core.cpp
    render/plugin_render.cpp
    manager/plugin_manager.cpp
    napi_init.cpp
)

find_library(
    EGL-lib
    EGL
    GLES-lib
    GLESv3
    hilog-lib
    hilog_ndk.z
    libace-lib
    ace_ndk.z
    libnapi-lib
    ace_napi.z
    libuv-lib
    uv
)

target_link_libraries(nativerender PUBLIC
    ${EGL-lib} ${GLES-lib} ${hilog-lib} ${libace-lib} ${libnapi-lib} ${libuv-lib} libc++.a)