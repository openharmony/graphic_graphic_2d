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

#include "dvsync_param.h"

namespace OHOS::Rosen {

bool DvsyncParam::IsDvsyncEnable() const
{
    return isDvsyncEnable_;
}

bool DvsyncParam::IsUiDvsyncEnable() const
{
    return isUiDvsyncEnable_;
}

bool DvsyncParam::IsNativeDvsyncEnable() const
{
    return isNativeDvsyncEnable_;
}

int32_t DvsyncParam::GetRsPreCount() const
{
    return rsPreCnt_;
}

int32_t DvsyncParam::GetAppPreCount() const
{
    return appPreCnt_;
}

int32_t DvsyncParam::GetNativePreCount() const
{
    return nativePreCnt_;
}

void DvsyncParam::SetDvsyncEnable(bool isEnable)
{
    isDvsyncEnable_ = isEnable;
}

void DvsyncParam::SetUiDvsyncEnable(bool isEnable)
{
    isUiDvsyncEnable_ = isEnable;
}

void DvsyncParam::SetNativeDvsyncEnable(bool isEnable)
{
    isNativeDvsyncEnable_ = isEnable;
}

void DvsyncParam::SetRsPreCount(int32_t cnt)
{
    rsPreCnt_ = cnt;
}

void DvsyncParam::SetAppPreCount(int32_t cnt)
{
    appPreCnt_ = cnt;
}

void DvsyncParam::SetNativePreCount(int32_t cnt)
{
    nativePreCnt_ = cnt;
}
} // namespace OHOS::Rosen