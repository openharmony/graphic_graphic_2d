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

#include "property/rs_spatial_effect_manager.h"
#include "pipeline/rs_canvas_render_node.h"
#include "params/rs_depth_render_params.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSSpatialEffectManager* RSSpatialEffectManager::Instance()
{
    static RSSpatialEffectManager instance;
    return &instance;
}

void RSSpatialEffectManager::RegisterDepthNodeAndSpatialEffect(
    const std::shared_ptr<RSRenderNode>& depthNode, const std::shared_ptr<RSRenderNode>& renderNode)
{
    auto nodeId = renderNode->GetId();
    auto wpNode = renderNode->weak_from_this();
    auto depthNodeId = depthNode->GetId();
    auto wpDepthNode = depthNode->weak_from_this();
    auto& entry = depthSpatialEffectNodeMap_[depthNodeId];
    entry.first = wpDepthNode;
    entry.second.emplace(nodeId, wpNode);
    spatialEffectDepthNodeMap_.emplace(nodeId, std::make_pair(wpNode, std::make_pair(depthNodeId, wpDepthNode)));
}

void RSSpatialEffectManager::RegisterSpatialEffect(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    auto depthNode = renderNode->GetParent().lock();
    while (depthNode && depthNode->GetType() != RSRenderNodeType::DEPTH_NODE) {
        depthNode = depthNode->GetParent().lock();
    }
    if (!depthNode) {
        return;
    }

    RegisterDepthNodeAndSpatialEffect(depthNode, renderNode);
}

void RSSpatialEffectManager::UnregisterSpatialEffect(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }

    auto nodeId = renderNode->GetId();
    auto iter = spatialEffectDepthNodeMap_.find(nodeId);
    if (iter == spatialEffectDepthNodeMap_.end()) {
        return;
    }
    auto depthNodeId = iter->second.second.first;
    spatialEffectDepthNodeMap_.erase(iter);

    auto jter = depthSpatialEffectNodeMap_.find(depthNodeId);
    if (jter == depthSpatialEffectNodeMap_.end()) {
        return;
    }
    jter->second.second.erase(nodeId);
}

void RSSpatialEffectManager::RegisterDepthSpace(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    auto nodeId = renderNode->GetId();
    auto wpNode = renderNode->weak_from_this();
    depthSpatialEffectNodeMap_.emplace(nodeId, std::make_pair(wpNode, NodeMap()));
    depthNodeMap_.emplace(nodeId, wpNode);
}

void RSSpatialEffectManager::RegisterDepthResource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    depthResourceNodeMap_.emplace(renderNode->GetId(), renderNode->weak_from_this());
}

void RSSpatialEffectManager::UnregisterDepthResource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    depthResourceNodeMap_.erase(renderNode->GetId());
}

void RSSpatialEffectManager::RegisterDepthBackground(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    depthBackgroundNodeMap_.emplace(renderNode->GetId(), renderNode->weak_from_this());
}

void RSSpatialEffectManager::UnregisterDepthBackground(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    depthBackgroundNodeMap_.erase(renderNode->GetId());
}

std::weak_ptr<RSRenderNode> RSSpatialEffectManager::GetMasterGlobalDepthNode(NodeId logicalDisplayNodeId)
{
    auto iter = masterGlobalDepthNodeMap_.find(logicalDisplayNodeId);
    return iter != masterGlobalDepthNodeMap_.end() ? iter->second : std::weak_ptr<RSRenderNode>();
}

std::weak_ptr<RSRenderNode> RSSpatialEffectManager::GetAncestorDepthNode(const RSRenderNode& renderNode)
{
    auto iter = spatialEffectDepthNodeMap_.find(renderNode.GetId());
    if (iter == spatialEffectDepthNodeMap_.end()) {
        return std::weak_ptr<RSRenderNode>();
    }
    return iter->second.second.second;
}

void RSSpatialEffectManager::ProcessDepthNodeAndSpatialEffectNodeDirty()
{
    CleanExpiredNodes();

    ResourceNodeInfectsDepthNodeDirty(depthBackgroundNodeMap_);
    ResourceNodeInfectsDepthNodeDirty(depthResourceNodeMap_);

    masterGlobalDepthNodeMap_.clear();
    for (const auto& [_, entry] : depthSpatialEffectNodeMap_) {
        auto spNode = entry.first.lock();
        if (!spNode) {
            continue;
        }
        auto depthNode = spNode->template ReinterpretCastTo<RSDepthRenderNode>();
        if (!depthNode) {
            continue;
        }

        // Test whether this depthnode is master-global
        if (IsMasterGlobalDepthNodeAndUpdate(depthNode) && depthNode->IsDirty()) {
            depthNode->ApplyDepthSpaceModifier();
        }
    }

    for (const auto& [_, entry] : depthSpatialEffectNodeMap_) {
        auto spNode = entry.first.lock();
        if (!spNode) {
            continue;
        }
        auto depthNode = spNode->template ReinterpretCastTo<RSDepthRenderNode>();
        if (!depthNode) {
            continue;
        }

        // 1. If master-global is dirty, mark slave-global dirty
        if (depthNode->GetDepthSpaceType() == DepthSpaceType::GLOBAL) {
            auto globalDepthNode = GetMasterGlobalDepthNode(depthNode->GetLogicalDisplayNodeId()).lock();
            if (globalDepthNode && globalDepthNode->IsDirty()) {
                spNode->SetDirty();
            }
        }

        // 2. Mark spatialeffectnode of dirty depthnode dirty.
        if (spNode->IsDirty()) {
            SetSpatialEffectNodeDirty(entry.second);
        }
    }
}

void RSSpatialEffectManager::PrepareSpatialEffectParams()
{
    ProcessDepthResourceNodeParams();
    CopyGlobalDepthNodeParams();
}

void RSSpatialEffectManager::CleanNodeMap(NodeMap& nodeMap)
{
    EraseIf(nodeMap, [](const auto& pair) {
        return pair.second.expired();
    });
}

void RSSpatialEffectManager::CleanExpiredNodes()
{
    EraseIf(depthSpatialEffectNodeMap_, [](auto& pair) {
        auto depthNode = pair.second.first.lock();
        if (!depthNode) {
            return true;
        }
        EraseIf(pair.second.second, [](const auto& spatialPair) {
            auto spatialEffectNode = spatialPair.second.lock();
            if (!spatialEffectNode) {
                return true;
            }
            const auto& properties = spatialEffectNode->GetRenderProperties();
            if (!properties.GetSpatialEffectVariantPara().has_value()) {
                return true;
            }
            return false;
        });
        // Do not erase depth node when there are no spatial effect node under it,
        // because it may be used as a master-global depth node
        return false;
    });

    EraseIf(spatialEffectDepthNodeMap_, [](auto& pair) {
        auto spatialEffectNode = pair.second.first.lock();
        if (!spatialEffectNode) {
            return true;
        }
        const auto& properties = spatialEffectNode->GetRenderProperties();
        if (!properties.GetSpatialEffectVariantPara().has_value()) {
            return true;
        }
        return false;
    });

    CleanNodeMap(depthNodeMap_);
    CleanNodeMap(depthBackgroundNodeMap_);
    CleanNodeMap(depthResourceNodeMap_);
}

void RSSpatialEffectManager::ResourceNodeInfectsDepthNodeDirty(const NodeMap& resourceNodeMap)
{
    for (const auto& [_, wp] : resourceNodeMap) {
        auto resourceNode = wp.lock();
        if (!resourceNode || !resourceNode->IsDirty()) {
            continue;
        }
        auto parent = resourceNode->GetParent().lock();
        if (!parent) {
            continue;
        }
        parent->SetDirty();
    }
}

bool RSSpatialEffectManager::IsMasterGlobalDepthNodeAndUpdate(const std::shared_ptr<RSDepthRenderNode>& depthNode)
{
    if (!depthNode || depthNode->GetDepthSpaceType() != DepthSpaceType::INSTANCE) {
        return false;
    }
    auto instanceRootNode = depthNode->GetInstanceRootNode();
    if (!instanceRootNode) {
        return false;
    }
    auto surfaceNode = instanceRootNode->template ReinterpretCastTo<RSSurfaceRenderNode>();
    if (!surfaceNode || surfaceNode->GetSurfaceWindowType() != SurfaceWindowType::SCB_WALLPAPER) {
        return false;
    }
    masterGlobalDepthNodeMap_.emplace(depthNode->GetLogicalDisplayNodeId(), depthNode->weak_from_this());
    return true;
}

void RSSpatialEffectManager::SetSpatialEffectNodeDirty(const NodeMap& spatialEffectNodeMap)
{
    for (const auto& [_, wp] : spatialEffectNodeMap) {
        if (auto spatialEffectNode = wp.lock()) {
            spatialEffectNode->AddDirtyType(ModifierNG::RSModifierType::SPATIAL_EFFECT);
            spatialEffectNode->SetDirty();
            spatialEffectNode->GetMutableRenderProperties().SetGeoDirty();
        }
    }
}

void RSSpatialEffectManager::ProcessDepthResourceNodeParams()
{
    for (const auto& [_, wp] : depthResourceNodeMap_) {
        auto depthResourceNode = wp.lock();
        if (!depthResourceNode || !depthResourceNode->IsOnTheTree()) {
            continue;
        }
        auto surfaceNodeAsDepthRes = depthResourceNode->template ReinterpretCastTo<RSSurfaceRenderNode>();
        if (!surfaceNodeAsDepthRes || !surfaceNodeAsDepthRes->GetIsDepthResource()) {
            continue;
        }
        auto parent = surfaceNodeAsDepthRes->GetParent().lock();
        if (!parent || !parent->IsInstanceOf<RSDepthRenderNode>()) {
            continue;
        }
        auto depthNode = parent->template ReinterpretCastTo<RSDepthRenderNode>();
        if (!depthNode) {
            continue;
        }
        const auto& params = static_cast<RSDepthRenderParams*>(depthNode->GetStagingRenderParams().get());
        if (!params) {
            continue;
        }
        auto depthResourceDrawable = depthResourceNode->GetRenderDrawable();
        if (!depthResourceDrawable) {
            continue;
        }
        params->SetDepthSrcSurfaceDrawable(depthResourceDrawable->weak_from_this());
    }
}

void RSSpatialEffectManager::CopyGlobalDepthNodeParams()
{
    for (const auto& [_, wp] : depthNodeMap_) {
        auto spNode = wp.lock();
        if (!spNode || !spNode->IsOnTheTree()) {
            continue;
        }
        auto depthNode = spNode->template ReinterpretCastTo<RSDepthRenderNode>();
        if (!depthNode) {
            continue;
        }
        if (depthNode->GetDepthSpaceType() != DepthSpaceType::GLOBAL) {
            continue;
        }

        auto masterGlobalDepthNode = GetMasterGlobalDepthNode(depthNode->GetLogicalDisplayNodeId()).lock();
        if (!masterGlobalDepthNode) {
            continue;
        }
        const auto& masterGlobalParams =
            static_cast<RSDepthRenderParams*>(masterGlobalDepthNode->GetStagingRenderParams().get());
        if (!masterGlobalParams) {
            continue;
        }
        masterGlobalParams->OnSyncDepthSpaceParams(depthNode->GetStagingRenderParams());
    }
}
} // namespace Rosen
} // namespace OHOS