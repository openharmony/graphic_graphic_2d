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

#ifndef HWC_PARAM_H
#define HWC_PARAM_H

#include <unordered_map>

#include "feature_param.h"

namespace OHOS::Rosen {
class HWCParam : public FeatureParam {
public:
    HWCParam() = default;
    ~HWCParam() = default;

    bool IsHwcEnable();
    bool IsHwcMirrorEnable();
    int GetSourceTuningForApp(std::string appName);
    int GetSolidColorLayerForApp(std::string appName);

    void SetHwcEnable(bool isEnable);
    void SetHwcMirrorEnable(bool isEnable);
    void SetSourceTuningForApp(std::string appName, std::string val);
    void SetSolidColorLayerForApp(std::string appName, std::string val);

private:
    bool hwcEnable;
    bool hwcMirrorEnable;

    std::unordered_map<std::string, int> sourceTuningMap;
    std::unordered_map<std::string, int> solidColorLayerMap;
};
} // namespace OHOS::Rosen
#endif // HWC_PARAM_H