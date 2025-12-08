/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "render_context_gl.h"

#include "memory/rs_tag_tracker.h"
#include "render_context/render_context_log.h"
#include "rs_trace.h"
#include "window.h"

#ifdef RS_ENABLE_GL
#include "EGL/egl.h"
#endif

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
PFNEGLSETDAMAGEREGIONKHRPROC RenderContextGL::GetEGLSetDamageRegionKHRFunc()
{
    static auto func =
        reinterpret_cast<PFNEGLSETDAMAGEREGIONKHRPROC>(eglGetProcAddress("eglSetDamageRegionKHR"));
    return func;
}

bool RenderContextGL::CheckEglExtension(const char* extensions, const char* extension)
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

EGLDisplay RenderContextGL::GetPlatformEglDisplay(EGLenum platform, void* native_display, const EGLint* attrib_list)
{
    static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

    if (!eglGetPlatformDisplayExt) {
        const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
                CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt = reinterpret_cast<GetPlatformDisplayExt>(
                eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT));
        }
    }

    if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
    }

    return eglGetDisplay(static_cast<EGLNativeDisplayType>(native_display));
}

int RenderContextGL::ColorTypeToGLFormat(Drawing::ColorType colorType)
{
    switch (colorType) {
        case Drawing::ColorType::COLORTYPE_RGBA_8888:
            return GL_RGBA8;
        case Drawing::ColorType::COLORTYPE_ALPHA_8:
            return GL_R8;
        case Drawing::ColorType::COLORTYPE_RGB_565:
            return GL_RGB565;
        case Drawing::ColorType::COLORTYPE_ARGB_4444:
            return GL_RGBA4;
        default:
            return GL_RGBA8;
    }
}

RenderContextGL::RenderContextGL()
{
    surface_ = nullptr;
    drGPUContext_ = nullptr;
    nativeWindow_ = nullptr;
    eglDisplay_ = EGL_NO_DISPLAY;
    eglContext_ = EGL_NO_CONTEXT;
    eglSurface_ = EGL_NO_SURFACE;
    config_ = nullptr;
    mHandler_ = nullptr;
}

RenderContextGL::~RenderContextGL()
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
    drGPUContext_ = nullptr;
    surface_ = nullptr;
    mHandler_ = nullptr;
}

bool RenderContextGL::Init()
{
    if (IsEglContextReady()) {
        return true;
    }

    LOGD("Creating EGLContext!!!");
    eglDisplay_ = GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL);
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        LOGW("Failed to create EGLDisplay gl errno : %{public}x", eglGetError());
        return false;
    }

    EGLint major;
    EGLint minor;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        LOGE("Failed to initialize EGLDisplay");
        return false;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        LOGE("Failed to bind OpenGL ES API");
        return false;
    }

    EGLint count;
    EGLint config_attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    unsigned int ret = eglChooseConfig(eglDisplay_, config_attribs, &config_, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        LOGE("Failed to eglChooseConfig");
        return false;
    }

    static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE };
    eglContext_ = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        LOGE("Failed to create egl context %{public}x", eglGetError());
        return false;
    }
    CreatePbufferSurface();
    if (!eglMakeCurrent(eglDisplay_, pbufferSurface_, pbufferSurface_, eglContext_)) {
        LOGE("Failed to make current on surface, error is %{public}x", eglGetError());
        return false;
    }
    LOGD("Create EGL context successfully, version %{public}d.%{public}d", major, minor);
    return true;
}

bool RenderContextGL::AbandonContext()
{
    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    return true;
}

bool RenderContextGL::SetUpGpuContext(std::shared_ptr<Drawing::GPUContext> drawingContext)
{
    if (drGPUContext_ != nullptr) {
        LOGD("Drawing GPUContext has already created!!");
        return true;
    }

    mHandler_ = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (isUniRenderMode_) {
        cacheDir_ = UNIRENDER_CACHE_DIR;
    }
    Drawing::GPUContextOptions options;
    if (glesVersion != nullptr) {
        auto size = glesVersion ? strlen(glesVersion) : 0;
        mHandler_->ConfigureContext(&options, glesVersion, size, cacheDir_, isUniRenderMode_);
    }

    auto drGPUContext = std::make_shared<Drawing::GPUContext>();
    if (!drGPUContext->BuildFromGL(options)) {
        LOGE("SetUpGrContext drGPUContext is null");
        return false;
    }
    drGPUContext_ = std::move(drGPUContext);
    return true;
}

void RenderContextGL::CreatePbufferSurface()
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

void RenderContextGL::MakeCurrent(EGLSurface surface, EGLContext context)
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

bool RenderContextGL::SwapBuffers(EGLSurface surface) const
{
    RS_TRACE_FUNC();
    if (!eglSwapBuffers(eglDisplay_, surface)) {
        LOGE("Failed to SwapBuffers on surface, error is %{public}x", eglGetError());
        return false;
    } else {
        LOGD("SwapBuffers successfully");
        return true;
    }
}

EGLSurface RenderContextGL::CreateEGLSurface(EGLNativeWindowType eglNativeWindow)
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

    LOGD("CreateEGLSurface");

    eglSurface_ = surface;
    return surface;
}

void RenderContextGL::DestroyEGLSurface(EGLSurface surface)
{
    if (!eglDestroySurface(eglDisplay_, surface)) {
        LOGE("Failed to DestroyEGLSurface surface, error is %{public}x", eglGetError());
    }
}

std::shared_ptr<Drawing::Surface> RenderContextGL::AcquireSurface(int width, int height)
{
    if (!SetUpGpuContext()) {
        LOGE("GrContext is not ready!!!");
        return nullptr;
    }

    std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr;

    switch (colorSpace_) {
        // [planning] in order to stay consistant with the colorspace used before, we disabled
        // COLOR_GAMUT_SRGB to let the branch to default, then skColorSpace is set to nullptr
        case GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
            colorSpace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB,
                Drawing::CMSMatrixType::DCIP3);
            break;
        case GRAPHIC_COLOR_GAMUT_ADOBE_RGB:
            colorSpace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB,
                Drawing::CMSMatrixType::ADOBE_RGB);
            break;
        case GRAPHIC_COLOR_GAMUT_BT2020:
            colorSpace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB,
                Drawing::CMSMatrixType::REC2020);
            break;
        default:
            break;
    }

    RSTagTracker tagTracker(GetSharedDrGPUContext(),
                            RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);

    struct Drawing::FrameBuffer bufferInfo;
    bufferInfo.width = width;
    bufferInfo.height = height;
    bufferInfo.FBOID = 0;
    bufferInfo.Format = GL_RGBA8;
    bufferInfo.gpuContext = drGPUContext_;
    bufferInfo.colorSpace = colorSpace;
    bufferInfo.colorType = Drawing::COLORTYPE_RGBA_8888;

    if (pixelFormat_ == GRAPHIC_PIXEL_FMT_RGBA_1010102) {
        bufferInfo.Format = GL_RGB10_A2;
        bufferInfo.colorType = Drawing::COLORTYPE_RGBA_1010102;
    }

    surface_ = std::make_shared<Drawing::Surface>();
    if (!surface_->Bind(bufferInfo)) {
        LOGW("surface_ is nullptr");
        surface_ = nullptr;
        return nullptr;
    }

    LOGD("CreateCanvas successfully!!!");
    return surface_;
}

void RenderContextGL::RenderFrame()
{
    RS_TRACE_FUNC();
    // flush commands
    if (surface_ != nullptr && surface_->GetCanvas() != nullptr) {
        LOGD("RenderFrame: Canvas");
        RSTagTracker tagTracker(GetSharedDrGPUContext(),
                                RSTagTracker::TAGTYPE::TAG_RENDER_FRAME);
        surface_->GetCanvas()->Flush();
    } else {
        LOGW("canvas is nullptr!!!");
    }
}

int32_t RenderContextGL::QueryEglBufferAge()
{
    if ((eglDisplay_ == EGL_NO_DISPLAY) || (eglSurface_ == EGL_NO_SURFACE)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return EGL_UNKNOWN;
    }
    EGLint bufferAge = EGL_UNKNOWN;
    EGLBoolean ret = eglQuerySurface(eglDisplay_, eglSurface_, EGL_BUFFER_AGE_KHR, &bufferAge);
    if (ret == EGL_FALSE) {
        LOGE("eglQuerySurface is failed");
        return EGL_UNKNOWN;
    }
    return static_cast<int32_t>(bufferAge);
}

void RenderContextGL::DamageFrame(const std::vector<RectI> &rects)
{
    if ((eglDisplay_ == EGL_NO_DISPLAY) || (eglSurface_ == EGL_NO_SURFACE)) {
        LOGE("eglDisplay or eglSurface is nullptr");
        return;
    }
    RS_TRACE_FUNC();

    size_t size = rects.size();
    if (size == 0) {
        LOGD("invalid rects size");
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

void RenderContextGL::ClearRedundantResources()
{
    RS_TRACE_FUNC();
    if (drGPUContext_ != nullptr) {
        LOGD("grContext clear redundant resources");
        drGPUContext_->Flush();
        // GPU resources that haven't been used in the past 10 seconds
        drGPUContext_->PerformDeferredCleanup(std::chrono::seconds(10));
    }
}

std::string RenderContextGL::GetShaderCacheSize() const
{
    if (mHandler_) {
        return mHandler_->QuerryShader();
    }

    LOGD("GetShaderCacheSize no shader cache");
    return "";
}

std::string RenderContextGL::CleanAllShaderCache() const
{
    if (mHandler_) {
        return mHandler_->ClearShader();
    }

    LOGD("CleanAllShaderCache no shader cache");
    return "";
}

void RenderContextGL::CreateShareContext()
{
    std::unique_lock<std::mutex> lock(shareContextMutex_);
    static const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE};
    eglShareContext_ = eglCreateContext(eglDisplay_, config_, eglContext_, context_attribs);
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    auto eglDisplay = GetEGLDisplay();
    if (!eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        LOGE("eglMakeCurrent failed");
        return;
    }
}

void RenderContextGL::DestroyShareContext()
{
    eglDestroyContext(GetEGLDisplay(), eglShareContext_);
    eglShareContext_ = EGL_NO_CONTEXT;
    eglMakeCurrent(GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}
} // namespace Rosen
} // namespace OHOS
