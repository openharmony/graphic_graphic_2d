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

RSPointLightManager* RSPointLightManager::Instance()
{
    static RSPointLightManager instance;
    return &instance;
}

void RSPointLightManager::RegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    lightSourceNodeMap_.emplace(nodeId, renderNode->weak_from_this());
}

void RSPointLightManager::RegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    illuminatedNodeMap_.emplace(nodeId, renderNode->weak_from_this());
}

void RSPointLightManager::UnRegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    lightSourceNodeMap_.erase(nodeId);
}
void RSPointLightManager::UnRegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode)
{
    NodeId nodeId = renderNode->GetId();
    illuminatedNodeMap_.erase(nodeId);
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
    for (auto &[_, weakPtr] : illuminatedNodeMap_) {
        if (auto node = weakPtr.lock()) {
            node->UpdatePointLightDirtySlot();
        }
    }
    if (lightSourceNodeMap_.empty() || illuminatedNodeMap_.empty()) {
        ClearDirtyList();
        return;
    }
    if ((dirtyIlluminatedList_.empty() && dirtyLightSourceList_.empty())) {
        return;
    }
    for (const auto& illuminatedWeakPtr : dirtyIlluminatedList_) {
        auto illuminatedNodePtr = illuminatedWeakPtr.lock();
        if (illuminatedNodePtr) {
            illuminatedNodePtr->GetRenderProperties().GetIlluminated()->ClearLightSourcesAndPosMap();
        }
    }
    PrepareLight(lightSourceNodeMap_, dirtyIlluminatedList_, false);
    PrepareLight(illuminatedNodeMap_, dirtyLightSourceList_, true);
    ClearDirtyList();
}
void RSPointLightManager::PrepareLight(std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>& map,
    std::vector<std::weak_ptr<RSRenderNode>>& dirtyList, bool isLightSourceDirty)
{
    EraseIf(map, [this, isLightSourceDirty, &dirtyList](const auto& pair) -> bool {
        auto mapElm = pair.second.lock();
        if (!mapElm) {
            return true;
        }
        if (!mapElm->IsOnTheTree()) { // skip check when node is not on the tree
            return false;
        }
        for (const auto& weakPtr : dirtyList) {
            auto dirtyNodePtr = weakPtr.lock();
            if (!dirtyNodePtr) {
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
    const auto& geoPtr = (illuminatedNode->GetRenderProperties().GetBoundsGeometry());
    auto lightSourcePtr = lightSourceNode->GetRenderProperties().GetLightSource();
    if (!geoPtr || geoPtr->IsEmpty() || !lightSourcePtr) {
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
    auto illuminatedRootNodeId = illuminatedNode->GetInstanceRootNodeId();
    auto lightSourceRootNodeId = lightSourceNode->GetInstanceRootNodeId();
    if (inIlluminatedRange && illuminatedRootNodeId == lightSourceRootNodeId) {
        auto lightPos = CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedAbsRect);
        illuminatedNode->GetRenderProperties().GetIlluminated()->AddLightSourcesAndPos(lightSourcePtr, lightPos);
        illuminatedNode->SetDirty();
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
    lightPos.w_ = lightPosition.w_;
    return lightPos;
}

} // namespace Rosen
} // namespace OHOS