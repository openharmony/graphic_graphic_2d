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

#ifndef VULKAN_RENDER_BACKEND_H
#define VULKAN_RENDER_BACKEND_H

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContextOptions.h"
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#include "interface_render_backend.h"

namespace OHOS {
namespace Rosen {
class VulkanRenderBackend : public IRenderBackend {
public:
    VulkanRenderBackend() noexcept = default;
    ~VulkanRenderBackend() override = default;
    void InitDrawContext() override {}
    void MakeCurrent() override {}
    void SwapBuffers() override {}
    void* CreateSurface(void* window) override {return nullptr;}
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override {}
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    GrDirectContext* GetGrContext() const
    {
        return grContext_.get();
    }
#else
    GrContext* GetGrContext() const
    {
        return grContext_.get();
    }
#endif
    bool SetUpGrContext() {return true;}
    void Destroy() override;
    void RenderFrame() override;
    SkCanvas* AcquireSkCanvas(std::unique_ptr<SurfaceFrame>& frame) override;
    Drawing::Canvas* AcquireDrCanvas(std::unique_ptr<SurfaceFrame>& frame) override;
private:
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
    SkSurface* skSurface_ = nullptr;
    Drawing::Surface* drSurface_ = nullptr;
};
}
}
#endif