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

#include "uifirst_param.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

bool UIFirstParam::IsUIFirstEnable() const
{
    return isUIFirstEnable_;
}

void UIFirstParam::SetUIFirstEnable(bool isEnable)
{
    isUIFirstEnable_ = isEnable;
}

bool UIFirstParam::IsCardUIFirstEnable() const
{
    return isCardUIFirstEnable_;
}

void UIFirstParam::SetCardUIFirstEnable(bool isCardUIFirstEnable)
{
    isCardUIFirstEnable_ = isCardUIFirstEnable;
}

bool UIFirstParam::IsCacheOptimizeRotateEnable() const
{
    return isCacheOptimizeRotateEnable_;
}

void UIFirstParam::SetCacheOptimizeRotateEnable(bool isCacheOptimizeRotateEnable)
{
    isCacheOptimizeRotateEnable_ = isCacheOptimizeRotateEnable;
}

bool UIFirstParam::IsFreeMultiWindowEnable() const
{
    return isFreeMultiWindowEnable_;
}

void UIFirstParam::SetFreeMultiWindowEnable(bool isFreeMultiWindowEnable)
{
    isFreeMultiWindowEnable_ = isFreeMultiWindowEnable;
}

int UIFirstParam::GetUIFirstEnableWindowThreshold() const
{
    return uiFirstEnableWindowThreshold_;
}

void UIFirstParam::SetUIFirstEnableWindowThreshold(int uiFirstEnableWindowThreshold)
{
    uiFirstEnableWindowThreshold_ = uiFirstEnableWindowThreshold;
}
} // namespace OHOS::Rosen