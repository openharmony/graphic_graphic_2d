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

#include "color_temp/rs_color_temp.h"

namespace OHOS {
namespace Rosen {
RSColorTemp& RSColorTemp::Get()
{
    static RSColorTemp instance;
    return instance;
}

RSColorTemp::~RSColorTemp() {}

void RSColorTemp::Init() {}

void RSColorTemp::RegisterRefresh(std::function<void()> refresh) {}

void RSColorTemp::UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus status) {}

bool RSColorTemp::IsDimmingOn(ScreenId screenId) { return false; }

void RSColorTemp::DimmingIncrease(ScreenId screenId) {}

std::vector<float> RSColorTemp::GetNewLinearCct(ScreenId screenId) { return {}; }

} // namespace Rosen
} // namespace OHOS