/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ge_grey_shader_filter.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "ge_log.h"

namespace OHOS {
namespace Rosen {

GEGreyShaderFilter::GEGreyShaderFilter(const Drawing::GEGreyShaderFilterParams& params)
    : greyCoef1_(params.greyCoef1), greyCoef2_(params.greyCoef2)
{
}

std::shared_ptr<Drawing::Image> GEGreyShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (!image) {
        LOGE("GEGreyShaderFilter::input image is null");
        return image;
    }

    auto shader = GEShaderStore::GetInstance()->GetShader(SHADER_GREY);
    if (shader == nullptr) {
        return image;
    }
    Drawing::RuntimeShaderBuilder builder(shader->GetShader());
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);

    builder.SetChild("imageShader", imageShader);
    builder.SetUniform("coefficient1", greyCoef1_);
    builder.SetUniform("coefficient2", greyCoef2_);
    auto greyImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (greyImage == nullptr) {
        LOGE("DrawGreyAdjustment failed");
        return image;
    }
    return greyImage;
};


} // namespace Rosen
} // namespace OHOS
