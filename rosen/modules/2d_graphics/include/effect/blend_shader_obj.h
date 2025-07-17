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

#ifndef BLEND_SHADER_OBJ_H
#define BLEND_SHADER_OBJ_H

#include "draw/blend_mode.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_obj.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class BlendShaderObj final : public ShaderEffectObj {
public:
    static std::shared_ptr<BlendShaderObj> CreateForUnmarshalling();
    static std::shared_ptr<BlendShaderObj> Create(const std::shared_ptr<ShaderEffect>& dst,
                                                  const std::shared_ptr<ShaderEffect>& src,
                                                  BlendMode mode);
    ~BlendShaderObj() override = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override;
#endif
    std::shared_ptr<void> GenerateBaseObject() override;

private:
    BlendShaderObj();
    BlendShaderObj(const std::shared_ptr<ShaderEffect>& dst,
                   const std::shared_ptr<ShaderEffect>& src,
                   BlendMode mode);
    std::shared_ptr<ShaderEffect> dstShader_ = nullptr;
    std::shared_ptr<ShaderEffect> srcShader_ = nullptr;
    BlendMode blendMode_ = BlendMode::SRC_OVER;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // BLEND_SHADER_OBJ_H