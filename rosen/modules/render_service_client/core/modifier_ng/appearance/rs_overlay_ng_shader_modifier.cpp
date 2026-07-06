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

#include "modifier_ng/appearance/rs_overlay_ng_shader_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSOverlayNGShaderModifier::SetOverlayNGShader(const std::shared_ptr<RSNGShaderBase>& overlayShader)
{
    Setter<RSProperty, std::shared_ptr<RSNGShaderBase>>(RSPropertyType::OVERLAY_NG_SHADER, overlayShader);
}

std::shared_ptr<RSNGShaderBase> RSOverlayNGShaderModifier::GetOverlayNGShader() const
{
    return Getter<std::shared_ptr<RSNGShaderBase>>(RSPropertyType::OVERLAY_NG_SHADER, nullptr);
}
} // namespace OHOS::Rosen::ModifierNG