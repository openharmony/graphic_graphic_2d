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
#include "render/rs_render_grey_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "pipeline/rs_paint_filter_canvas.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
RSGreyShaderFilter::RSGreyShaderFilter(float greyCoefLow, float greyCoefHigh)
    : RSRenderFilterParaBase(RSUIFilterType::GREY), greyCoefLow_(greyCoefLow), greyCoefHigh_(greyCoefHigh)
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&greyCoefLow_, sizeof(greyCoefLow_), hash_);
    hash_ = hashFunc(&greyCoefHigh_, sizeof(greyCoefHigh_), hash_);
}

float RSGreyShaderFilter::GetGreyCoefLow() const
{
    return greyCoefLow_;
}

float RSGreyShaderFilter::GetGreyCoefHigh() const
{
    return greyCoefHigh_;
}

void RSGreyShaderFilter::GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto greyFilter = std::make_shared<Drawing::GEVisualEffect>("GREY", Drawing::DrawingPaintType::BRUSH);
    greyFilter->SetParam("GREY_COEF_1", greyCoefLow_);
    greyFilter->SetParam("GREY_COEF_2", greyCoefHigh_);
    visualEffectContainer->AddToChainedFilter(greyFilter);
}
} // namespace Rosen
} // namespace OHOS