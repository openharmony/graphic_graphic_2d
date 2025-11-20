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
#include "rs_render_composer_context.h"

namespace OHOS {
namespace Rosen {
std::vector<std::shared_ptr<RSLayer>> RSRenderComposerContext::GetRSLayersVec()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return std::move(layersVec_);
}

void RSRenderComposerContext::SetRSLayersVec(std::vector<std::shared_ptr<RSLayer>>&& layersVec)
{
    std::lock_guard<std::mutex> lock(mutex_);
    layersVec_ = std::move(layersVec);
}

void RSRenderComposerContext::ClearAllRSLayers()
{
    std::lock_guard<std::mutex> lock(mutex_);
    layersVec_.clear();
}
} // namespace Rosen
} // namespace OHOS