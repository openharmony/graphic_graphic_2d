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

#ifndef SOFTWARE_RENDER_BACKEND_H
#define SOFTWARE_RENDER_BACKEND_H
#include "interface_render_backend.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"

namespace OHOS {
namespace Rosen {
class SoftwareRenderBackend : public IRenderBackend {
public:
    SoftwareRenderBackend() : skCanvas_(nullptr) {}
    ~SoftwareRenderBackend() override {}
    void InitDrawContext() override {};
    void MakeCurrent() override {};
    void SwapBuffers() override {};
    void* CreateSurface(void* window) override
    {
        return nullptr;
    };
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override {};
    void Destroy() override {};
    void RenderFrame() override {};
    SkCanvas* AcquireCanvas(std::unique_ptr<SurfaceFrame>& frame) override;
protected:
    std::shared_ptr<SkCanvas> skCanvas_;
};
}
}
#endif