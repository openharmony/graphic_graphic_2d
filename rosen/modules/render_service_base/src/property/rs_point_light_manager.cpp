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

#include "property/rs_point_light_manager.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_def.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
constexpr int TWO = 2;
constexpr size_t CLEANUP_THRESHOLD = 500;

RSPointLightManager* RSPointLightManager::Instance()
{
    static RSPointLightManager instance;
    return &instance;
}

void RSPointLightManager::RegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    NodeId nodeId = renderNode->GetId();
    lightSourceNodeMap_.emplace(nodeId, renderNode->weak_from_this());
}

void RSPointLightManager::RegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    NodeId nodeId = renderNode->GetId();
    illuminatedNodeMap_.emplace(nodeId, renderNode->weak_from_this());
}

void RSPointLightManager::UnRegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    NodeId nodeId = renderNode->GetId();
    lightSourceNodeMap_.erase(nodeId);
}
void RSPointLightManager::UnRegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return;
    }
    NodeId nodeId = renderNode->GetId();
    illuminatedNodeMap_.erase(nodeId);
}
void RSPointLightManager::SetChildHasVisibleIlluminated(
    const std::shared_ptr<RSRenderNode>& renderNode, bool hasVisibleIlluminated)
{
    if (!renderNode) {
        return;
    }
    NodeId id = renderNode->GetId();
    auto weak_ptr = renderNode->weak_from_this();
    if (hasVisibleIlluminated) {
        childHasVisibleIlluminatedNodeMap_[id] = weak_ptr;
    } else {
        childHasVisibleIlluminatedNodeMap_.erase(id);
    }
}
bool RSPointLightManager::GetChildHasVisibleIlluminated(const std::shared_ptr<RSRenderNode>& renderNode)
{
    if (!renderNode) {
        return false;
    }
    NodeId id = renderNode->GetId();
    auto it = childHasVisibleIlluminatedNodeMap_.find(id);
    return it != childHasVisibleIlluminatedNodeMap_.end();
}
bool RSPointLightManager::HasVisibleIlluminated(const std::shared_ptr<RSRenderNode>& illuminatedNodePtr)
{
    if (!illuminatedNodePtr) {
        return false;
    }
    auto illuminated = illuminatedNodePtr->GetRenderProperties().GetIlluminated();
    if (!illuminated || !illuminated->IsIlluminatedValid()) {
        return false;
    }
    return std::any_of(lightSourceNodeMap_.begin(), lightSourceNodeMap_.end(), [illuminatedNodePtr](const auto& pair) {
        auto lightSourceNodePtr = pair.second.lock();
        if (!lightSourceNodePtr || !lightSourceNodePtr->IsOnTheTree()) {
            return false;
        }
        return illuminatedNodePtr->GetInstanceRootNodeId() == lightSourceNodePtr->GetInstanceRootNodeId();
    });
}
void RSPointLightManager::AddDirtyLightSource(std::weak_ptr<RSRenderNode> renderNode)
{
    dirtyLightSourceList_.emplace_back(renderNode);
}
void RSPointLightManager::AddDirtyIlluminated(std::weak_ptr<RSRenderNode> renderNode)
{
    dirtyIlluminatedList_.emplace_back(renderNode);
}
void RSPointLightManager::ClearDirtyList()
{
    dirtyLightSourceList_.clear();
    dirtyIlluminatedList_.clear();
}
void RSPointLightManager::PrepareLight()
{
    // Early exit if there are no illuminated nodes in previous frame and dirty nodes.
    if ((dirtyIlluminatedList_.empty() && dirtyLightSourceList_.empty() && previousFrameIlluminatedNodeMap_.empty())) {
        return;
    }
    // Collect the nodes illuminated by the light source in the previous frame.
    CollectPreviousFrameIlluminatedNodes();
    // Checks illumination relationships between light sources and illuminable nodes, marking illuminated nodes as
    // dirty.
    PrepareLight(lightSourceNodeMap_, dirtyIlluminatedList_, false);
    PrepareLight(illuminatedNodeMap_, dirtyLightSourceList_, true);
    // Mark nodes as dirty if they were illuminated in the previous frame but not in the current frame.
    ProcessLostIlluminationNode();
    ClearDirtyList();
    // Clean up expired node in childHasVisibleIlluminatedNodeMap_.
    if (childHasVisibleIlluminatedNodeMap_.size() > CLEANUP_THRESHOLD) {
        EraseIf(childHasVisibleIlluminatedNodeMap_, [](const auto& pair) -> bool { return pair.second.expired(); });
    }
}

void RSPointLightManager::PrepareLight(std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>& map,
    std::vector<std::weak_ptr<RSRenderNode>>& dirtyList, bool isLightSourceDirty)
{
    EraseIf(map, [this, isLightSourceDirty, &dirtyList](const auto& pair) -> bool {
        auto mapElm = pair.second.lock();
        if (!mapElm || !mapElm->IsOnTheTree()) {
            return !mapElm;
        }
        for (const auto& weakPtr : dirtyList) {
            auto dirtyNodePtr = weakPtr.lock();
            if (!dirtyNodePtr || !dirtyNodePtr->IsOnTheTree()) {
                continue;
            }
            std::shared_ptr<RSRenderNode> lightSourceNode = isLightSourceDirty ? dirtyNodePtr : mapElm;
            std::shared_ptr<RSRenderNode> illuminatedNode = isLightSourceDirty ? mapElm : dirtyNodePtr;
            CheckIlluminated(lightSourceNode, illuminatedNode);
        }
        return false;
    });
}
void RSPointLightManager::CheckIlluminated(
    const std::shared_ptr<RSRenderNode>& lightSourceNode, const std::shared_ptr<RSRenderNode>& illuminatedNode)
{
    if (illuminatedNode->GetInstanceRootNodeId() != lightSourceNode->GetInstanceRootNodeId()) {
        return;
    }
    const auto& geoPtr = (illuminatedNode->GetRenderProperties().GetBoundsGeometry());
    auto lightSourcePtr = lightSourceNode->GetRenderProperties().GetLightSource();
    auto illuminatedPtr = illuminatedNode->GetRenderProperties().GetIlluminated();
    if (!geoPtr || geoPtr->IsEmpty() || !lightSourcePtr || !illuminatedPtr) {
        return;
    }
    RectI illuminatedAbsRect = geoPtr->GetAbsRect();
    int radius = static_cast<int>(lightSourcePtr->GetLightRadius());
    auto illuminatedRange = RectI(illuminatedAbsRect.left_ - radius, illuminatedAbsRect.top_ - radius,
        illuminatedAbsRect.width_ + TWO * radius, illuminatedAbsRect.height_ + TWO * radius);
    const auto& lightSourceAbsPosition = lightSourcePtr->GetAbsLightPosition();
    auto lightAbsPositionX = static_cast<int>(lightSourceAbsPosition[0]);
    auto lightAbsPositionY = static_cast<int>(lightSourceAbsPosition[1]);
    auto rotation = GetScreenRotation();
    auto inIlluminatedRange = false;
    if (rotation == ScreenRotation::ROTATION_0 || rotation == ScreenRotation::ROTATION_180) {
        inIlluminatedRange = illuminatedRange.Intersect(lightAbsPositionX, lightAbsPositionY);
    } else if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        inIlluminatedRange = illuminatedRange.Intersect(lightAbsPositionY, lightAbsPositionX);
    }

    if (inIlluminatedRange && !illuminatedPtr->GetLightSourcesAndPosMap().count(lightSourcePtr)) {
        auto lightPos = CalculateLightPosForIlluminated(*lightSourcePtr, geoPtr->GetAbsRect());
        illuminatedPtr->AddLightSourcesAndPos(lightSourcePtr, lightPos);
        MarkIlluminatedNodeDirty(illuminatedNode);
    }
}

Vector4f RSPointLightManager::CalculateLightPosForIlluminated(
    const RSLightSource& lightSource, const RectI& illuminatedAbsRect)
{
    Vector4f lightPos;
    auto lightSourceAbsPosition = lightSource.GetAbsLightPosition();
    auto lightPosition = lightSource.GetLightPosition();
    auto rotation = GetScreenRotation();
    switch (rotation) {
        case ScreenRotation::ROTATION_0:
            lightPos.x_ = lightSourceAbsPosition.x_ - illuminatedAbsRect.GetLeft();
            lightPos.y_ = lightSourceAbsPosition.y_ - illuminatedAbsRect.GetTop();
            break;
        case ScreenRotation::ROTATION_90:
            lightPos.x_ = illuminatedAbsRect.GetBottom() - lightSourceAbsPosition.x_;
            lightPos.y_ = lightSourceAbsPosition.y_ - illuminatedAbsRect.GetLeft();
            break;
        case ScreenRotation::ROTATION_180:
            lightPos.x_ = illuminatedAbsRect.GetRight() - lightSourceAbsPosition.x_;
            lightPos.y_ = illuminatedAbsRect.GetBottom() - lightSourceAbsPosition.y_;
            break;
        case ScreenRotation::ROTATION_270:
            lightPos.x_ = lightSourceAbsPosition.x_ - illuminatedAbsRect.GetTop();
            lightPos.y_ = illuminatedAbsRect.GetRight() - lightSourceAbsPosition.y_;
            break;
        default:
            break;
    }
    lightPos.z_ = lightPosition.z_;
    // store the lightRadius at lightPos.w_
    lightPos.w_ = ROSEN_EQ(lightSource.GetLightRadius(), 0.0f) ? 1.0f : lightSource.GetLightRadius();
    return lightPos;
}

void RSPointLightManager::CollectPreviousFrameIlluminatedNodes()
{
    previousFrameIlluminatedNodeMap_.clear();
    for (auto& [id, weakPtr] : illuminatedNodeMap_) {
        auto illuminatedNodePtr = weakPtr.lock();
        if (!illuminatedNodePtr) {
            continue;
        }
        auto illuminatedPtr = illuminatedNodePtr->GetRenderProperties().GetIlluminated();
        if (illuminatedPtr && illuminatedPtr->IsIlluminated()) {
            previousFrameIlluminatedNodeMap_.emplace(id, weakPtr);
            illuminatedPtr->ClearLightSourcesAndPosMap();
        }
    }
}

void RSPointLightManager::ProcessLostIlluminationNode()
{
    if (previousFrameIlluminatedNodeMap_.empty()) {
        return;
    }
    for (auto& [_, weakPtr] : previousFrameIlluminatedNodeMap_) {
        auto illuminatedNodePtr = weakPtr.lock();
        if (!illuminatedNodePtr || !illuminatedNodePtr->IsOnTheTree()) {
            continue;
        }
        auto illuminatedPtr = illuminatedNodePtr->GetRenderProperties().GetIlluminated();
        // Mark nodes as dirty if they were illuminated in the previous frame but are no longer illuminated in the
        // current frame.
        if (illuminatedPtr && !illuminatedPtr->IsIlluminated()) {
            MarkIlluminatedNodeDirty(illuminatedNodePtr);
        }
    }
}

void RSPointLightManager::MarkIlluminatedNodeDirty(const std::shared_ptr<RSRenderNode>& illuminatedNodePtr)
{
    if (!illuminatedNodePtr) {
        return;
    }
    // The illuminated nodes's point light properties did not change, so it's point light drawable slot was not marked
    // dirty, we need to manually mark point light slot dirty.
    illuminatedNodePtr->UpdatePointLightDirtySlot();
    illuminatedNodePtr->SetDirty();
}

} // namespace Rosen
} // namespace OHOS