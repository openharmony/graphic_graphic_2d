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
#include "effect/runtime_shader_builder.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RuntimeShaderBuilder::RuntimeShaderBuilder(std::shared_ptr<RuntimeEffect> runtimeEffect) noexcept
    : impl_(ImplFactory::CreateRuntimeShaderBuilderImpl(runtimeEffect))
{}

std::shared_ptr<ShaderEffect> RuntimeShaderBuilder::MakeShader(const Matrix* localMatrix, bool isOpaque)
{
    return impl_->MakeShader(localMatrix, isOpaque);
}

std::shared_ptr<Image> RuntimeShaderBuilder::MakeImage(GPUContext* gpuContext,
    const Matrix* localMatrix, ImageInfo resultInfo, bool mipmapped)
{
    return impl_->MakeImage(gpuContext, localMatrix, resultInfo, mipmapped);
}

void RuntimeShaderBuilder::SetChild(const std::string& name, std::shared_ptr<ShaderEffect> shader)
{
    impl_->SetChild(name, shader);
}

void RuntimeShaderBuilder::SetUniform(const std::string& name, float val)
{
    impl_->SetUniform(name, val);
}

void RuntimeShaderBuilder::SetUniform(const std::string& name, float x, float y)
{
    impl_->SetUniform(name, x, y);
}

void RuntimeShaderBuilder::SetUniform(const std::string& name, float x, float y, float z)
{
    impl_->SetUniform(name, x, y, z);
}

void RuntimeShaderBuilder::SetUniform(const std::string& name, float x, float y, float width, float height)
{
    impl_->SetUniform(name, x, y, width, height);
}

void RuntimeShaderBuilder::SetUniform(const std::string& name, const float values[], size_t size)
{
    impl_->SetUniform(name, values, size);
}

void RuntimeShaderBuilder::SetUniform(const std::string& name, const Matrix& uniformMatrix33)
{
    impl_->SetUniform(name, uniformMatrix33);
}

void RuntimeShaderBuilder::SetUniform(const std::string& name, const Matrix44& uniformMatrix44)
{
    impl_->SetUniform(name, uniformMatrix44);
}

void RuntimeShaderBuilder::SetUniformVec4(const std::string& name, float x, float y, float z, float w)
{
    impl_->SetUniformVec4(name, x, y, z, w);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS