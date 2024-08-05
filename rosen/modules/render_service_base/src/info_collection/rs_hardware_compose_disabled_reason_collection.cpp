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
HwcDisabledReasonCollection& HwcDisabledReasonCollection::GetInstance()
{
    static HwcDisabledReasonCollection instance;
    return instance;
}

HwcDisabledReasonCollection::HwcDisabledReasonCollection()
{
}

HwcDisabledReasonCollection::~HwcDisabledReasonCollection() noexcept
{
}

void HwcDisabledReasonCollection::UpdateHwcDisabledReasonForDFX(NodeId id,
    int32_t disabledReason, const std::string& nodeName)
{
    std::lock_guard<std::mutex> lock(hwcDisabledReasonMtx_);
    auto& hwcDisabledReasonInfo = hwcDisabledReasonInfoMap_[id];
    ++hwcDisabledReasonInfo.disabledReasonStatistics[disabledReason];
    hwcDisabledReasonInfo.pidOfBelongsApp = ExtractPid(id);
    hwcDisabledReasonInfo.nodeName = nodeName;
}

HwcDisabledReasonInfos HwcDisabledReasonCollection::GetHwcDisabledReasonInfo()
{
    std::lock_guard<std::mutex> lock(hwcDisabledReasonMtx_);
    HwcDisabledReasonInfos hwcDisabledReasonInfos;
    for (const auto& hwcDisabledReasonInfo : hwcDisabledReasonInfoMap_) {
        hwcDisabledReasonInfos.emplace_back(hwcDisabledReasonInfo.second);
    }
    hwcDisabledReasonInfoMap_.clear();
    return hwcDisabledReasonInfos;
}
} // namespace Rosen
} // namespace OHOS