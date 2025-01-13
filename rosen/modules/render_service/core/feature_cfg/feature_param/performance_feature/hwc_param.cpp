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

#include "hwc_param.h"

namespace OHOS::Rosen {

bool HWCParam::IsHwcEnable() const
{
    return isHwcEnable_;
}

bool HWCParam::IsHwcMirrorEnable() const
{
    return isHwcMirrorEnable_;
}

int HWCParam::GetSourceTuningForApp(std::string appName) const
{
    auto iter = sourceTuningMap_.find(appName);
    if (iter != sourceTuningMap_.end()) {
        return iter->second;
    }
    HGM_LOGI("HWCParamParse parse SourceTuningForApp cannot find name %{public}s", appName.c_str());
    return -1;
}

int HWCParam::GetSolidColorLayerForApp(std::string appName) const
{
    auto iter = solidColorLayerMap_.find(appName);
    if (iter != solidColorLayerMap_.end()) {
        return iter->second;
    }
    HGM_LOGI("HWCParamParse parse SolidColorLayerForApp cannot find name %{public}s", appName.c_str());
    return -1;
}

void HWCParam::SetHwcEnable(bool isEnable)
{
    isHwcEnable_ = isEnable;
}

void HWCParam::SetHwcMirrorEnable(bool isEnable)
{
    isHwcMirrorEnable_ = isEnable;
}

void HWCParam::SetSourceTuningForApp(std::string appName, std::string val)
{
    sourceTuningMap_[appName] = std::atoi(val.c_str());
}

void HWCParam::SetSolidColorLayerForApp(std::string appName, std::string val)
{
    solidColorLayerMap_[appName] = std::atoi(val.c_str());
}
} // namespace OHOS::Rosen