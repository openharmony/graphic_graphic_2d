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
    void RegisterSpatialEffect(const std::shared_ptr<RSRenderNode>& renderNode);
    void UnregisterSpatialEffect(const std::shared_ptr<RSRenderNode>& renderNode);
    void RegisterDepthSpace(const std::shared_ptr<RSRenderNode>& renderNode);
    void RegisterDepthResource(const std::shared_ptr<RSRenderNode>& renderNode);
    void UnregisterDepthResource(const std::shared_ptr<RSRenderNode>& renderNode);
    void RegisterDepthBackground(const std::shared_ptr<RSRenderNode>& renderNode);
    void UnregisterDepthBackground(const std::shared_ptr<RSRenderNode>& renderNode);
    std::weak_ptr<RSRenderNode> GetMasterGlobalDepthNode(NodeId logicalDisplayNodeId);
    std::weak_ptr<RSRenderNode> GetAncestorDepthNode(const RSRenderNode& renderNode);
    void ProcessDepthNodeAndSpatialEffectNodeDirty();
    void PrepareSpatialEffectParams();

private:
    RSSpatialEffectManager() = default;
    ~RSSpatialEffectManager() = default;
    RSSpatialEffectManager(const RSSpatialEffectManager&) = delete;
    RSSpatialEffectManager(RSSpatialEffectManager&&) = delete;
    RSSpatialEffectManager& operator=(const RSSpatialEffectManager&) = delete;
    RSSpatialEffectManager& operator=(RSSpatialEffectManager&&) = delete;

    using NodeMap = std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>;

    void RegisterDepthNodeAndSpatialEffect(
        const std::shared_ptr<RSRenderNode>& depthNode, const std::shared_ptr<RSRenderNode>& renderNode);
    static void CleanNodeMap(NodeMap& nodeMap);
    void CleanExpiredNodes();
    static void ResourceNodeInfectsDepthNodeDirty(const NodeMap& resourceNodeMap);
    bool IsMasterGlobalDepthNodeAndUpdate(const std::shared_ptr<RSDepthRenderNode>& depthNode);
    void SetSpatialEffectNodeDirty(const NodeMap& spatialEffectNodeMap);
    void ProcessDepthResourceNodeParams();
    void CopyGlobalDepthNodeParams();

    std::unordered_map<NodeId, std::pair<std::weak_ptr<RSRenderNode>, NodeMap>> depthSpatialEffectNodeMap_;
    std::unordered_map<NodeId, std::pair<std::weak_ptr<RSRenderNode>, std::pair<NodeId, std::weak_ptr<RSRenderNode>>>>
        spatialEffectDepthNodeMap_;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> masterGlobalDepthNodeMap_;
    NodeMap depthNodeMap_;
    NodeMap depthResourceNodeMap_;
    NodeMap depthBackgroundNodeMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_MANAGER_H