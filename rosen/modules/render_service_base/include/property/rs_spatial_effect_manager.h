/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_MANAGER_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_MANAGER_H

#include <unordered_map>

#include "pipeline/rs_depth_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSpatialEffectManager {
public:
    static RSSpatialEffectManager* Instance();
    void  RegisterSpatialEffect(const std::shared_ptr<RSRenderNode>& renderNode);
    void  RegisterDepthSpace(const std::shared_ptr<RSRenderNode>& renderNode);
    void  RegisterDepthResource(const std::shared_ptr<RSRenderNode>& renderNode);
    void  PrepareSpatialEffect();

private:
    RSSpatialEffectManager() = default;
    ~RSSpatialEffectManager() = default;
    RSSpatialEffectManager(const RSSpatialEffectManager&) = delete;
    RSSpatialEffectManager(RSSpatialEffectManager&&) = delete;
    RSSpatialEffectManager& operator=(const RSSpatialEffectManager&) = delete;
    RSSpatialEffectManager& operator=(RSSpatialEffectManager&&) = delete;

    using SpatialEffectNodeMap = std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>;
    std::unordered_map<NodeId, std::pair<std::weak_ptr<RSRenderNode>, SpatialEffectNodeMap>>
        depthNodeSpatialEffectMap_;
    std::weak_ptr<RSRenderNode> globalDepthNode_;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> depthNodeMap_;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> depthResourceNodeMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_MANAGER_H