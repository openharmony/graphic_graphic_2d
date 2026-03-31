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
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {

enum class RenderContextVKType : uint8_t {
    BASIC_RENDER = 0,
    PROTECTED_REDRAW,
    UNPROTECTED_REDRAW,
    MAX_INTERFACE_TYPE,
};

class RenderContextVK : public RenderContext {
public:
    RenderContextVK() = default;
    ~RenderContextVK() override;

    bool Init() override;
    bool AbandonContext() override;
    std::string GetShaderCacheSize() const override;
    std::string CleanAllShaderCache() const override;
    bool SetUpGpuContext(std::shared_ptr<Drawing::GPUContext> drawingContext = nullptr) override;
    void SetRenderContextType(uint8_t type) override;
    void ChangeProtectedState(bool isProtected) override;
    #ifdef ROSEN_ARKUI_X
    void AddSurface() override {}
    void DeleteSurface() override {}
    void SetCleanUpHelper(std::function<void()> func) override {}
    void DestroySharedSource() override {}
    #endif
    
private:
    std::atomic<bool> isProtected_{false};
};
} // namespace Rosen
} // namespace OHOS
#endif
