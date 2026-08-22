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

#ifndef RENDER_CONTEXT_VK_H
#define RENDER_CONTEXT_VK_H

#include <atomic>

#ifndef ROSEN_ARKUI_X
#include "vulkan_context/rs_vulkan_context.h"
#else
#include "rs_vulkan_context.h"
#endif

#include "render_context/render_context.h"
#include "vulkan_context/rs_vulkan_header_ext.h"

namespace OHOS {
namespace Rosen {

class RenderContextVK : public RenderContext {
public:
    RenderContextVK() = default;
    ~RenderContextVK() override;

    bool Init(RenderEngineType type = RenderEngineType::BASIC_RENDER, const std::string& cacheDir = "") override;
    bool AbandonContext() override;
    RenderEngineType GetType() override { return contextType_; }
    bool SetUpGpuContext(const std::string& cacheDir = "") override;
    bool QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight) override;
    std::shared_ptr<Drawing::GPUContext> CreateDrawingGPUContext(const std::string& cacheDir = "") override;
    void ReleaseDrawingGPUContext(std::shared_ptr<Drawing::GPUContext> gpuContext) override;

    #ifdef ROSEN_ARKUI_X
    void AddSurface() override;
    void DeleteSurface() override;
    void SetCleanUpHelper(std::function<void()> func) override;
    void DestroySharedSource() override;
    #endif

private:
    RenderEngineType contextType_ = RenderEngineType::BASIC_RENDER;
};
} // namespace Rosen
} // namespace OHOS
#endif
