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
    std::vector<std::shared_ptr<RSLayer>> layersVector;
    for (const auto& pair : layersMap_) {
        auto rsLayer = pair.second;
        if (!rsLayer->GetIsNeedComposition()) {
            RS_LOGD("GetRSLayersVec: layer %{public}s did not compose", rsLayer->GetSurfaceName().c_str());
            continue;
        }
        layersVector.push_back(pair.second);
    }
    return layersVector;
}

std::shared_ptr<RSLayer> RSRenderComposerContext::GetRSRenderLayer(RSLayerId rsLayerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = layersMap_.find(rsLayerId);
    return (iter != layersMap_.end()) ? iter->second : nullptr;
}

void RSRenderComposerContext::AddRSRenderLayer(RSLayerId rsLayerId, std::shared_ptr<RSLayer> rsLayer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    layersMap_[rsLayerId] = rsLayer;
}

void RSRenderComposerContext::RemoveRSRenderLayer(RSLayerId rsLayerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = layersMap_.find(rsLayerId);
    if (iter == layersMap_.end()) {
        RS_LOGW("RemoveRSRenderLayer: layerId=%{public}" PRIu64 " not found", rsLayerId);
        return;
    }
    auto surfaceName = (iter != layersMap_.end() && iter->second) ? iter->second->GetSurfaceName() : "";
    RS_TRACE_NAME_FMT("RSRenderComposerContext::RemoveRSRenderLayer layerId: %" PRIu64 ", surfaceName: %s",
        rsLayerId, surfaceName.c_str());
    RS_LOGI("RemoveRSRenderLayer: removing layerId=%{public}" PRIu64 ", surfaceName=%{public}s",
        rsLayerId, surfaceName.c_str());
    layersMap_.erase(iter);
}

uint64_t RSRenderComposerContext::GetRSRenderLayerCount()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return layersMap_.size();
}
} // namespace Rosen
} // namespace OHOS