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

#include "egl_manager.h"

#include <sstream>
#include <string>

#include "window.h"
#include "drawing_utils.h"

namespace OHOS {
namespace Rosen {
EGLManager::EGLManager() noexcept
    : nativeWindow_(nullptr),
      eglDisplay_(EGL_NO_DISPLAY),
      eglContext_(EGL_NO_CONTEXT),
      eglSurface_(EGL_NO_SURFACE),
      eglConfig_(nullptr)
{
}

EGLManager::~EGLManager()
{
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        return;
    }
    eglDestroyContext(eglDisplay_, eglContext_);
    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(eglDisplay_);
    eglReleaseThread();

    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
    eglSurface_ = EGL_NO_SURFACE;
}

using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr const char* EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char* EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char* CHARACTER_STRING_WHITESPACE = " ";
constexpr const char* EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";

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

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void* native_display, const EGLint* attrib_list)
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
        return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
    }

    return eglGetDisplay((EGLNativeDisplayType)native_display);
}

void EGLManager::Init()
{
    if (IsEglContextReady()) {
        return;
    }

    LOGI("Start to creating EGLContext!!!");
    eglDisplay_ = GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        LOGW("Failed to create EGLDisplay gl errno : %{public}x", eglGetError());
        return;
    }

    EGLint major, minor;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        LOGE("Failed to initialize EGLDisplay");
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        LOGE("Failed to bind OpenGL ES API");
        return;
    }

    EGLint count;
    EGLint config_attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    EGLBoolean ret = eglChooseConfig(eglDisplay_, config_attribs, &eglConfig_, 1, &count);
    if (!(ret && count >= 1)) {
        LOGE("Failed to eglChooseConfig");
        return;
    }

    static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    eglContext_ = eglCreateContext(eglDisplay_, eglConfig_, EGL_NO_CONTEXT, context_attribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        LOGE("Failed to create egl context %{public}x", eglGetError());
        return;
    }

    LOGW("Create EGL context successfully, version %{public}d.%{public}d", major, minor);
}

bool EGLManager::IsEglContextReady()
{
    return (eglContext_ != EGL_NO_DISPLAY);
}

EGLSurface EGLManager::CreateSurface(EGLNativeWindowType eglNativeWindow)
{
    if (!IsEglContextReady()) {
        LOGE("EGL context has not initialized");
        return EGL_NO_SURFACE;
    }
    nativeWindow_ = eglNativeWindow;

    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    EGLint winAttribs[] = { EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE };
    EGLSurface surface = eglCreateWindowSurface(eglDisplay_, eglConfig_, nativeWindow_, winAttribs);
    if (surface == EGL_NO_SURFACE) {
        LOGW("Failed to create eglsurface!!! %{public}x", eglGetError());
        return EGL_NO_SURFACE;
    }

    LOGW("CreateEGLSurface: %{public}p", surface);

    eglSurface_ = surface;
    return surface;
}

void EGLManager::DestroySurface()
{
    if (!eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext_)) {
        LOGE("Failed to make current on surface %{public}p, error=%{public}x", EGL_NO_SURFACE, eglGetError());
    }

    if (!eglDestroySurface(eglDisplay_, eglSurface_)) {
        LOGE("Failed to destroy surface %{public}p, error=%{public}x", (void*)eglSurface_, eglGetError());
    }
}

void EGLManager::MakeCurrent()
{
    if (!eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_)) {
        LOGE("Failed to make current on surface %{public}p, error=%{public}x", eglSurface_, eglGetError());
    }
}

void EGLManager::SwapBuffers()
{
    if (!eglSwapBuffers(eglDisplay_, eglSurface_)) {
        LOGE("Failed to SwapBuffers on surface %{public}p, error=%{public}x", eglSurface_, eglGetError());
    } else {
        LOGW("SwapBuffers successfully, surface is %{public}p", eglSurface_);
    }
}

void EGLManager::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
}
}
}