/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_hwc_context.h"

#include "hgm_core.h"
#include "hwc_param.h"
#include "common/rs_common_hook.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
// use in temporary scheme to check package name
void RSHWCContext::CheckPackageInConfigList(const std::vector<std::string>& pkgs)
{
    auto& rsCommonHook = RsCommonHook::Instance();
    rsCommonHook.SetVideoSurfaceFlag(false);
    rsCommonHook.SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(false);
    rsCommonHook.SetHardwareEnabledByBackgroundAlphaFlag(false);
    rsCommonHook.SetIsWhiteListForSolidColorLayerFlag(false);

    auto& videoConfigFromHgm = sourceTuningConfig_;
    auto& solidLayerConfigFromHgm = solidLayerConfig_;
    auto& hwcVideoConfigFromHgm = hwcSourceTuningConfig_;
    auto& hwcSolidLayerConfigFromHgm = hwcSolidLayerConfig_;
    static bool unused = [&solidLayerConfigFromHgm, &hwcSolidLayerConfigFromHgm, &rsCommonHook]() {
        rsCommonHook.SetSolidColorLayerConfigFromHgm(solidLayerConfigFromHgm);
        rsCommonHook.SetHwcSolidColorLayerConfigFromHgm(hwcSolidLayerConfigFromHgm);
        return true;
    }();
    if (pkgs.size() > 1) {
        return;
    }
    for (auto& param : pkgs) {
        std::string pkgNameForCheck = param.substr(0, param.find(':'));
        // 1 means crop source tuning
        auto videoIter = videoConfigFromHgm.find(pkgNameForCheck);
        auto hwcVideoIter = hwcVideoConfigFromHgm.find(pkgNameForCheck);
        if ((videoIter != videoConfigFromHgm.end() && videoIter->second == "1") ||
            (hwcVideoIter != hwcVideoConfigFromHgm.end() && hwcVideoIter->second == "1")) {
            rsCommonHook.SetVideoSurfaceFlag(true);
        // 2 means skip hardware disabled by hwc node and background alpha
        } else if ((videoIter != videoConfigFromHgm.end() && videoIter->second == "2") ||
                   (hwcVideoIter != hwcVideoConfigFromHgm.end() && hwcVideoIter->second == "2")) {
            rsCommonHook.SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);
            rsCommonHook.SetHardwareEnabledByBackgroundAlphaFlag(true);
        }
        // 1 means enable dss by solid color layer
        auto iter = solidLayerConfigFromHgm.find(pkgNameForCheck);
        auto hwcIter = hwcSolidLayerConfigFromHgm.find(pkgNameForCheck);
        if ((iter != solidLayerConfigFromHgm.end() && iter->second == "1") ||
            (hwcIter != hwcSolidLayerConfigFromHgm.end() && hwcIter->second == "1")) {
            rsCommonHook.SetIsWhiteListForSolidColorLayerFlag(true);
        }
    }
}
} // namespace OHOS::Rosen