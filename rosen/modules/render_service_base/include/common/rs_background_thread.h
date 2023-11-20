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

#ifndef RS_BACKGROUND_THREAD_H
#define RS_BACKGROUND_THREAD_H

#include "event_handler.h"
#include "common/rs_macros.h"
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif
#include "include/core/SkSurface.h"
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#endif
#else
#include "image/gpu_context.h"
#endif
#endif

namespace OHOS::Rosen {
class RenderContext;

class RSB_EXPORT RSBackgroundThread final {
public:
    static RSBackgroundThread& Instance();
    void PostTask(const std::function<void()>& task);
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void InitRenderContext(RenderContext* context);
    void CleanGrResource();
#ifndef USE_ROSEN_DRAWING
    sk_sp<GrDirectContext> GetShareGrContext() const;
#else
    std::shared_ptr<Drawing::GPUContext> GetShareGPUContext() const;
#endif
#endif
private:
    RSBackgroundThread();
    ~RSBackgroundThread() = default;
    RSBackgroundThread(const RSBackgroundThread&);
    RSBackgroundThread(const RSBackgroundThread&&);
    RSBackgroundThread& operator=(const RSBackgroundThread&);
    RSBackgroundThread& operator=(const RSBackgroundThread&&);

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef RS_ENABLE_GL
    void CreateShareEglContext();
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#endif
    RenderContext* renderContext_ = nullptr;
#ifndef USE_ROSEN_DRAWING
    sk_sp<GrDirectContext> CreateShareGrContext();
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    EGLContext eglShareContext_ = static_cast<EGLContext>(0);
    std::shared_ptr<Drawing::GPUContext> CreateShareGPUContext();
    std::shared_ptr<Drawing::GPUContext> gpuContext_ = nullptr;
#endif
#endif
};
}
#endif // RS_BACKGROUND_THREAD_H
