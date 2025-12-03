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

#ifndef ROSEN_ENGINE_CORE_RENDER_UI_SHADER_BASE_H
#define ROSEN_ENGINE_CORE_RENDER_UI_SHADER_BASE_H

#include "effect/rs_render_shader_base.h"
#include "ui_effect/property/include/rs_ui_property_tag.h"
#include "ui_effect/property/include/rs_ui_template.h"

namespace OHOS {
namespace Rosen {

class RSNGShaderBase : public RSNGEffectBase<RSNGShaderBase, RSNGRenderShaderBase> {
public:
    virtual ~RSNGShaderBase() = default;

    static std::shared_ptr<RSNGShaderBase> Create(RSNGEffectType type);
    static std::shared_ptr<RSNGShaderBase> Create(std::shared_ptr<VisualEffectPara> effectPara);
};

template<RSNGEffectType Type, typename... PropertyTags>
using RSNGShaderTemplate = RSNGEffectTemplate<RSNGShaderBase, Type, PropertyTags...>;

#define SEPARATOR ,
#define ADD_PROPERTY_TAG(Effect, Prop) Effect##Prop##Tag
#define DECLARE_SHADER(ShaderName, ShaderType, ...) \
    using RSNG##ShaderName = RSNGShaderTemplate<RSNGEffectType::ShaderType, __VA_ARGS__>

#include "effect/rs_render_shader_def.in"

#undef SEPARATOR
#undef ADD_PROPERTY_TAG
#undef DECLARE_SHADER

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_RENDER_UI_SHADER_BASE_H
