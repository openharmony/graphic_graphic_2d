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

#include "include/core/SkM44.h"

#include "skia_image.h"
#include "skia_image_info.h"
#include "skia_matrix.h"
#include "skia_matrix44.h"
#include "skia_shader_effect.h"
#include "skia_runtime_effect.h"
#include "skia_runtime_shader_builder.h"

#include "effect/runtime_effect.h"
#include "image/image.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaRuntimeShaderBuilder::SkiaRuntimeShaderBuilder(
    std::shared_ptr<RuntimeEffect> effect) noexcept : skRuntimeShaderBuilder_(std::make_shared<SkRuntimeShaderBuilder>(
    effect->GetImpl<SkiaRuntimeEffect>()->GetRuntimeEffect())) {}

std::shared_ptr<ShaderEffect> SkiaRuntimeShaderBuilder::MakeShader(const Matrix* localMatrix, bool isOpaque)
{
    if (!skRuntimeShaderBuilder_) {
        return nullptr;
    }
    sk_sp<SkShader> skShader = skRuntimeShaderBuilder_->makeShader(
        localMatrix ? &localMatrix->GetImpl<SkiaMatrix>()->ExportSkiaMatrix() : nullptr, isOpaque);
    auto shader = std::make_shared<ShaderEffect>();
    shader->GetImpl<SkiaShaderEffect>()->SetSkShader(skShader);

    return shader;
}

std::shared_ptr<Image> SkiaRuntimeShaderBuilder::MakeImage(GPUContext* grContext, const Matrix* localMatrix,
    ImageInfo resultInfo, bool mipmapped)
{
    if (!skRuntimeShaderBuilder_ || !grContext) {
        return nullptr;
    }
    sk_sp<SkImage> skImage = skRuntimeShaderBuilder_->makeImage(
        grContext->GetImpl<SkiaGPUContext>()->GetGrContext().get(),
        localMatrix ? &localMatrix->GetImpl<SkiaMatrix>()->ExportSkiaMatrix() : nullptr,
        SkiaImageInfo::ConvertToSkImageInfo(resultInfo), mipmapped);
    auto image = std::make_shared<Image>();
    image->GetImpl<SkiaImage>()->SetSkImage(skImage);

    return image;
}

void SkiaRuntimeShaderBuilder::SetChild(const std::string& name, std::shared_ptr<ShaderEffect> shader)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }
    skRuntimeShaderBuilder_->child(name.c_str()) = shader->GetImpl<SkiaShaderEffect>()->GetShader();
}

void SkiaRuntimeShaderBuilder::SetUniform(const std::string& name, float val)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }
    skRuntimeShaderBuilder_->uniform(name.c_str()) = val;
}

void SkiaRuntimeShaderBuilder::SetUniform(const std::string& name, float x, float y)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }
    skRuntimeShaderBuilder_->uniform(name.c_str()) = SkV2{x, y};
}

void SkiaRuntimeShaderBuilder::SetUniform(const std::string& name, float x, float y, float z)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }
    skRuntimeShaderBuilder_->uniform(name.c_str()) = SkV3{x, y, z};
}

void SkiaRuntimeShaderBuilder::SetUniform(const std::string& name, const float values[], size_t size)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }
    skRuntimeShaderBuilder_->uniform(name.c_str()).set(values, size);
}

void SkiaRuntimeShaderBuilder::SetUniform(const std::string& name, const Matrix& uniformMatrix33)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }

    skRuntimeShaderBuilder_->uniform(name.c_str()) = uniformMatrix33.GetImpl<SkiaMatrix>()->ExportSkiaMatrix();
}

void SkiaRuntimeShaderBuilder::SetUniform(const std::string& name, const Matrix44& uniformMatrix44)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }

    skRuntimeShaderBuilder_->uniform(name.c_str()) = uniformMatrix44.GetImpl<SkiaMatrix44>()->GetSkMatrix44();
}

void SkiaRuntimeShaderBuilder::SetUniformVec4(const std::string& name, float x, float y, float z, float w)
{
    if (!skRuntimeShaderBuilder_) {
        return;
    }
    SkV4 uniformSk4 { x, y, z, w };
    skRuntimeShaderBuilder_->uniform(name.c_str()) = uniformSk4;
}

void SkiaRuntimeShaderBuilder::SetUniform(const std::string& name, float x, float y, float width, float height)
{
    if (!skRuntimeShaderBuilder_ || width == 0 || height == 0) {
        return;
    }
    SkV2 offsets[5] = {
        SkV2{0.0f, 0.0f},
        SkV2{x / width, y / height},
        SkV2{-x / width, y / height},
        SkV2{x / width, -y / height},
        SkV2{-x / width, -y / height},
    };

    skRuntimeShaderBuilder_->uniform(name.c_str()) = offsets;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS