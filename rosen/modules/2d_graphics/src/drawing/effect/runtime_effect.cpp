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

#include "effect/runtime_effect.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RuntimeEffect::RuntimeEffect(const std::string& sl, const RuntimeEffectOptions& options) noexcept : RuntimeEffect()
{
    impl_->InitForShader(sl, options);
}

RuntimeEffect::RuntimeEffect(const std::string& sl) noexcept : RuntimeEffect()
{
    impl_->InitForShader(sl);
}

RuntimeEffect::RuntimeEffect(const std::string& sl, const RuntimeEffectOptions& options,
    bool isES3) noexcept : RuntimeEffect()
{
    impl_->InitForES3Shader(sl);
}

RuntimeEffect::RuntimeEffect(const std::string& sl, bool isBlender) noexcept : RuntimeEffect()
{
    impl_->InitForBlender(sl);
}

RuntimeEffect::RuntimeEffect() noexcept : impl_(ImplFactory::CreateRuntimeEffectImpl())
{}

std::shared_ptr<RuntimeEffect> RuntimeEffect::CreateForShader(const std::string& sl,
    const RuntimeEffectOptions& options)
{
    return std::make_shared<RuntimeEffect>(sl, options);
}

std::shared_ptr<RuntimeEffect> RuntimeEffect::CreateForShader(const std::string& sl)
{
    return std::make_shared<RuntimeEffect>(sl);
}

std::shared_ptr<RuntimeEffect> RuntimeEffect::CreateForES3Shader(const std::string& sl)
{
    return std::make_shared<RuntimeEffect>(sl, RuntimeEffectOptions(), true);
}

std::shared_ptr<RuntimeEffect> RuntimeEffect::CreateForBlender(const std::string& sl)
{
    return std::make_shared<RuntimeEffect>(sl, true);
}

std::shared_ptr<ShaderEffect> RuntimeEffect::MakeShader(std::shared_ptr<Data> uniforms,
    std::shared_ptr<ShaderEffect> children[], size_t childCount, const Matrix* localMatrix,
    bool isOpaque)
{
    return impl_->MakeShader(uniforms, children, childCount, localMatrix, isOpaque);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS