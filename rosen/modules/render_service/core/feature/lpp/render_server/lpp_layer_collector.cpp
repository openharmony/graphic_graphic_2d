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
#include "feature/lpp/render_server/lpp_layer_collector.h"

namespace OHOS::Rosen {
void LppLayerCollector::AddLppLayerId(const std::vector<RSLayerPtr>& layers)
{
    lppLayerId_.clear();
    for (const auto& layer : layers) {
        bool isLppLayer =
            layer != nullptr && layer->GetTunnelLayerId() != 0 && layer->GetTunnelLayerProperty() == LPP_LAYER_PROPERTY;
        if (!isLppLayer) {
            continue;
        }
        lppLayerId_.insert(static_cast<uint64_t>(layer->GetNodeId()));
    }
}

void LppLayerCollector::RemoveLayerId(const std::vector<RSLayerPtr>& layers)
{
    for (const auto& layer : layers) {
        bool isLppLayer =
            layer != nullptr && layer->GetTunnelLayerId() != 0 && layer->GetTunnelLayerProperty() == LPP_LAYER_PROPERTY;
        if (!isLppLayer) {
            continue;
        }
        lppLayerId_.erase(static_cast<uint64_t>(layer->GetNodeId()));
    }
}

const std::unordered_set<uint64_t>& LppLayerCollector::GetLppLayerId()
{
    return lppLayerId_;
}

} // namespace OHOS::Rosen