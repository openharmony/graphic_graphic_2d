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

#include "display_engine/rs_color_temperature.h"

namespace OHOS {
namespace Rosen {
RSColorTemperature& RSColorTemperature::Get()
{
    static RSColorTemperature instance;
    return instance;
}

RSColorTemperature::~RSColorTemperature() {}

void RSColorTemperature::Init() {}

void RSColorTemperature::RegisterRefresh(std::function<void()>&& refresh) {}

void RSColorTemperature::UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus status) {}

bool RSColorTemperature::IsDimmingOn(ScreenId screenId) { return false; }

void RSColorTemperature::DimmingIncrease(ScreenId screenId) {}

std::vector<float> RSColorTemperature::GetNewLinearCct(ScreenId screenId) { return {}; }

} // namespace Rosen
} // namespace OHOS