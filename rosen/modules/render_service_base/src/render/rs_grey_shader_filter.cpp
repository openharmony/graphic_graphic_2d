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
#include "render/rs_grey_shader_filter.h"

#include "pipeline/rs_paint_filter_canvas.h"


namespace OHOS {
namespace Rosen {
RSGreyShaderFilter::RSGreyShaderFilter(float greyCoef1, float greyCoef2)
    : greyCoef1_(greyCoef1), greyCoef2_(greyCoef2)
{
    type_ = ShaderFilterType::GREY;
    hash_ = SkOpts::hash(&greyCoef1_, sizeof(greyCoef1_), hash_);
    hash_ = SkOpts::hash(&greyCoef2_, sizeof(greyCoef2_), hash_);
}

RSGreyShaderFilter::~RSGreyShaderFilter() {};

void RSGreyShaderFilter::GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto greyFilter = std::make_shared<Drawing::GEVisualEffect>("GREY", Drawing::DrawingPaintType::BRUSH);
    greyFilter->SetParam("GREY_COEF_1", greyCoef1_);
    greyFilter->SetParam("GREY_COEF_2", greyCoef2_);
    visualEffectContainer->AddToChainedFilter(greyFilter);
}
} // namespace Rosen
} // namespace OHOS