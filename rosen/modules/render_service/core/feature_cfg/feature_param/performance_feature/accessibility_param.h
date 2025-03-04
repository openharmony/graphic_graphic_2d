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

    bool IsHighContrastEnabled() const;
    bool IsCurtainScreenEnabled() const;
    bool IsColorReverseEnabled() const;
    bool IsColorCorrectionEnabled() const;

protected:
    void SetHighContrastEnabled(bool isEnable);
    void SetCurtainScreenEnabled(bool isEnable);
    void SetColorReverseEnabled(bool isEnable);
    void SetColorCorrectionEnabled(bool isEnable);

private:
    bool isHighContrastEnabled_ = true;
    bool isCurtainScreenEnabled_ = true;
    bool isColorReverseEnabled_ = true;
    bool isColorCorrectionEnabled_ = true;

    friend class AccessibilityParamParse;
};
} // namespace OHOS::Rosen
#endif // ACCESSIBILITY_PARAM_H