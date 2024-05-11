# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MyApplication_glFramebufferTexture3DOES)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_definitions(-DOHOS_PLATFORM)
add_definitions(-DEGL_EGLEXT_PROTOTYPES)
add_definitions(-DGL_GLEXT_PROTOTYPES)

add_library(entry SHARED test_glFramebufferTexture3DOES.cpp)
target_link_libraries(entry PUBLIC libace_napi.z.so libhilog_ndk.z.so libGLESv3.so libGLESv2.so libEGL.so)