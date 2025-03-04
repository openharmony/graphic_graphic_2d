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

#include "accessibility_param.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

bool AccessibilityParam::IsHighContrastEnabled() const
{
    return isHighContrastEnabled_;
}

bool AccessibilityParam::IsCurtainScreenEnabled() const
{
    return isCurtainScreenEnabled_;
}

bool AccessibilityParam::IsColorReverseEnabled() const
{
    return isColorReverseEnabled_;
}

bool AccessibilityParam::IsColorCorrectionEnabled() const
{
    return isColorCorrectionEnabled_;
}

void AccessibilityParam::SetHighContrastEnabled(bool isEnable)
{
    isHighContrastEnabled_ = isEnable;
}

void AccessibilityParam::SetCurtainScreenEnabled(bool isEnable)
{
    isCurtainScreenEnabled_ = isEnable;
}

void AccessibilityParam::SetColorReverseEnabled(bool isEnable)
{
    isColorReverseEnabled_ = isEnable;
}

void AccessibilityParam::SetColorCorrectionEnabled(bool isEnable)
{
    isColorCorrectionEnabled_ = isEnable;
}
} // namespace OHOS::Rosen