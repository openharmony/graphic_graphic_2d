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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_GLOBAL_LIGHT_MANAGER_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_GLOBAL_LIGHT_MANAGER_H

#include "common/rs_vector4.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_def.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPointLightManager {
public:
    static RSPointLightManager* Instance();
    void RegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode);
    void RegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode);
    void UnRegisterLightSource(const std::shared_ptr<RSRenderNode>& renderNode);
    void UnRegisterIlluminated(const std::shared_ptr<RSRenderNode>& renderNode);
    void AddDirtyLightSource(std::weak_ptr<RSRenderNode> renderNode);
    void AddDirtyIlluminated(std::weak_ptr<RSRenderNode> renderNode);
    void PrepareLight();
    Vector4f CalculateLightPosForIlluminated(const RSLightSource& lightSource,
        const RectI& illuminatedAbsRect);
    void SetScreenRotation(ScreenRotation screenRotation)
    {
        screenRotation_ = screenRotation;
    }
    ScreenRotation GetScreenRotation() const
    {
        return screenRotation_;
    }

private:
    RSPointLightManager() = default;
    ~RSPointLightManager() = default;
    RSPointLightManager(const RSPointLightManager&) = delete;
    RSPointLightManager(const RSPointLightManager&&) = delete;
    RSPointLightManager& operator=(const RSPointLightManager&) = delete;
    RSPointLightManager& operator=(const RSPointLightManager&&) = delete;

    void ClearDirtyList();
    void CheckIlluminated(
        const std::shared_ptr<RSRenderNode>& lightSourceNode, const std::shared_ptr<RSRenderNode>& illuminatedNode);
    void PrepareLight(std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>& map,
        std::vector<std::weak_ptr<RSRenderNode>>& dirtyList, bool isLightSourceDirty);
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> lightSourceNodeMap_;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> illuminatedNodeMap_;
    std::vector<std::weak_ptr<RSRenderNode>> dirtyLightSourceList_;
    std::vector<std::weak_ptr<RSRenderNode>> dirtyIlluminatedList_;
    ScreenRotation screenRotation_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_GLOBAL_LIGHT_MANAGER_H