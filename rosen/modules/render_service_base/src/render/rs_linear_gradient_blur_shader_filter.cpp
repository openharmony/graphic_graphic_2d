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
#include "render/rs_linear_gradient_blur_shader_filter.h"

#include "src/core/SkOpts.h"


namespace OHOS {
namespace Rosen {
RSLinearGradientBlurShaderFilter::RSLinearGradientBlurShaderFilter(
    const std::shared_ptr<RSLinearGradientBlurPara>& para, const float geoWidth, const float geoHeight)
    : linearGradientBlurPara_(para)
{
    type_ = ShaderFilterType::LINEAR_GRADIENT_BLUR;
    geoWidth_ = geoWidth;
    geoHeight_ = geoHeight;
    hash_ = SkOpts::hash(&linearGradientBlurPara_, sizeof(linearGradientBlurPara_), 0);
    hash_ = SkOpts::hash(&geoWidth_, sizeof(geoWidth_), hash_);
    hash_ = SkOpts::hash(&geoHeight_, sizeof(geoHeight_), hash_);
}

RSLinearGradientBlurShaderFilter::~RSLinearGradientBlurShaderFilter() = default;

void RSLinearGradientBlurShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto gelinearGradientBlurFilter = std::make_shared<Drawing::GEVisualEffect>(
        "LINEAR_GRADIENT_BLUR", Drawing::DrawingPaintType::BRUSH);
    gelinearGradientBlurFilter->SetParam("BLURRADIUS", linearGradientBlurPara_->blurRadius_);
    gelinearGradientBlurFilter->SetParam("GEOWIDTH", geoWidth_);
    gelinearGradientBlurFilter->SetParam("GEOHEIGHT", geoHeight_);
    gelinearGradientBlurFilter->SetParam("TRANX", tranX_);
    gelinearGradientBlurFilter->SetParam("TRANY", tranY_);
    gelinearGradientBlurFilter->SetParam("CANVASMAT", mat_);
    gelinearGradientBlurFilter->SetParam("FRACTIONSTOPS", linearGradientBlurPara_->fractionStops_);
    gelinearGradientBlurFilter->SetParam("DIRECTION", (int)linearGradientBlurPara_->direction_);
 
    visualEffectContainer->AddToChainedFilter(gelinearGradientBlurFilter);
}
} // namespace Rosen
} // namespace OHOS
