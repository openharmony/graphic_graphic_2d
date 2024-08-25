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
#include "render/rs_mesa_blur_shader_filter.h"

#include "effect/color_matrix.h"
#include "effect/runtime_shader_builder.h"
#include "include/gpu/GrDirectContext.h"
#include "platform/common/rs_system_properties.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
RSMESABlurShaderFilter::RSMESABlurShaderFilter(int radius)
    : radius_(radius)
{
    type_ = ShaderFilterType::MESA;
    CalculateHash();
}

RSMESABlurShaderFilter::RSMESABlurShaderFilter(int radius, float greyCoefLow, float greyCoefHigh)
    : radius_(radius), greyCoefLow_(greyCoefLow), greyCoefHigh_(greyCoefHigh)
{
    type_ = ShaderFilterType::MESA;
    CalculateHash();
}

RSMESABlurShaderFilter::~RSMESABlurShaderFilter() = default;

int RSMESABlurShaderFilter::GetRadius() const
{
    return radius_;
}

void RSMESABlurShaderFilter::SetRadius(int radius)
{
    radius_ = radius;
    CalculateHash();
}

void RSMESABlurShaderFilter::SetPixelStretchParams(std::shared_ptr<RSPixelStretchParams>& param)
{
    pixelStretchParam_ = std::move(param);
    CalculateHash();
}

void RSMESABlurShaderFilter::CalculateHash()
{
    hash_ = SkOpts::hash(&radius_, sizeof(radius_), 0);
    hash_ = SkOpts::hash(&greyCoefLow_, sizeof(greyCoefLow_), hash_);
    hash_ = SkOpts::hash(&greyCoefHigh_, sizeof(greyCoefHigh_), hash_);
    if (pixelStretchParam_) {
        hash_ = SkOpts::hash(&pixelStretchParam_->offsetX_, sizeof(pixelStretchParam_->offsetX_), hash_);
        hash_ = SkOpts::hash(&pixelStretchParam_->offsetY_, sizeof(pixelStretchParam_->offsetY_), hash_);
        hash_ = SkOpts::hash(&pixelStretchParam_->offsetZ_, sizeof(pixelStretchParam_->offsetZ_), hash_);
        hash_ = SkOpts::hash(&pixelStretchParam_->offsetW_, sizeof(pixelStretchParam_->offsetW_), hash_);
        hash_ = SkOpts::hash(&pixelStretchParam_->tileMode_, sizeof(pixelStretchParam_->tileMode_), hash_);
        hash_ = SkOpts::hash(&pixelStretchParam_->width_, sizeof(pixelStretchParam_->width_), hash_);
        hash_ = SkOpts::hash(&pixelStretchParam_->height_, sizeof(pixelStretchParam_->height_), hash_);
    }
}

std::string RSMESABlurShaderFilter::GetDetailedDescription() const
{
    std::string filterString = ", radius: " + std::to_string(radius_) + " sigma";
    filterString = filterString + ", greyCoef1: " + std::to_string(greyCoefLow_);
    filterString = filterString + ", greyCoef2: " + std::to_string(greyCoefHigh_);
    if (pixelStretchParam_) {
        filterString = filterString + ", pixel stretch offsetX: " + std::to_string(pixelStretchParam_->offsetX_);
        filterString = filterString + ", offsetY: " + std::to_string(pixelStretchParam_->offsetY_);
        filterString = filterString + ", offsetZ: " + std::to_string(pixelStretchParam_->offsetZ_);
        filterString = filterString + ", offsetW: " + std::to_string(pixelStretchParam_->offsetW_);
        filterString = filterString + ", tileMode: " + std::to_string(pixelStretchParam_->tileMode_);
        filterString = filterString + ", width: " + std::to_string(pixelStretchParam_->width_);
        filterString = filterString + ", height: " + std::to_string(pixelStretchParam_->height_);
    }
    return filterString;
}

void RSMESABlurShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto mesaFilter = std::make_shared<Drawing::GEVisualEffect>("MESA_BLUR", Drawing::DrawingPaintType::BRUSH);
    mesaFilter->SetParam("MESA_BLUR_RADIUS", (int)radius_); // blur radius
    mesaFilter->SetParam("MESA_BLUR_GREY_COEF_1", greyCoefLow_);
    mesaFilter->SetParam("MESA_BLUR_GREY_COEF_2", greyCoefHigh_);
    if (pixelStretchParam_) {
        mesaFilter->SetParam("OFFSET_X", pixelStretchParam_->offsetX_);
        mesaFilter->SetParam("OFFSET_Y", pixelStretchParam_->offsetY_);
        mesaFilter->SetParam("OFFSET_Z", pixelStretchParam_->offsetZ_);
        mesaFilter->SetParam("OFFSET_W", pixelStretchParam_->offsetW_);
        mesaFilter->SetParam("TILE_MODE", pixelStretchParam_->tileMode_);
        mesaFilter->SetParam("WIDTH", pixelStretchParam_->width_);
        mesaFilter->SetParam("HEIGHT", pixelStretchParam_->height_);
    } else {
        mesaFilter->SetParam("OFFSET_X", 0.f);
        mesaFilter->SetParam("OFFSET_Y", 0.f);
        mesaFilter->SetParam("OFFSET_Z", 0.f);
        mesaFilter->SetParam("OFFSET_W", 0.f);
        mesaFilter->SetParam("TILE_MODE", 0);
        mesaFilter->SetParam("WIDTH", 0.f);
        mesaFilter->SetParam("HEIGHT", 0.f);
    }
    visualEffectContainer->AddToChainedFilter(mesaFilter);
}
} // namespace Rosen
} // namespace OHOS
