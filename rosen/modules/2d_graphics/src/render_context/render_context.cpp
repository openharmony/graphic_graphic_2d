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

#include "EGL/egl.h"
#include "rs_trace.h"
#include "window.h"

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr const char* EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char* EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char* CHARACTER_STRING_WHITESPACE = " ";
constexpr const char* EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
constexpr const char* EGL_KHR_SURFACELESS_CONTEXT = "EGL_KHR_surfaceless_context";

// use functor to call gel*KHR API
static PFNEGLSETDAMAGEREGIONKHRPROC GetEGLSetDamageRegionKHRFunc()
{
    static auto func = reinterpret_cast<PFNEGLSETDAMAGEREGIONKHRPROC>(eglGetProcAddress("eglSetDamageRegionKHR"));
    return func;
}

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

RenderContext::RenderContext()
    : grContext_(nullptr),
      skSurface_(nullptr),
      nativeWindow_(nullptr),
      eglDisplay_(EGL_NO_DISPLAY),
      eglContext_(EGL_NO_CONTEXT),
      eglSurface_(EGL_NO_SURFACE),
      config_(nullptr),
      mHandler_(nullptr)
{}

RenderContext::~RenderContext()
{
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        return;
    }

    eglDestroyContext(eglDisplay_, eglContext_);
    if (pbufferSurface_ != EGL_NO_SURFACE) {
        eglDestroySurface(eglDisplay_, pbufferSurface_);
    }
    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(eglDisplay_);
    eglReleaseThread();

    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
    eglSurface_ = EGL_NO_SURFACE;
    pbufferSurface_ = EGL_NO_SURFACE;
    grContext_ = nullptr;
    skSurface_ = nullptr;
    mHandler_ = nullptr;
}

void RenderContext::CreatePbufferSurface()
{
    const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    if ((extensions != nullptr) &&
       (!CheckEglExtension(extensions, EGL_KHR_SURFACELESS_CONTEXT)) &&
       (pbufferSurface_ == EGL_NO_SURFACE)) {
        EGLint attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
        pbufferSurface_ = eglCreatePbufferSurface(eglDisplay_, config_, attribs);
        if (pbufferSurface_ == EGL_NO_SURFACE) {
            LOGE("Failed to create pbuffer surface");
            return;
        }
    }
}

void RenderContext::InitializeEglContext()
{
    if (IsEglContextReady()) {
        return;
    }

    LOGD("Creating EGLContext!!!");
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

    unsigned int ret;
    EGLint count;
    EGLint config_attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(eglDisplay_, config_attribs, &config_, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        LOGE("Failed to eglChooseConfig");
        return;
    }

    static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };

    eglContext_ = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        LOGE("Failed to create egl context %{public}x", eglGetError());
        return;
    }
    CreatePbufferSurface();
    if (!eglMakeCurrent(eglDisplay_, pbufferSurface_, pbufferSurface_, eglContext_)) {
        LOGE("Failed to make current on surface %{public}p, error is %{public}x", pbufferSurface_, eglGetError());
        return;
    }

    LOGD("Create EGL context successfully, version %{public}d.%{public}d", major, minor);
}

void RenderContext::MakeCurrent(EGLSurface surface, EGLContext context)
{
    if (surface == EGL_NO_SURFACE) {
        surface = pbufferSurface_;
    }
    if (context == EGL_NO_CONTEXT) {
        context = eglContext_;
    }
    if (!eglMakeCurrent(eglDisplay_, surface, surface, context)) {
        LOGE("Failed to make current on surface, error is %{public}x", eglGetError());
    }
    eglSurface_ = surface;
}

void RenderContext::SwapBuffers(EGLSurface surface) const
{
    RS_TRACE_FUNC();
    if (!eglSwapBuffers(eglDisplay_, surface)) {
        LOGE("Failed to SwapBuffers on surface %{public}p, error is %{public}x", surface, eglGetError());
    } else {
        LOGD("SwapBuffers successfully, surface is %{public}p", surface);
    }
}

void RenderContext::DestroyEGLSurface(EGLSurface surface)
{
    if (!eglDestroySurface(eglDisplay_, surface)) {
        LOGE("Failed to DestroyEGLSurface surface %{public}p, error is %{public}x", surface, eglGetError());
    }
}

EGLSurface RenderContext::CreateEGLSurface(EGLNativeWindowType eglNativeWindow)
{
    if (!IsEglContextReady()) {
        LOGE("EGL context has not initialized");
        return EGL_NO_SURFACE;
    }
    nativeWindow_ = eglNativeWindow;

    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    EGLSurface surface = eglCreateWindowSurface(eglDisplay_, config_, nativeWindow_, NULL);
    if (surface == EGL_NO_SURFACE) {
        LOGW("Failed to create eglsurface!!! %{public}x", eglGetError());
        return EGL_NO_SURFACE;
    }

    LOGD("CreateEGLSurface: %{public}p", surface);

    eglSurface_ = surface;
    return surface;
}

void RenderContext::SetColorSpace(ColorGamut colorSpace)
{
    colorSpace_ = colorSpace;
}

bool RenderContext::SetUpGrContext()
{
    if (grContext_ != nullptr) {
        LOGD("grContext has already created!!");
        return true;
    }

    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (glInterface.get() == nullptr) {
        LOGE("SetUpGrContext failed to make native interface");
        return false;
    }

    GrContextOptions options;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    mHandler_ = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    if (isUniRenderMode_) {
        cacheDir_ = UNIRENDER_CACHE_DIR;
    }
    mHandler_->ConfigureContext(&options, glesVersion, size, cacheDir_, isUniRenderMode_);

    sk_sp<GrContext> grContext(GrContext::MakeGL(std::move(glInterface), options));
    if (grContext == nullptr) {
        LOGE("SetUpGrContext grContext is null");
        return false;
    }
    grContext_ = std::move(grContext);
    return true;
}

sk_sp<SkSurface> RenderContext::AcquireSurface(int width, int height)
{
    if (!SetUpGrContext()) {
        LOGE("GrContext is not ready!!!");
        return nullptr;
    }

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;

    GrBackendRenderTarget backendRenderTarget(width, height, 0, 8, framebufferInfo);
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;

    sk_sp<SkColorSpace> skColorSpace = nullptr;

    switch (colorSpace_) {
        // [planning] in order to stay consistant with the colorspace used before, we disabled
        // COLOR_GAMUT_SRGB to let the branch to default, then skColorSpace is set to nullptr
        case COLOR_GAMUT_DISPLAY_P3:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
            break;
        case COLOR_GAMUT_ADOBE_RGB:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kAdobeRGB);
            break;
        case COLOR_GAMUT_BT2020:
            skColorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kRec2020);
            break;
        default:
            break;
    }

    skSurface_ = SkSurface::MakeFromBackendRenderTarget(
        GetGrContext(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, skColorSpace, &surfaceProps);
    if (skSurface_ == nullptr) {
        LOGW("skSurface is nullptr");
        return nullptr;
    }

    LOGD("CreateCanvas successfully!!! (%{public}p)", skSurface_->getCanvas());
    return skSurface_;
}

void RenderContext::RenderFrame()
{
    RS_TRACE_FUNC();
    // flush commands
    if (skSurface_->getCanvas() != nullptr) {
        LOGD("RenderFrame: Canvas is %{public}p", skSurface_->getCanvas());
        skSurface_->getCanvas()->flush();
    } else {
        LOGW("canvas is nullptr!!!");
    }
}

EGLint RenderContext::QueryEglBufferAge()
{
    if ((eglDisplay_ == nullptr) || (eglSurface_ == nullptr)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return EGL_UNKNOWN;
    }
    EGLint bufferAge = EGL_UNKNOWN;
    EGLBoolean ret = eglQuerySurface(eglDisplay_, eglSurface_, EGL_BUFFER_AGE_KHR, &bufferAge);
    if (ret == EGL_FALSE) {
        LOGE("eglQuerySurface is failed");
        return EGL_UNKNOWN;
    }
    return bufferAge;
}

void RenderContext::DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height)
{
    if ((eglDisplay_ == nullptr) || (eglSurface_ == nullptr)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return;
    }

    EGLint rect[4];
    rect[0] = left;
    rect[1] = top;
    rect[2] = width;
    rect[3] = height;

    EGLBoolean ret = GetEGLSetDamageRegionKHRFunc()(eglDisplay_, eglSurface_, rect, 1);
    if (ret == EGL_FALSE) {
        LOGE("eglSetDamageRegionKHR is failed");
    }
}

void RenderContext::DamageFrame(const std::vector<RectI> &rects)
{
    if ((eglDisplay_ == nullptr) || (eglSurface_ == nullptr)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return;
    }

    size_t size = rects.size();
    if (size == 0) {
        LOGE("invalid rects size");
        return;
    }

    EGLint eglRect[size * 4]; // 4 is size of RectI.
    int index = 0;
    for (const RectI& rect : rects) {
        eglRect[index * 4] = rect.left_; // 4 is size of RectI.
        eglRect[index * 4 + 1] = rect.top_; // 4 is size of RectI.
        eglRect[index * 4 + 2] = rect.width_; // 4 is size of RectI, 2 is the index of the width_ subscript.
        eglRect[index * 4 + 3] = rect.height_; // 4 is size of RectI, 3 is the index of the height_ subscript.
        index++;
    }

    EGLBoolean ret = GetEGLSetDamageRegionKHRFunc()(eglDisplay_, eglSurface_, eglRect, size);
    if (ret == EGL_FALSE) {
        LOGE("eglSetDamageRegionKHR is failed");
    }
}

void RenderContext::ClearRedundantResources()
{
    RS_TRACE_FUNC();
    if (grContext_ != nullptr) {
        LOGD("grContext clear redundant resources");
        grContext_->flush();
        // GPU resources that haven't been used in the past 10 seconds
        grContext_->purgeResourcesNotUsedInMs(std::chrono::seconds(10));
    }
}

RenderContextFactory& RenderContextFactory::GetInstance()
{
    static RenderContextFactory rf;
    return rf;
}
} // namespace Rosen
} // namespace OHOS
