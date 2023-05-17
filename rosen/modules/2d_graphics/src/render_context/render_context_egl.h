/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_CONTEXT_EGL_H
#define RENDER_CONTEXT_EGL_H
#include <memory>
#include <mutex>
#include "common/rs_rect.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "include/gpu/gl/GrGLInterface.h"
#include "memory_handler.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif

#include "render_context.h"

#define GLES_VERSION 2
namespace OHOS {
namespace Rosen {
class RenderContextEGL : public RenderContext {
public:
    RenderContextEGL();
    virtual ~RenderContextEGL();
    void CreateCanvas(int width, int height);
    sk_sp<SkSurface> AcquireSurface(int width, int height) override;

    void InitializeEglContext() override;

#if defined(NEW_SKIA)
    GrDirectContext* GetGrContext() const override
    {
        return grContext_.get();
    }
#else
    GrContext* GetGrContext() const override
    {
        return grContext_.get();
    }
#endif

    sk_sp<SkSurface> GetSurface() const override
    {
        return skSurface_;
    }

    bool SetUpGrContext() override;

    EGLSurface CreateEGLSurface(EGLNativeWindowType eglNativeWindow) override;
    void DestroyEGLSurface(EGLSurface surface);
    void MakeCurrent(EGLSurface surface = nullptr, EGLContext context = EGL_NO_CONTEXT) override;
    
    void SwapBuffers(EGLSurface surface = nullptr) const override;
    void RenderFrame() override;
    EGLint QueryEglBufferAge() override;
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height);
    void DamageFrame(const std::vector<RectI> &rects);
    void ClearRedundantResources() override;
    void CreatePbufferSurface();
    void ShareMakeCurrent(EGLContext shareContext);
    void ShareMakeCurrentNoSurface(EGLContext shareContext);
    void MakeSelfCurrent();
    EGLSurface GetEGLSurface() const
    {
        return eglSurface_;
    }

    EGLContext GetEGLContext() const override
    {
        return eglContext_;
    }

    EGLDisplay GetEGLDisplay() const
    {
        return eglDisplay_;
    }

#ifndef ROSEN_CROSS_PLATFORM
    void SetColorSpace(ColorGamut colorSpace);
    ColorGamut GetColorSpace() const
    {
        return colorSpace_;
    }
#endif

    bool IsEglContextReady() const
    {
        return eglContext_ != EGL_NO_DISPLAY;
    }

    void SetCacheDir(const std::string& filePath) override
    {
        cacheDir_ = filePath;
    }

    void SetUniRenderMode(bool isUni) override
    {
        isUniRenderMode_ = isUni;
    }
#ifdef RS_ENABLE_GL
    std::string GetShaderCacheSize() const override
    {
        return mHandler_->QuerryShader();
    }

    std::string CleanAllShaderCache() const override
    {
        return mHandler_->ClearShader();
    }
#endif
    EGLContext CreateShareContext();

private:
#if defined(NEW_SKIA)
    sk_sp<GrDirectContext> grContext_;
#else
    sk_sp<GrContext> grContext_;
#endif
    sk_sp<SkSurface> skSurface_;

    EGLNativeWindowType nativeWindow_;

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLSurface pbufferSurface_= EGL_NO_SURFACE;
    EGLConfig config_;
#ifndef ROSEN_CROSS_PLATFORM
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;
#endif

    bool isUniRenderMode_ = false;
    const std::string UNIRENDER_CACHE_DIR = "/data/service/el0/render_service";
    std::string cacheDir_;
    std::shared_ptr<MemoryHandler> mHandler_;
    std::mutex shareContextMutex_;
};

} // namespace Rosen
} // namespace OHOS
#endif
