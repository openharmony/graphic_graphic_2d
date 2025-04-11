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

#ifndef MULTISCREEN_PARAM_H
#define MULTISCREEN_PARAM_H
 
#include "feature_param.h"
 
namespace OHOS::Rosen {
class MultiScreenParam : public FeatureParam {
public:
    MultiScreenParam() = default;
    ~MultiScreenParam() = default;
 
    static bool IsExternalScreenSecure();
    static bool IsSlrScaleEnabled();
    static bool IsRsReportHwcDead();
    static bool IsRsSetScreenPowerStatus();
    static bool IsMirrorDisplayCloseP3();

protected:
    static void SetExternalScreenSecure(bool isSecure);
    static void SetSlrScaleEnabled(bool isEnabled);
    static void SetRsReportHwcDead(bool isEnabled);
    static void SetRsSetScreenPowerStatus(bool isEnabled);
    static void SetMirrorDisplayCloseP3(bool isEnabled);

private:
    inline static bool isExternalScreenSecure_ = false;
    inline static bool isSlrScaleEnabled_ = false;
    inline static bool isRsReportHwcDead_ = false;
    inline static bool isRsSetScreenPowerStatus_ = true;
    inline static bool isMirrorDisplayCloseP3_ = true;
 
    friend class MultiScreenParamParse;
};
} // namespace OHOS::Rosen
#endif // MULTISCREEN_PARAM_H