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

#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "surface_type.h"

#define GLES_VERSION 2
namespace OHOS {
namespace Rosen {
class RenderContext {
public:
    RenderContext();
    virtual ~RenderContext();
    void CreateCanvas(int width, int height);
    SkCanvas* AcquireCanvas(int width, int height);

    void SetColorSpace(SurfaceColorGamut colorSpace);
    void InitializeEglContext();

    GrContext* GetGrContext() const
    {
        return grContext_.get();
    }

    bool SetUpGrContext();

    EGLSurface CreateEGLSurface(EGLNativeWindowType eglNativeWindow);
    void MakeCurrent(EGLSurface surface) const;
    void SwapBuffers(EGLSurface surface) const;
    void RenderFrame();
    void DamageFrame(int32_t left, int32_t top, int32_t width, int32_t height);

    EGLSurface GetEGLSurface() const
    {
        return eglSurface_;
    }

    EGLDisplay GetEGLDisplay() const
    {
        return eglDisplay_;
    }

    bool IsEglContextReady()
    {
        return eglContext_ != EGL_NO_DISPLAY;
    }

private:
    sk_sp<GrContext> grContext_;
    sk_sp<SkSurface> skSurface_;

    EGLNativeWindowType nativeWindow_;

    EGLDisplay eglDisplay_;
    EGLContext eglContext_;
    EGLSurface eglSurface_;
    EGLConfig config_;
    SurfaceColorGamut colorSpace_ = SurfaceColorGamut::COLOR_GAMUT_SRGB;
};

class RenderContextFactory {
public:
    ~RenderContextFactory()
    {
        if (context_ != nullptr) {
            delete context_;
        }
        context_ = nullptr;
    }

    static RenderContextFactory& GetInstance()
    {
        static RenderContextFactory rf;
        return rf;
    }

    RenderContext* CreateEngine()
    {
        if (context_ == nullptr) {
            context_ = new RenderContext();
        }

        return context_;
    }

private:
    RenderContextFactory() : context_(nullptr) {}
    RenderContextFactory(const RenderContextFactory&) = delete;
    RenderContextFactory& operator=(const RenderContextFactory&) = delete;

    RenderContext* context_;
};
} // namespace Rosen
} // namespace OHOS
#endif
