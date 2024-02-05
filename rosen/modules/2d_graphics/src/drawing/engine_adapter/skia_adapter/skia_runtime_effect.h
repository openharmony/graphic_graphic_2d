/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_RUNTIME_EFFECT_H
#define SKIA_RUNTIME_EFFECT_H

#include "include/effects/SkRuntimeEffect.h"
#include "src/core/SkRuntimeEffectPriv.h"

#include "effect/blender.h"
#include "effect/shader_effect.h"
#include "impl_interface/runtime_effect_impl.h"
#include "utils/data.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaRuntimeEffect : public RuntimeEffectImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    static void GlslToSksl(std::string& sksl, std::string& glsl);

    SkiaRuntimeEffect() noexcept;
    ~SkiaRuntimeEffect() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void InitForShader(const std::string& sl, const RuntimeEffectOptions& options) override;
    void InitForShader(const std::string& sl) override;
    void InitForES3Shader(const std::string& sl) override;
    void InitForBlender(const std::string& sl) override;
    std::shared_ptr<ShaderEffect> MakeShader(std::shared_ptr<Data> uniforms,
        std::shared_ptr<ShaderEffect> children[], size_t childCount,
        const Matrix* localMatrix, bool isOpaque) override;
    sk_sp<SkRuntimeEffect> GetRuntimeEffect() const;
    /*
     * @brief  Update the member variable to SkRuntimeEffect, adaptation layer calls.
     */
    void SetRuntimeEffect(const sk_sp<SkRuntimeEffect>& skRuntimeEffect);

private:
    sk_sp<SkRuntimeEffect> skRuntimeEffect_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif