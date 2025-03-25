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

#ifndef ACCESSIBILITY_PARAM_H
#define ACCESSIBILITY_PARAM_H

#include <unordered_map>

#include "feature_param.h"

namespace OHOS::Rosen {
class AccessibilityParam : public FeatureParam {
public:
    AccessibilityParam() = default;
    ~AccessibilityParam() = default;

    static bool IsHighContrastEnabled();
    static bool IsCurtainScreenEnabled();
    static bool IsColorReverseEnabled();
    static bool IsColorCorrectionEnabled();

protected:
    static void SetHighContrastEnabled(bool isEnable);
    static void SetCurtainScreenEnabled(bool isEnable);
    static void SetColorReverseEnabled(bool isEnable);
    static void SetColorCorrectionEnabled(bool isEnable);

private:
    inline static bool isHighContrastEnabled_ = true;
    inline static bool isCurtainScreenEnabled_ = true;
    inline static bool isColorReverseEnabled_ = true;
    inline static bool isColorCorrectionEnabled_ = true;

    friend class AccessibilityParamParse;
};
} // namespace OHOS::Rosen
#endif // ACCESSIBILITY_PARAM_H