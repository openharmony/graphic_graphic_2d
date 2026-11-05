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

#include "rs_multi_display_screen_interpolation_helper.h"
#include "hdi_display_type.h"

namespace OHOS::Rosen {
RSMultiDisplayScreenInterpolationHelper::RSMultiDisplayScreenInterpolationHelper(uint32_t width, uint32_t height)
    : realWidth_(width), realHeight_(height) {}

void RSMultiDisplayScreenInterpolationHelper::BackupAndModifyDisplayInfo(
    GraphicDisplayCapability& capability, std::vector<GraphicDisplayModeInfo>& supportedModes)
{
    originCapability_ = capability;
    capability.phyWidth = realWidth_;
    capability.phyHeight = realHeight_;

    originSupportedModes_ = supportedModes;
    for (auto& mode : supportedModes) {
        mode.width = realWidth_;
        mode.height = realHeight_;
    }
}

std::optional<GraphicDisplayModeInfo> RSMultiDisplayScreenInterpolationHelper::GetOriginModeInfo(int32_t modeId) const
{
    auto iter = std::find_if(originSupportedModes_.cbegin(), originSupportedModes_.cend(),
        [modeId](const auto& mode) { return mode.id == modeId; });
    if (iter == originSupportedModes_.cend()) {
        return {};
    }
    return *iter;
}

const GraphicDisplayCapability& RSMultiDisplayScreenInterpolationHelper::GetOriginCapability() const
{
    return originCapability_;
}
} // namespace OHOS::Rosen
