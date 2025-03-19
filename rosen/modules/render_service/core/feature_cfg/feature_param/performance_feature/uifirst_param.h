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

#ifndef UIFIRST_PARAM_H
#define UIFIRST_PARAM_H

#include "feature_param.h"

namespace OHOS::Rosen {
class UIFirstParam : public FeatureParam {
public:
    UIFirstParam() = default;
    ~UIFirstParam() = default;

    bool IsUIFirstEnable() const;
    bool IsCardUIFirstEnable() const;
    bool IsCacheOptimizeRotateEnable() const;
    bool IsFreeMultiWindowEnable() const;
    int GetUIFirstEnableWindowThreshold() const;
    int GetUIFirstType() const;

protected:
    void SetUIFirstEnable(bool isEnable);
    void SetCardUIFirstEnable(bool isCardUIFirstEnable);
    void SetCacheOptimizeRotateEnable(bool isCacheOptimizeRotateEnable);
    void SetFreeMultiWindowEnable(bool isFreeMultiWindowEnable);
    void SetUIFirstEnableWindowThreshold(int uiFirstEnableWindowThreshold);
    void SetUIFirstType(int uiFirstType);

private:
    bool isUIFirstEnable_ = true;
    bool isCardUIFirstEnable_ = true;
    bool isCacheOptimizeRotateEnable_ = false;
    bool isFreeMultiWindowEnable_ = false;
    int uiFirstEnableWindowThreshold_ = 1;
    int uiFirstType_ = 1;

    friend class UIFirstParamParse;
};
} // namespace OHOS::Rosen
#endif // UIFIRST_PARAM_H