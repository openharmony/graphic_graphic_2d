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

#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {
class RenderContextVK : public RenderContext {

public:
    RenderContextVK() = default;
    ~RenderContextVK() override;

    bool Init() override;
    bool AbandonContext() override;
    std::string GetShaderCacheSize() const override;
    std::string CleanAllShaderCache() const override;
    bool SetUpGpuContext(std::shared_ptr<Drawing::GPUContext> drawingContext) override;
};
} // namespace Rosen
} // namespace OHOS
#endif