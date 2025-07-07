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

    static bool IsUIFirstEnable();
    static bool IsCardUIFirstEnable();
    static bool IsCacheOptimizeRotateEnable();
    static bool IsFreeMultiWindowEnable();
    static int GetUIFirstEnableWindowThreshold();
    static int GetUIFirstType();

protected:
    static void SetUIFirstEnable(bool isEnable);
    static void SetCardUIFirstEnable(bool isCardUIFirstEnable);
    static void SetCacheOptimizeRotateEnable(bool isCacheOptimizeRotateEnable);
    static void SetFreeMultiWindowEnable(bool isFreeMultiWindowEnable);
    static void SetUIFirstEnableWindowThreshold(int uiFirstEnableWindowThreshold);
    static void SetUIFirstType(int uiFirstType);

private:
    inline static bool isUIFirstEnable_ = true;
    inline static bool isCardUIFirstEnable_ = true;
    inline static bool isCacheOptimizeRotateEnable_ = false;
    inline static bool isFreeMultiWindowEnable_ = false;
    inline static int uiFirstEnableWindowThreshold_ = 0;
    inline static int uiFirstType_ = 1;

    friend class UIFirstParamParse;
};
} // namespace OHOS::Rosen
#endif // UIFIRST_PARAM_H