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

#ifndef GLES_RENDER_BACKEND_H
#define GLES_RENDER_BACKEND_H
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContextOptions.h"

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#else
#include "include/gpu/GrDirectContext.h"
#endif

#include "interface_render_backend.h"
#include "egl_manager.h"
#include "draw/surface.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {
class GLESRenderBackend : public IRenderBackend {
public:
    GLESRenderBackend() noexcept;
    ~GLESRenderBackend() override;
    void InitDrawContext() override;
    void MakeCurrent() override;
    void SwapBuffers() override;
    void* CreateSurface(void* window) override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override;
    GrDirectContext* GetGrContext() const
    {
        return grContext_.get();
    }
    bool SetUpGrContext();
    bool SetUpDrContext();
    void Destroy() override;
    void RenderFrame() override;
    SkCanvas* AcquireSkCanvas(std::unique_ptr<SurfaceFrame>& frame) override;
    Drawing::Canvas* AcquireDrCanvas(std::unique_ptr<SurfaceFrame>& frame) override;
private:
    EGLManager* eglManager_ = nullptr;
    sk_sp<GrDirectContext> grContext_ = nullptr;
    sk_sp<SkSurface> skSurface_ = nullptr;
    SkSurface* pSkSurface_ = nullptr;

    std::shared_ptr<Drawing::GPUContext> drGPUContext_ = nullptr;
    std::shared_ptr<Drawing::Surface> drSurface_ = nullptr;
};
}
}
#endif