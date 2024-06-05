/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "native_image.h"

#include <cstddef>
#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "graphic_common_c.h"
#include "surface_type.h"
#include "window.h"
#include "GLES/gl.h"

namespace {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr const char* EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char* EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char* CHARACTER_STRING_WHITESPACE = " ";
constexpr const char* EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
EGLDisplay g_eglDisplay = EGL_NO_DISPLAY;
EGLContext g_eglContext = EGL_NO_CONTEXT;
EGLConfig g_config;

static bool CheckEglExtension(const char* extensions, const char* extension)
{
    size_t extlen = strlen(extension);
    const char* end = extensions + strlen(extensions);

    while (extensions < end) {
        size_t n = 0;
        /* Skip whitespaces, if any */
        if (*extensions == CHARACTER_WHITESPACE) {
            extensions++;
            continue;
        }
        n = strcspn(extensions, CHARACTER_STRING_WHITESPACE);
        /* Compare strings */
        if (n == extlen && strncmp(extension, extensions, n) == 0) {
            return true; /* Found */
        }
        extensions += n;
    }
    /* Not found */
    return false;
}

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void* nativeDisplay, const EGLint* attribList)
{
    static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

    if (!eglGetPlatformDisplayExt) {
        const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
                CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt = (GetPlatformDisplayExt)eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT);
        }
    }

    if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, nativeDisplay, attribList);
    }

    return eglGetDisplay((EGLNativeDisplayType)nativeDisplay);
}

void AddBuffer(OHNativeWindow* nativeWindow)
{
    int code = SET_USAGE;
    int32_t usage = OHOS::BUFFER_USAGE_CPU_READ | OHOS::BUFFER_USAGE_CPU_WRITE | OHOS::BUFFER_USAGE_MEM_DMA;
    int32_t ret = NativeWindowHandleOpt(nativeWindow, code, usage);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_USAGE faile" << std::endl;
    }
    code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    ret = NativeWindowHandleOpt(nativeWindow, code, width, height);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_BUFFER_GEOMETRY failed" << std::endl;
    }
    code = SET_STRIDE;
    int32_t stride = 0x8;
    ret = NativeWindowHandleOpt(nativeWindow, code, stride);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_STRIDE failed" << std::endl;
    }
    code = SET_FORMAT;
    int32_t format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    ret = NativeWindowHandleOpt(nativeWindow, code, format);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_FORMAT failed" << std::endl;
    }

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_FORMAT failed" << std::endl;
    }

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    if (ret != GSERROR_OK) {
        std::cout << "NativeWindowHandleOpt SET_FORMAT failed" << std::endl;
    }
    delete region;
}

int32_t GetData(OH_NativeImage* image, OHNativeWindow* nativeWindow)
{
    AddBuffer(nativeWindow);
    int32_t ret = OH_NativeImage_UpdateSurfaceImage(image);
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "OH_NativeImage_UpdateSurfaceImage failed" << std::endl;
        return -1;
    }

    int64_t timeStamp = OH_NativeImage_GetTimestamp(image);
    if (timeStamp == SURFACE_ERROR_ERROR) {
        std::cout << "OH_NativeImage_GetTimestamp failed" << std::endl;
        return -1;
    }

    float matrix[16];
    ret = OH_NativeImage_GetTransformMatrix(image, matrix);
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "OH_NativeImage_GetTransformMatrix failed" << std::endl;
        return -1;
    }
    return SURFACE_ERROR_OK;
}

void InitEglContext()
{
    if (g_eglContext != EGL_NO_DISPLAY) {
        return;
    }

    std::cout << "Creating EGLContext!!!" << std::endl;
    g_eglDisplay = GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL);
    if (g_eglDisplay == EGL_NO_DISPLAY) {
        std::cout << "Failed to create EGLDisplay gl errno : " << eglGetError() << std::endl;
        return;
    }

    EGLint major, minor;
    if (eglInitialize(g_eglDisplay, &major, &minor) == EGL_FALSE) {
        std::cout << "Failed to initialize EGLDisplay" << std::endl;
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        std::cout << "Failed to bind OpenGL ES API" << std::endl;
        return;
    }

    unsigned int ret;
    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(g_eglDisplay, configAttribs, &g_config, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        std::cout << "Failed to eglChooseConfig" << std::endl;
        return;
    }

    static const EGLint CONTEXT_ATTRIBS[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    g_eglContext = eglCreateContext(g_eglDisplay, g_config, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    if (g_eglContext == EGL_NO_CONTEXT) {
        std::cout << "Failed to create egl context %{public}x, error:" << eglGetError() << std::endl;
        return;
    }

    eglMakeCurrent(g_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, g_eglContext);

    std::cout << "Create EGL context successfully, version" << major << "." << minor << std::endl;
}

void Deinit()
{
    if (g_eglDisplay == EGL_NO_DISPLAY) {
        return;
    }
    eglDestroyContext(g_eglDisplay, g_eglContext);
    eglMakeCurrent(g_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(g_eglDisplay);
    eglReleaseThread();

    g_eglDisplay = EGL_NO_DISPLAY;
    g_eglContext = EGL_NO_CONTEXT;
}

}

int32_t main(int32_t argc, const char *argv[])
{
    std::cout << "sample start" << std::endl;
    InitEglContext();
    GLuint textureId;
    glGenTextures(1, &textureId);

    OH_NativeImage* image = OH_NativeImage_Create(textureId, GL_TEXTURE_2D);

    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(image);
    if (nativeWindow == nullptr) {
        std::cout << "OH_NativeImage_AcquireNativeWindow failed" << std::endl;
        return -1;
    }

    int32_t ret = GetData(image, nativeWindow);
    if (ret != SURFACE_ERROR_OK) {
        return -1;
    }

    GLuint textureId2;
    glGenTextures(1, &textureId2);
    ret = OH_NativeImage_AttachContext(image, textureId2);
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "OH_NativeImage_AttachContext textureId2 failed" << std::endl;
        return -1;
    }

    ret = GetData(image, nativeWindow);
    if (ret != SURFACE_ERROR_OK) {
        return -1;
    }

    ret = OH_NativeImage_DetachContext(image);
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "OH_NativeImage_DetachContext failed" << std::endl;
        return -1;
    }

    ret = OH_NativeImage_AttachContext(image, textureId2);
    if (ret != SURFACE_ERROR_OK) {
        std::cout << "OH_NativeImage_AttachContext after OH_NativeImage_DetachContext failed" << std::endl;
        return -1;
    }

    ret = GetData(image, nativeWindow);
    if (ret != SURFACE_ERROR_OK) {
        return -1;
    }

    OH_NativeImage_Destroy(&image);
    if (image != nullptr) {
        std::cout << "OH_NativeImage_Destroy failed" << std::endl;
        return -1;
    }

    Deinit();
    std::cout << "sample end successfully" << std::endl;
    return 0;
}