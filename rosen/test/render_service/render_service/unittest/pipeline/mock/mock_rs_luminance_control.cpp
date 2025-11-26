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

#include "mock_rs_luminance_control.h"

namespace OHOS {
namespace Rosen {
namespace Mock {
bool RSLuminanceControlInterfaceMock::SetHdrStatus([[maybe_unused]] ScreenId screenId, HdrStatus curDisplayHdrStatus)
{
    hdrStatus_ = curDisplayHdrStatus;
    return true;
}

bool RSLuminanceControlInterfaceMock::IsHdrOn([[maybe_unused]] ScreenId screenId) const
{
    return isHdrOn_;
}

bool RSLuminanceControlInterfaceMock::IsHardwareHdrDisabled([[maybe_unused]] bool checkBrightnessRatio,
    [[maybe_unused]] ScreenId screenId)
{
    return isHardwareHdrDisabled_;
}
} // Mock
} // Rosen
} // OHOS