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

bool DvsyncParam::IsDvsyncEnable()
{
    return isDvsyncEnable;
}

bool DvsyncParam::IsUiDvsyncEnable()
{
    return isUiDvsyncEnable;
}

bool DvsyncParam::IsNativeDvsyncEnable()
{
    return isNativeDvsyncEnable;
}

int32_t DvsyncParam::GetRsPreCount()
{
    return rsPreCnt;
}

int32_t DvsyncParam::GetAppPreCount()
{
    return appPreCnt;
}

int32_t DvsyncParam::GetNativePreCount()
{
    return nativePreCnt;
}

void DvsyncParam::SetDvsyncEnable(bool isEnable)
{
    isDvsyncEnable = isEnable;
}

void DvsyncParam::SetUiDvsyncEnable(bool isEnable)
{
    isUiDvsyncEnable = isEnable;
}

void DvsyncParam::SetNativeDvsyncEnable(bool isEnable)
{
    isNativeDvsyncEnable = isEnable;
}

void DvsyncParam::SetRsPreCount(int32_t cnt)
{
    rsPreCnt = cnt;
}

void DvsyncParam::SetAppPreCount(int32_t cnt)
{
    appPreCnt = cnt;
}

void DvsyncParam::SetNativePreCount(int32_t cnt)
{
    nativePreCnt = cnt;
}
} // namespace OHOS::Rosen