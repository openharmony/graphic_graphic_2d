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

#include "modifier_ng/background/rs_background_shader_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSBackgroundShaderRenderModifier::LegacyPropertyApplierMap
    RSBackgroundShaderRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::BACKGROUND_SHADER,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSShader>, &RSProperties::SetBackgroundShader> },
        { RSPropertyType::BACKGROUND_SHADER_PROGRESS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBackgroundShaderProgress,
                &RSProperties::GetBackgroundShaderProgress> },
        { RSPropertyType::COMPLEX_SHADER_PARAM,
            RSRenderModifier::PropertyApplyHelperAdd<std::vector<float>, &RSProperties::SetComplexShaderParam,
                &RSProperties::GetComplexShaderParam> },
    };

void RSBackgroundShaderRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetBackgroundShader(nullptr);
    properties.SetBackgroundShaderProgress(0.f);
    properties.SetComplexShaderParam({});
}
} // namespace OHOS::Rosen::ModifierNG
