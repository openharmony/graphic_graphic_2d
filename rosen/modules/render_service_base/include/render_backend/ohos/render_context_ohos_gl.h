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

#ifndef RENDER_CONTEXT_OHOS_GL_H
#define RENDER_CONTEXT_OHOS_GL_H

#include <memory>
#include <mutex>

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#else
#include "include/gpu/gl/GrGLInterface.h"
#endif

#include "common/rs_rect.h"
#include "render_context_base.h"

namespace OHOS {
namespace Rosen {
class RenderContextOhosGl : public RenderContextBase {
public:
    explicit RenderContextOhosGl() noexcept;
    ~RenderContextOhosGl();
    void Init() override;
    bool IsContextReady() override;
    void MakeCurrent(void* curSurface = nullptr, void* curContext = nullptr) override;
    void* CreateContext(bool share = false) override;
    bool CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void DestroySurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void DamageFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    int32_t GetBufferAge() override;
    void SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
private:
    void CreatePbufferSurface();
    void SetEGLState();

    EGLNativeWindowType nativeWindow_;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLSurface pbufferSurface_ = EGL_NO_SURFACE;
    EGLConfig config_;
    std::mutex shareContextMutex_;
};
}
}
#endif