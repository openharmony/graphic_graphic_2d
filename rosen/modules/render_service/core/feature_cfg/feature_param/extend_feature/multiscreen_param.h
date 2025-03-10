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
 
    bool IsExternalScreenSecure() const;
    bool IsSlrScaleEnabled() const;
    bool IsRsReportHwcDead() const;
    bool IsRsSetScreenPowerStatus() const;
 
protected:
    void SetExternalScreenSecure(bool isSecure);
    void SetSlrScaleEnabled(bool isEnabled);
    void SetRsReportHwcDead(bool isEnabled);
    void SetRsSetScreenPowerStatus(bool isEnabled);
 
private:
    bool isExternalScreenSecure_ = false;
    bool isSlrScaleEnabled_ = false;
    bool isRsReportHwcDead_ = false;
    bool isRsSetScreenPowerStatus_ = false;
 
    friend class MultiScreenParamParse;
};
} // namespace OHOS::Rosen
#endif // MULTISCREEN_PARAM_H