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
#include "render/rs_water_ripple_shader_filter.h"
 
#include "platform/common/rs_log.h"
#include "effect/color_matrix.h"
#include "effect/runtime_shader_builder.h"
#include "platform/common/rs_system_properties.h"
#include "src/core/SkOpts.h"
#include "property/rs_properties.h"
 
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#else
#include "src/core/SkOpts.h"
#include "include/gpu/GrDirectContext.h"
#endif

namespace OHOS {
namespace Rosen {
RSWaterRippleShaderFilter::RSWaterRippleShaderFilter(
    const float progress, const uint32_t waveCount, const float rippleCenterX, const float rippleCenterY,
    const uint32_t rippleMode)
{
    type_ = ShaderFilterType::WATER_RIPPLE;
    progress_ = progress;
    waveCount_ = waveCount;
    rippleCenterX_ = rippleCenterX;
    rippleCenterY_ = rippleCenterY;
    rippleMode_ = rippleMode;
#ifndef ENABLE_M133_SKIA
    hash_ = SkOpts::hash(&progress_, sizeof(progress_), hash_);
    hash_ = SkOpts::hash(&waveCount_, sizeof(waveCount_), hash_);
    hash_ = SkOpts::hash(&rippleCenterX_, sizeof(rippleCenterX_), hash_);
    hash_ = SkOpts::hash(&rippleCenterY_, sizeof(rippleCenterY_), hash_);
    hash_ = SkOpts::hash(&rippleMode_, sizeof(rippleMode_), hash_);
#else
    hash_ = SkChecksum::Hash32(&progress_, sizeof(progress_), hash_);
    hash_ = SkChecksum::Hash32(&waveCount_, sizeof(waveCount_), hash_);
    hash_ = SkChecksum::Hash32(&rippleCenterX_, sizeof(rippleCenterX_), hash_);
    hash_ = SkChecksum::Hash32(&rippleCenterY_, sizeof(rippleCenterY_), hash_);
    hash_ = SkChecksum::Hash32(&rippleMode_, sizeof(rippleMode_), hash_);
#endif
}
 
RSWaterRippleShaderFilter::~RSWaterRippleShaderFilter() = default;
 
float RSWaterRippleShaderFilter::GetProgress() const
{
    return progress_;
}
 
uint32_t RSWaterRippleShaderFilter::GetwaveCount() const
{
    return waveCount_;
}
 
float RSWaterRippleShaderFilter::GetRippleCenterX() const
{
    return rippleCenterX_;
}
 
float RSWaterRippleShaderFilter::GetRippleCenterY() const
{
    return rippleCenterY_;
}

uint32_t RSWaterRippleShaderFilter::GetRippleMode() const
{
    return rippleMode_;
}
 
void RSWaterRippleShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto waterRippleFilter = std::make_shared<Drawing::GEVisualEffect>
        ("WATER_RIPPLE", Drawing::DrawingPaintType::BRUSH);
    waterRippleFilter->SetParam(RS_FILTER_WATER_RIPPLE_PROGRESS, progress_);
    waterRippleFilter->SetParam(RS_FILTER_WATER_RIPPLE_WAVE_NUM, waveCount_);
    waterRippleFilter->SetParam(RS_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X, rippleCenterX_);
    waterRippleFilter->SetParam(RS_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y, rippleCenterY_);
    waterRippleFilter->SetParam(RS_FILTER_WATER_RIPPLE_RIPPLE_MODE, rippleMode_);
    visualEffectContainer->AddToChainedFilter(waterRippleFilter);
}
} // namespace Rosen
} // namespace OHOS