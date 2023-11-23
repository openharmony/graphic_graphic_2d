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
#include "common/rs_vector4.h"
#include "pipeline/rs_render_node.h"

#define TWO 2
#define THREE 3
namespace OHOS {
namespace Rosen {

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
    auto& lightSourceMap = GetLightSourceMap();
    auto& illuminatedMap = GetIlluminatedMap();
    auto& dirtyLightSourceList = GetDirtyLightSourceList();
    auto& dirtyIlluminatedList = GetDirtyIlluminatedList();
    if (lightSourceMap.empty() || illuminatedMap.empty()) {
        ClearDirtyList();
        return;
    }
    if ((dirtyIlluminatedList.empty() && dirtyLightSourceList.empty())) {
        return;
    }
    PrepareLight(lightSourceMap, dirtyIlluminatedList, false);
    PrepareLight(illuminatedMap, dirtyLightSourceList, true);
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
        for (const auto& weakPtr : dirtyList) {
            auto dirtyNodePtr = weakPtr.lock();
            if (!dirtyNodePtr) {
                continue;
            }
            Vector4f lightSourceAbsPosition;
            std::shared_ptr<RSRenderNode> illuminatedNode = nullptr;
            if (isLightSourceDirty) {
                lightSourceAbsPosition = dirtyNodePtr->GetRenderProperties().GetLightSource()->GetAbsLightPosition();
                illuminatedNode = mapElm;
            } else {
                lightSourceAbsPosition = mapElm->GetRenderProperties().GetLightSource()->GetAbsLightPosition();
                illuminatedNode = dirtyNodePtr;
            }
            CheckIlluminated(lightSourceAbsPosition, illuminatedNode);
        }
        return false;
    });
}
void RSPointLightManager::CheckIlluminated(Vector4f lightSourceAbsPosition,
    const std::shared_ptr<RSRenderNode>& illuminatedNode)
{
    RectI effectAbsRect = (illuminatedNode->GetRenderProperties().GetBoundsGeometry())->GetAbsRect();
    int32_t radius = effectAbsRect.GetHeight() * THREE;
    effectAbsRect.SetAll(effectAbsRect.left_ - radius, effectAbsRect.top_ - radius, effectAbsRect.width_ + TWO * radius,
        effectAbsRect.height_ + TWO * radius);
    auto lightAbsPositionX = static_cast<int>(lightSourceAbsPosition[0]);
    auto lightAbsPositionY = static_cast<int>(lightSourceAbsPosition[1]);
    if (effectAbsRect.Intersect(lightAbsPositionX, lightAbsPositionY)) {
        illuminatedNode->GetRenderProperties().GetIlluminated()->SetIsIlluminated(true);
        illuminatedNode->SetDirty();
    } else {
        illuminatedNode->GetRenderProperties().GetIlluminated()->SetIsIlluminated(false);
    }
}
} // namespace Rosen
} // namespace OHOS