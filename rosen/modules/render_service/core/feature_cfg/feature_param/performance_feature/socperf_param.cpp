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

#include "socperf_param.h"

namespace OHOS::Rosen {

SOCPerfParam& SOCPerfParam::GetInstance()
{
    static SOCPerfParam instance;
    return instance;
}

bool SOCPerfParam::IsMultiLayersSOCPerfEnable() const
{
    return isMultiLayersSOCPerfEnable_;
}

bool SOCPerfParam::IsUnlockSOCPerfEnable() const
{
    return isUnlockSOCPerfEnable_;
}

bool SOCPerfParam::IsBlurSOCPerfEnable() const
{
    return isBlurSOCPerfEnable_;
}

void SOCPerfParam::SetMultiLayersSOCPerfEnable(bool isMultiLayersSOCPerfEnable)
{
    isMultiLayersSOCPerfEnable_ = isMultiLayersSOCPerfEnable;
}

void SOCPerfParam::SetUnlockSOCPerfEnable(bool isUnlockSOCPerfEnable)
{
    isUnlockSOCPerfEnable_ = isUnlockSOCPerfEnable;
}

void SOCPerfParam::SetBlurSOCPerfEnable(bool isBlurSOCPerfEnable)
{
    isBlurSOCPerfEnable_ = isBlurSOCPerfEnable;
}
} // namespace OHOS::Rosen