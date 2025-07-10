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

#include "modifier_ng/background/rs_background_ng_shader_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSBackgroundNGShaderRenderModifier::LegacyPropertyApplierMap
    RSBackgroundNGShaderRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::BACKGROUND_NG_SHADER,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSNGRenderShaderBase>,
                &RSProperties::SetBackgroundNGShader> },
    };

void RSBackgroundNGShaderRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetBackgroundNGShader(nullptr);
}
} // namespace OHOS::Rosen::ModifierNG
