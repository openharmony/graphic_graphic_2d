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

#include "info_collection/rs_hardware_compose_disabled_reason_collection.h"

namespace OHOS {
namespace Rosen {
HardwareComposeDisabledReasonCollection& HardwareComposeDisabledReasonCollection::GetInstance()
{
    static HardwareComposeDisabledReasonCollection instance;
    return instance;
}

HardwareComposeDisabledReasonCollection::HardwareComposeDisabledReasonCollection()
{
}

HardwareComposeDisabledReasonCollection::~HardwareComposeDisabledReasonCollection() noexcept
{
}

void HardwareComposeDisabledReasonCollection::UpdateHardwareComposeDisabledReasonInfoForDFX(NodeId id,
    int32_t disabledReason, const std::string& nodeName)
{
    std::lock_guard<std::mutex> lock(hardwareComposeDisabledReasonMtx_);
    ++hardwareComposeDisabledReasonInfoMap_[id].disabledReasonStatistics[disabledReason];
    activeDirtyRegionInfoMap_[id].pidOfBelongsApp = ExtractPid(id);
    activeDirtyRegionInfoMap_[id].nodeName = nodeName;
}

std::vector<HardwareComposeDisabledReasonInfo> HardwareComposeDisabledReasonCollection::GetHardwareComposeDisabledReasonInfo() const
{
    std::lock_guard<std::mutex> lock(hardwareComposeDisabledReasonMtx_);
    std::vector<HardwareComposeDisabledReasonInfo> hardwareComposeDisabledReasonInfos;
    for (const auto& hardwareComposeDisabledReasonInfo : hardwareComposeDisabledReasonInfoMap_) {
        hardwareComposeDisabledReasonInfos.emplace_back(hardwareComposeDisabledReasonInfo.second);
    }
    return hardwareComposeDisabledReasonInfos;
}

void HardwareComposeDisabledReasonCollection::ResetGlobalDirtyRegionInfo()
{
    std::lock_guard<std::mutex> lock(hardwareComposeDisabledReasonMtx_);
    hardwareComposeDisabledReasonInfoMap_.clear();
}
} // namespace Rosen
} // namespace OHOS