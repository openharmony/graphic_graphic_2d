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

#include "filter_param.h"

namespace OHOS::Rosen {

bool FilterParam::IsFilterCacheEnable()
{
    return isFilterCacheEnable_;
}

bool FilterParam::IsEffectMergeEnable()
{
    return isEffectMergeEnable_;
}

bool FilterParam::IsBlurAdaptiveAdjust()
{
    return isBlurAdaptiveAdjust_;
}

bool FilterParam::IsMesablurAllEnable()
{
    return isMesablurAllEnbale_;
}

int FilterParam::GetSimplifiedMesaMode()
{
    return simplifiedMesaMode_;
}

void FilterParam::SetFilterCacheEnable(bool isEnable)
{
    isFilterCacheEnable_ = isEnable;
}

void FilterParam::SetEffectMergeEnable(bool isEnable)
{
    isEffectMergeEnable_ = isEnable;
}

void FilterParam::SetBlurAdaptiveAdjust(bool isEnable)
{
    isBlurAdaptiveAdjust_ = isEnable;
}

void FilterParam::SetMesablurAllEnable(bool isEnable)
{
    isMesablurAllEnbale_ = isEnable;
}

void FilterParam::SetSimplifiedMesaMode(int mode)
{
    simplifiedMesaMode_ = mode;
}
} // namespace OHOS::Rosen