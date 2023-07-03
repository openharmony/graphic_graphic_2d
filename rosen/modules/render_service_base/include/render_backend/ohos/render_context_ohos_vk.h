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

#ifndef RENDER_SERVICE_RENDER_CONTEXT_OHOS_VK_H
#define RENDER_SERVICE_RENDER_CONTEXT_OHOS_VK_H

#include <memory>

#include <vulkan_window.h>

#include "render_context_base.h"
#include "rs_render_surface_frame.h"

namespace OHOS {
namespace Rosen {
class RenderContextOhosVk : public RenderContextBase {
public:
    explicit RenderContextOhosVk() noexcept = default;
    ~RenderContextOhosVk();
    void Init() override;
    bool IsContextReady() override;
    bool CreateSurface(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    void DamageFrame(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
    int32_t GetBufferAge() override;
    void SwapBuffers(const std::shared_ptr<RSRenderSurfaceFrame>& frame) override;
private:
    std::shared_ptr<vulkan::VulkanWindow> vulkanWindow_;
};
}
}
#endif