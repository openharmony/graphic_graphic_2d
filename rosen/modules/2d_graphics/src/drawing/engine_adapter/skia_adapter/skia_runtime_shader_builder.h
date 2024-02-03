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

#ifndef SKIA_RUNTIME_SHADER_BUILDER_H
#define SKIA_RUNTIME_SHADER_BUILDER_H

#include "include/effects/SkRuntimeEffect.h"

#include "skia_gpu_context.h"
#include "skia_shader_effect.h"

#include "impl_interface/runtime_shader_builder_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RuntimeEffect;
class Image;
class DRAWING_API SkiaRuntimeShaderBuilder : public RuntimeShaderBuilderImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaRuntimeShaderBuilder() noexcept = default;
    SkiaRuntimeShaderBuilder(std::shared_ptr<RuntimeEffect>) noexcept;
    ~SkiaRuntimeShaderBuilder() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    std::shared_ptr<ShaderEffect> MakeShader(const Matrix* localMatrix, bool isOpaque) override;
    std::shared_ptr<Image> MakeImage(GPUContext* grContext, const Matrix* localMatrix,
        ImageInfo resultInfo, bool mipmapped) override;

    void SetChild(const std::string& name, std::shared_ptr<ShaderEffect> shader) override;
    void SetUniform(const std::string& name, float val) override;
    void SetUniform(const std::string& name, float x, float y) override;
    void SetUniform(const std::string& name, float x, float y, float z) override;
    void SetUniform(const std::string& name, float x, float y, float width, float height) override;
    void SetUniform(const std::string& name, const float values[], size_t size) override;
    void SetUniform(const std::string& name, const Matrix& uniformMatrix33) override;
    void SetUniform(const std::string& name, const Matrix44& uniformMatrix44) override;
    void SetUniformVec4(const std::string& name, float x, float y, float z, float w) override;

private:
    std::shared_ptr<SkRuntimeShaderBuilder> skRuntimeShaderBuilder_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif