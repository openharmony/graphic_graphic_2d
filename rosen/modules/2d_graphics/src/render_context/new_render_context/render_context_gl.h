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

#ifndef RENDER_CONTEXT_GL_H
#define RENDER_CONTEXT_GL_H

#include <memory>

#include "common/rs_rect.h"
#include "render_context/render_context.h"

#ifdef ROSEN_IOS
#include "render_context/render_context_egl_defines.h"
#else
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"
#endif

namespace OHOS {
namespace Rosen {
class RenderContextGL : public RenderContext {

public:
    RenderContextGL();
    ~RenderContextGL() override;

    bool Init() override;
    bool AbandonContext() override;
    std::string GetShaderCacheSize() const override;
    std::string CleanAllShaderCache() const override;
    bool SetUpGpuContext(std::shared_ptr<Drawing::GPUContext> context = nullptr) override;

    std::shared_ptr<Drawing::Surface> AcquireSurface(int width, int height) override;
    void RenderFrame() override;
    void DamageFrame(const std::vector<RectI> &rects) override;
    void ClearRedundantResources() override;
    void CreateShareContext() override;
    void DestroyShareContext() override;
    int32_t QueryEglBufferAge() override;

    void DestroyEGLSurface(EGLSurface surface);
    EGLSurface CreateEGLSurface(EGLNativeWindowType eglNativeWindow);
    void MakeCurrent(EGLSurface surface, EGLContext context = EGL_NO_CONTEXT);
    bool SwapBuffers(EGLSurface surface) const;
    EGLSurface GetEGLSurface() const
    {
        return eglSurface_;
    }

    EGLContext GetEGLContext() const
    {
        return eglContext_;
    }

    EGLDisplay GetEGLDisplay() const
    {
        return eglDisplay_;
    }

#ifdef ROSEN_IOS
    std::shared_ptr<Drawing::ColorSpace> ColorSpace() const { return color_space_; }
    bool UpdateStorageSizeIfNecessary();
    bool ResourceMakeCurrent();
    static const EGLContext GetResourceContext();
#endif

protected:
#ifdef ROSEN_IOS
    std::shared_ptr<Drawing::ColorSpace> color_space_ = nullptr;
    void *layer_ = nullptr;
    static EGLContext resourceContext;
    static std::mutex resourceContextMutex;

    uint32_t framebuffer_ = 0;
    uint32_t colorbuffer_ = 0;
    int32_t storage_width_ = 0;
    int32_t storage_height_ = 0;
    bool valid_ = false;
#endif

private:
    bool IsEglContextReady() const
    {
        return eglContext_ != EGL_NO_CONTEXT;
    }

    void CreatePbufferSurface();
#ifndef ROSEN_IOS
    static PFNEGLSETDAMAGEREGIONKHRPROC GetEGLSetDamageRegionKHRFunc();
    static bool CheckEglExtension(const char* extensions, const char* extension);
    static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void* native_display, const EGLint* attrib_list);
    int ColorTypeToGLFormat(Drawing::ColorType colorType);
#endif

    EGLConfig config_;
    EGLNativeWindowType nativeWindow_;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLSurface pbufferSurface_ = EGL_NO_SURFACE;
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;

    const std::string UNIRENDER_CACHE_DIR = "/data/service/el0/render_service";
};
} // namespace Rosen
} // namespace OHOS
#endif
