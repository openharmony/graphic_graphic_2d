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

static bool IsLightSourceAffectIlluminatedNode(const Vector4f& pos, const RectF& rect, float radius)
{
    float closestX = std::max(rect.left_, std::min(pos.x_, rect.GetRight()));
    float closestY = std::max(rect.top_, std::min(pos.y_, rect.GetBottom()));

    float dx = pos.x_ - closestX;
    float dy = pos.y_ - closestY;
    return ROSEN_LE(dx * dx + dy * dy, radius * radius);
}

void RSPointLightManager::CheckIlluminated(
    const std::shared_ptr<RSRenderNode>& lightSourceNode, const std::shared_ptr<RSRenderNode>& illuminatedNode)
{
    if (illuminatedNode->GetInstanceRootNodeId() != lightSourceNode->GetInstanceRootNodeId()) {
        return;
    }
    auto lightSourcePtr = lightSourceNode->GetRenderProperties().GetLightSource();
    auto illuminatedPtr = illuminatedNode->GetRenderProperties().GetIlluminated();
    if (!lightSourcePtr || !illuminatedPtr) {
        return;
    }
    // Calculate the relative position of the light source and illuminable node.
    auto lightRelativePosOpt = CalculateLightRelativePosition(lightSourceNode, illuminatedNode);
    if (!lightRelativePosOpt.has_value()) {
        return;
    }
    const Vector4f lightRelativePos = lightRelativePosOpt.value();
    // Check if light source affect the illuminable node.
    bool isIlluminated = IsLightSourceAffectIlluminatedNode(lightRelativePos,
        illuminatedNode->GetRenderProperties().GetBoundsRect(), lightSourcePtr->GetLightRadius());
    // Mark the nodes as dirty where light added as dirty.
    if (isIlluminated && !illuminatedPtr->GetLightSourcesAndPosMap().count(lightSourcePtr)) {
        illuminatedPtr->AddLightSourcesAndPos(lightSourcePtr, lightRelativePos);
        MarkIlluminatedNodeDirty(illuminatedNode);
    }
}
std::optional<Vector4f> RSPointLightManager::CalculateLightRelativePosition(
    const std::shared_ptr<RSRenderNode>& lightSourceNodePtr, const std::shared_ptr<RSRenderNode>& illuminatedNodePtr)
{
    const auto& lightGeoPtr = lightSourceNodePtr->GetRenderProperties().GetBoundsGeometry();
    const auto& illuminatedGeoPtr = illuminatedNodePtr->GetRenderProperties().GetBoundsGeometry();
    if (!lightGeoPtr || lightGeoPtr->IsEmpty() || !illuminatedGeoPtr || illuminatedGeoPtr->IsEmpty()) {
        return std::nullopt;
    }
    const auto& lightSourcePtr = lightSourceNodePtr->GetRenderProperties().GetLightSource();
    if (!lightSourcePtr) {
        return std::nullopt;
    }
    Drawing::Matrix invertMatrix;
    if (!illuminatedGeoPtr->GetAbsMatrix().Invert(invertMatrix)) {
        return std::nullopt;
    }
    const Vector4f lightLocalPos = lightSourcePtr->GetLightPosition();
    Drawing::Point lightLocalPoint = { lightLocalPos.x_, lightLocalPos.y_ };
    std::vector<Drawing::Point> lightLocalPointVec = { lightLocalPoint };
    constexpr int pointVecSize = 1;
    std::vector<Drawing::Point> transformedPointVec(pointVecSize);
    std::vector<Drawing::Point> lightRelativePointVec(pointVecSize);
    // Transform light position from lightSource node's local coordinates to abs coordinates.
    lightGeoPtr->GetAbsMatrix().MapPoints(transformedPointVec, lightLocalPointVec, pointVecSize);
    // Transform light position from lightSource node's abs coordinates to illuminable node's local coordinates, then we
    // can get the relative position of the light source and illuminatable node.
    invertMatrix.MapPoints(lightRelativePointVec, transformedPointVec, pointVecSize);
    Drawing::Point lightRelativePoint = lightRelativePointVec[0];
    const float radius = lightSourcePtr->GetLightRadius();
    // store the light radius at lightRelativePosition.w_.
    Vector4f lightRelativePosition = { lightRelativePoint.GetX(), lightRelativePoint.GetY(), lightLocalPos.z_,
        ROSEN_EQ(radius, 0.0f) ? 1.0f : radius };
    return lightRelativePosition;
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