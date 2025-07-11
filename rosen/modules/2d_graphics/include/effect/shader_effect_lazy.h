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

#ifndef SHADER_EFFECT_LAZY_H
#define SHADER_EFFECT_LAZY_H

#include "draw/blend_mode.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_obj.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

// 仅支持 Drawing NDK 接口
class DRAWING_API ShaderEffectLazy : public ShaderEffect {
public:
    static std::shared_ptr<ShaderEffectLazy> CreateBlendShader(const std::shared_ptr<ShaderEffect>& dst,
        const std::shared_ptr<ShaderEffect>& src, BlendMode mode);
    static std::shared_ptr<ShaderEffectLazy> CreateFromShaderEffectObj(std::shared_ptr<ShaderEffectObj> object);
    static std::shared_ptr<ShaderEffectLazy> CreateForUnmarshalling();

    ~ShaderEffectLazy() override = default;

    bool IsLazy() const override { return true; }

    std::shared_ptr<ShaderEffectObj> GetShaderEffectObj() const { return shaderEffectObj_; }
    std::shared_ptr<ShaderEffect> Materialize();

    // WARNING: Do not call Serialize()/Deserialize() on Lazy objects!
    // Use ExtendObject Marshalling()/Unmarshalling() instead.
    // These methods will return nullptr/false and log errors.
    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    static std::shared_ptr<ShaderEffectLazy> Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0);
#endif
private:
    ShaderEffectLazy() noexcept : ShaderEffect(ShaderEffectType::LAZY_SHADER) {}
    ShaderEffectLazy(std::shared_ptr<ShaderEffectObj> shaderEffectObj) noexcept;

    std::shared_ptr<ShaderEffectObj> shaderEffectObj_ = nullptr;
    std::shared_ptr<ShaderEffect> shaderEffectCache_ = nullptr; // not lazy
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
