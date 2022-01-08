/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "render_context.h"

#include <sstream>
#include <string>

#include "utils/log.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr const char *EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char *EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char *CHARACTER_STRING_WHITESPACE = " ";
constexpr const char *EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";

static bool CheckEglExtension(const char *extensions, const char *extension)
{
    size_t extlen = strlen(extension);
    const char *end = extensions + strlen(extensions);

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

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void *native_display, const EGLint *attrib_list)
{
    static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

    if (!eglGetPlatformDisplayExt) {
        const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
            CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt = (GetPlatformDisplayExt)eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT);
        }
    }

    if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
    }

    return eglGetDisplay((EGLNativeDisplayType) native_display);
}

RenderContext::RenderContext()
    : grContext_(nullptr), skSurface_(nullptr), skCanvas_(nullptr), nativeWindow_(nullptr),
    eglDisplay_(nullptr), eglContext_(nullptr), eglSurface_(nullptr), config_(nullptr)
{
}

RenderContext::~RenderContext()
{
    eglDestroyContext(eglDisplay_, eglContext_);
    eglTerminate(eglDisplay_);
    eglReleaseThread();

    grContext_ = nullptr;
    skSurface_ = nullptr;
    skCanvas_ = nullptr;
}

void RenderContext::InitializeEglContext()
{
    eglDisplay_ = GetPlatformEglDisplay(EGL_PLATFORM_OHOS, EGL_DEFAULT_DISPLAY, NULL);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        LOGW("Failed to create EGLDisplay gl errno : %{public}x", eglGetError());
        return;
    }

    LOGW("EGLDisplay is : %{public}p， rendercontext is %{public}p", eglDisplay_, this);

    EGLint major, minor;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        LOGW("Failed to initialize EGLDisplay");
        return;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        LOGW("Failed to bind OpenGL ES API");
        return;
    }

    EGLint ret, count;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    ret = eglChooseConfig(eglDisplay_, config_attribs, &config_, 1, &count);
    if (!(ret && count >= 1)) {
        LOGW("Failed to eglChooseConfig");
        return;
    }

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM,
        EGL_NONE
    };

    if (eglContext_ == EGL_NO_CONTEXT) {
        eglContext_ = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
        if (eglContext_ == EGL_NO_CONTEXT) {
            LOGW("CreateEGLSurface failed to eglCreateContext %{public}x", eglGetError());
            return;
        }
    }
    LOGW("InitializeEglContext finished");
}

void RenderContext::MakeCurrent(EGLSurface surface) const
{
    if ((eglDisplay_ != nullptr) && (eglContext_ != EGL_NO_CONTEXT) && (surface != nullptr)) {
        LOGW("MakeCurrent successfully, context is %{public}p", this);
        eglMakeCurrent(eglDisplay_, surface, surface, eglContext_);
    } else {
        LOGE("Can not makeCurrent!!!");
    }
}

void RenderContext::SwapBuffers(EGLSurface surface) const
{
    if ((eglDisplay_ != nullptr) && (surface != nullptr)) {
        LOGW("SwapBuffers successfully, context is %{public}p", this);
        eglSwapBuffers(eglDisplay_, surface);
    } else {
        LOGW("Can not SwapBuffers!!!");
    }
}

EGLSurface RenderContext::CreateEGLSurface(EGLNativeWindowType eglNativeWindow)
{
    nativeWindow_ = eglNativeWindow;

    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    EGLint winAttribs[] = {EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};
    EGLSurface surface = eglCreateWindowSurface(eglDisplay_, config_, nativeWindow_, winAttribs);
    if (surface == EGL_NO_SURFACE) {
        LOGW("CreateEGLSurface failed to create eglsurface %{public}x", eglGetError());
        return nullptr;
    }

    LOGW("CreateEGLSurface: eglDisplay_ is %{public}p, eglSurface_ is %{public}p", eglDisplay_, surface);

    eglSurface_ = surface;
    return surface;
}

static void SetUpGrContext(sk_sp<GrContext>& grContext)
{
    if (grContext != nullptr) {
        LOGW("grContext has already created!!");
        return;
    }

    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (glInterface == nullptr) {
        LOGW("SetUpGrContext failed to make native interface");
        return;
    }

    GrContextOptions options;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    grContext = GrContext::MakeGL(std::move(glInterface), options);
    if (grContext == nullptr) {
        LOGE("SetUpGrContext grContext is null");
        return;
    }

    LOGI("SetUpGrContext successfully!");
}

void RenderContext::CreateCanvas(int width, int height)
{
    if (skCanvas_ != nullptr) {
        LOGW("canvas has been created!!");
        return;
    }

    SetUpGrContext(grContext_);

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    GrBackendRenderTarget backendRenderTarget(width, height, 0, 8, framebufferInfo);
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;

    skSurface_ = SkSurface::MakeFromBackendRenderTarget(grContext_.get(), backendRenderTarget,
        kBottomLeft_GrSurfaceOrigin, colorType, nullptr, &surfaceProps);
    if (skSurface_ == nullptr) {
        LOGW("skSurface is nullptr");
        return;
    }

    skCanvas_ = skSurface_->getCanvas();
    if (skCanvas_ == nullptr) {
        LOGE("CreateCanvas failed, skCanvas_ is null");
        return;
    }
    LOGE("CreateCanvas successfully!!!");
}

SkCanvas* RenderContext::GetCanvas() const
{
    return skCanvas_;
}

void RenderContext::RenderFrame()
{
    // flush commands
    if (skCanvas_ != nullptr) {
        LOGW("RenderFrame: %{public}p", this);
        skCanvas_->flush();
    } else {
        LOGW("RenderFrame failed!!!");
    }
}

void RenderContext::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height)
{
#if EGL_EGLEXT_PROTOTYPES
    RenderContext* rc = RenderContextFactory::GetInstance().CreateEngine();
    EGLDisplay eglDisplay = rc->GetEGLDisplay();
    EGLSurface eglSurface = rc->GetEGLSurface();

    if ((eglDisplay == nullptr) || (eglSurface == nullptr)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return;
    }

    EGLint rect[4];
    rect[0] = left;
    rect[1] = top;
    rect[2] = width;
    rect[3] = height;

    if (!eglSetDamageRegionKHR(eglDisplay, eglSurface, rect, 0)) {
        LOGE("eglSetDamageRegionKHR is failed");
    }
#endif
}
}
}
