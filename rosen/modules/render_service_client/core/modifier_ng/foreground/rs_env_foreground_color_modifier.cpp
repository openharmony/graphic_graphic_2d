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

#include "modifier_ng/foreground/rs_env_foreground_color_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSEnvForegroundColorModifier::SetEnvForegroundColor(Color colorValue)
{
    Setter(RSPropertyType::ENV_FOREGROUND_COLOR, colorValue);
}

Color RSEnvForegroundColorModifier::GetEnvForegroundColor() const
{
    return Getter(RSPropertyType::ENV_FOREGROUND_COLOR, Color());
}

void RSEnvForegroundColorModifier::SetEnvForegroundColorStrategy(ForegroundColorStrategyType strategyType)
{
    Setter<RSProperty>(RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, static_cast<int>(strategyType));
}

ForegroundColorStrategyType RSEnvForegroundColorModifier::GetEnvForegroundColorStrategy() const
{
    return static_cast<ForegroundColorStrategyType>(Getter(RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, 0));
}
} // namespace OHOS::Rosen::ModifierNG
