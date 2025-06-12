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

#include "modifier_ng/background/rs_background_shader_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSBackgroundShaderModifier::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    Setter<RSProperty, std::shared_ptr<RSShader>>(RSPropertyType::BACKGROUND_SHADER, shader);
}

std::shared_ptr<RSShader> RSBackgroundShaderModifier::GetBackgroundShader() const
{
    return Getter<std::shared_ptr<RSShader>>(RSPropertyType::BACKGROUND_SHADER, nullptr);
}

void RSBackgroundShaderModifier::SetBackgroundShaderProgress(float progress)
{
    Setter(RSPropertyType::BACKGROUND_SHADER_PROGRESS, progress);
}

float RSBackgroundShaderModifier::GetBackgroundShaderProgress() const
{
    return Getter(RSPropertyType::BACKGROUND_SHADER_PROGRESS, 0.f);
}

void RSBackgroundShaderModifier::SetComplexShaderParam(const std::vector<float>& param)
{
    Setter<RSProperty, std::vector<float>>(RSPropertyType::COMPLEX_SHADER_PARAM, param);
}

std::vector<float> RSBackgroundShaderModifier::GetComplexShaderParam() const
{
    return Getter<std::vector<float>>(RSPropertyType::COMPLEX_SHADER_PARAM, {});
}
} // namespace OHOS::Rosen::ModifierNG
