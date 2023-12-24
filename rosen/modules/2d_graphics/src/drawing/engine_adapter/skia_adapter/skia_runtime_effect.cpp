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

#include "skia_runtime_effect.h"

#include "effect/runtime_effect.h"

#include "skia_adapter/skia_data.h"
#include "skia_adapter/skia_matrix.h"
#include "skia_shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaRuntimeEffect::SkiaRuntimeEffect() noexcept : skRuntimeEffect_(nullptr) {}

void SkiaRuntimeEffect::InitForShader(const std::string& sl, const RuntimeEffectOptions& options)
{
    SkRuntimeEffect::Options skOptions;
    skOptions.useAF = options.useAF;
    skOptions.forceNoInline = options.forceNoInline;
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(sl.c_str()), skOptions);
    skRuntimeEffect_ = effect;
}

void SkiaRuntimeEffect::InitForShader(const std::string& sl)
{
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(sl.c_str()));
    skRuntimeEffect_ = effect;
}

void SkiaRuntimeEffect::InitForBlender(const std::string& sl)
{
    auto [effect, err] = SkRuntimeEffect::MakeForBlender(SkString(sl.c_str()));
    skRuntimeEffect_ = effect;
}

std::shared_ptr<ShaderEffect> SkiaRuntimeEffect::MakeShader(std::shared_ptr<Data> uniforms,
    std::shared_ptr<ShaderEffect> children[], size_t childCount, const Matrix* localMatrix,
    bool isOpaque)
{
    auto data = uniforms ? uniforms->GetImpl<SkiaData>()->GetSkData() : SkData::MakeEmpty();
    sk_sp<SkShader> skChildren[childCount];
    for (size_t i = 0; i < childCount; ++i) {
        auto skShaderImpl = children[i]->GetImpl<SkiaShaderEffect>();
        if (skShaderImpl) {
            skChildren[i] = sk_sp<SkShader>(skShaderImpl->GetShader());
        }
    }
    sk_sp<SkShader> skShader = skRuntimeEffect_->makeShader(data, skChildren,
        childCount, localMatrix ? &localMatrix->GetImpl<SkiaMatrix>()->ExportSkiaMatrix() : nullptr,
        isOpaque);
    std::shared_ptr<ShaderEffect> shader = std::make_shared<ShaderEffect>();
    shader->GetImpl<SkiaShaderEffect>()->SetSkShader(skShader);
    return shader;
}

sk_sp<SkRuntimeEffect> SkiaRuntimeEffect::GetRuntimeEffect() const
{
    return skRuntimeEffect_;
}

void SkiaRuntimeEffect::SetRuntimeEffect(const sk_sp<SkRuntimeEffect>& skRuntimeEffect)
{
    skRuntimeEffect_ = skRuntimeEffect;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS