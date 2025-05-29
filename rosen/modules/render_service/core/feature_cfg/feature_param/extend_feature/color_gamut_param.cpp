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

#include "color_gamut_param.h"
#include "common/rs_common_hook.h"

namespace OHOS::Rosen {

bool ColorGamutParam::DisableP3OnWiredExtendedScreen()
{
    return disableP3OnWiredExtendedScreen_;
}

void ColorGamutParam::SetDisableP3OnWiredExtendedScreen(bool isEnable)
{
    disableP3OnWiredExtendedScreen_ = isEnable;
}

bool ColorGamutParam::IsAdaptiveColorGamutEnabled()
{
    return isAdaptiveColorGamutEnabled_;
}

void ColorGamutParam::SetAdaptiveColorGamutEnable(bool isEnable)
{
    RsCommonHook::Instance().SetAdaptiveColorGamutEnable(isEnable);
    isAdaptiveColorGamutEnabled_ = isEnable;
}

bool ColorGamutParam::SkipOccludedNodeDuringColorGamutCollection()
{
    return skipOccludedNodeDuringColorGamutCollection_;
}

void ColorGamutParam::SetSkipOccludedNodeDuringColorGamutCollection(bool isEnable)
{
    skipOccludedNodeDuringColorGamutCollection_ = isEnable;
}
} // namespace OHOS::Rosen