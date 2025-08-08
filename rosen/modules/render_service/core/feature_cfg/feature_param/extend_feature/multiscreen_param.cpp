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

#include "multiscreen_param.h"

namespace OHOS::Rosen {
 
bool MultiScreenParam::IsExternalScreenSecure()
{
    return isExternalScreenSecure_;
}

void MultiScreenParam::SetExternalScreenSecure(bool isSecure)
{
    isExternalScreenSecure_ = isSecure;
}

bool MultiScreenParam::IsSlrScaleEnabled()
{
    return isSlrScaleEnabled_;
}

void MultiScreenParam::SetSlrScaleEnabled(bool isEnabled)
{
    isSlrScaleEnabled_ = isEnabled;
}

bool MultiScreenParam::IsRsReportHwcDead()
{
    return isRsReportHwcDead_;
}

void MultiScreenParam::SetRsReportHwcDead(bool isEnabled)
{
    isRsReportHwcDead_ = isEnabled;
}

bool MultiScreenParam::IsRsSetScreenPowerStatus()
{
    return isRsSetScreenPowerStatus_;
}

void MultiScreenParam::SetRsSetScreenPowerStatus(bool isEnabled)
{
    isRsSetScreenPowerStatus_ = isEnabled;
}

bool MultiScreenParam::IsMirrorDisplayCloseP3()
{
    return isMirrorDisplayCloseP3_;
}

void MultiScreenParam::SetMirrorDisplayCloseP3(bool isEnabled)
{
    isMirrorDisplayCloseP3_ = isEnabled;
}

Drawing::MipmapMode MultiScreenParam::GetMipmapMode()
{
    return mipMapModeValue_;
}
 
void MultiScreenParam::SetMipmapMode(Drawing::MipmapMode modeValue)
{
    mipMapModeValue_ = modeValue;
}
} // namespace OHOS::Rosen