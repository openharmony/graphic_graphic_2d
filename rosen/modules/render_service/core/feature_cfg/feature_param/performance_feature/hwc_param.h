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

    static bool IsDisableHwcOnExpandScreen();
    static bool IsSolidLayerEnable();

protected:
    void SetSourceTuningForApp(std::string appName, std::string val);
    void SetSolidColorLayerForApp(std::string appName, std::string val);
    void SetEnableHwcNodeBelowSelfInAppForApp(std::string appName, std::string val);
    static void SetSolidLayerEnable(bool isEnable);
    static void SetDisableHwcOnExpandScreen(bool isEnable);
    void MoveDataToHgmCore();

private:
    inline static bool isSolidLayerEnable_ = false;
    inline static bool isDisableHwcOnExpandScreen_ = false;
    std::unordered_map<std::string, std::string> sourceTuningMap_;
    std::unordered_map<std::string, std::string> solidColorLayerMap_;
    std::unordered_map<std::string, std::string> enableHwcNodeBelowSelfInAppMap_;

    friend class HWCParamParse;
};
} // namespace OHOS::Rosen
#endif // HWC_PARAM_H