/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_anco_manager.h"
#include <memory>


#include "params/rs_surface_render_params.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSAncoManager* RSAncoManager::Instance()
{
    static RSAncoManager instance;
    return &instance;
}

AncoHebcStatus RSAncoManager::GetAncoHebcStatus() const
{
    return static_cast<AncoHebcStatus>(ancoHebcStatus_.load());
}

void RSAncoManager::SetAncoHebcStatus(AncoHebcStatus hebcStatus)
{
    ancoHebcStatus_.store(static_cast<int32_t>(hebcStatus));
}

bool RSMainThread::AncoOptimizeCheck(bool isHebc, int nodesCnt, int sfvNodesCnt)
{
    constexpr int MIN_OPTIMIZE_ANCO_NUMS = 3;
    constexpr int MIN_OPTIMIZE_ANCO_SFV_NUMS = 2;
    bool numsMatch = nodesCnt == MIN_OPTIMIZE_ANCO_NUMS && sfvNodesCnt == MIN_OPTIMIZE_ANCO_SFV_NUMS;
    if (numsMatch && isHebc) {
        RS_LOGI("doDirect anco disable hebc");
        SetAncoHebcStatus(AncoHebcStatus::NOT_USE_HEBC);
        return true;
    }
    if (!numsMatch && !isHebc) {
        RS_LOGI("doDirect anco enable hebc");
        SetAncoHebcStatus(AncoHebcStatus::USE_HEBC);
        return true;
    }
    SetAncoHebcStatus(AncoHebcStatus::INITIAL);
    return false;
}

bool AncoOptimizeDisplayNode(std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes,
        ScreenRotation rotation, int width, int height)
{
    if (!RSSurfaceRenderNode::GetOriAncoForceDoDirect() || !RSSystemProperties::IsTabletType() ||
        displayNode->GetRotation() != ScreenRotation::ROTATION_0) {
        return false;
    }

    if (surfaceHandler == nullptr || surfaceHandler->GetBuffer() == nullptr) {
        return false;
    }
    bool isHebc = true;
    if ((surfaceHandler->GetBuffer()->GetUsage() & BUFFER_USAGE_CPU_READ) ||
        (surfaceHandler->GetBuffer()->GetUsage() & BUFFER_USAGE_CPU_WRITE)) {
        isHebc = false;
    }

    // process displayNode rect
    int minDisplayW = static_cast<int32_t>(width / 2);
    int minDisplayH = static_cast<int32_t>(height / 2);
    if (minDisplayW <= 0 && minDisplayH <= 0) {
        return false;
    }

    int nodesCnt = 0;
    int sfvNodesCnt = 0;
    for (auto& surfaceNode : hardwareEnabledNodes_) {
        if ((surfaceNode->GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)) == 0) {
            continue;
        }
        auto alpha = surfaceNode->GetGlobalAlpha();
        if (ROSEN_EQ(alpha, 0.0f) || !surfaceNode->GetRSSurfaceHandler() ||
            !surfaceNode->GetRSSurfaceHandler()->GetBuffer()) {
            continue;
        }
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
        if (params == nullptr) {
            continue;
        }
        auto& layerInfo = params->GetLayerInfo().dstRect;
        if (layerInfo.w >= minDisplayW && layerInfo.h >= minDisplayH) {
            nodesCnt++;
            if (surfaceNode->GetAncoFlags() == static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE)) {
                sfvNodesCnt++;
            }
        }
    }
    return AncoOptimizeCheck(isHebc, nodesCnt, sfvNodesCnt);
}

} // namespace Rosen
} // namespace OHOS
