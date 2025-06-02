/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "render/rs_dispersion_shader_filter.h"
#include "ge_shader_filter_params.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSDispersionShaderFilter::RSDispersionShaderFilter(const DispersionShaderFilterParams& param)
    : mask_(param.mask), opacity_(param.opacity), redOffsetX_(param.redOffset[0]), redOffsetY_(param.redOffset[1]),
      greenOffsetX_(param.greenOffset[0]), greenOffsetY_(param.greenOffset[1]), blueOffsetX_(param.blueOffset[0]),
      blueOffsetY_(param.blueOffset[1])
{
    type_ = ShaderFilterType::DISPERSION;

#ifndef ENABLE_M133_SKIA
    const auto hashFunc = SkOpts::hash;
#else
    const auto hashFunc = SkChecksum::Hash32;
#endif
    if (mask_) {
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
    }
    hash_ = hashFunc(&opacity_, sizeof(opacity_), hash_);
    hash_ = hashFunc(&redOffsetX_, sizeof(redOffsetX_), hash_);
    hash_ = hashFunc(&redOffsetY_, sizeof(redOffsetY_), hash_);
    hash_ = hashFunc(&greenOffsetX_, sizeof(greenOffsetX_), hash_);
    hash_ = hashFunc(&greenOffsetY_, sizeof(greenOffsetY_), hash_);
    hash_ = hashFunc(&blueOffsetX_, sizeof(blueOffsetX_), hash_);
    hash_ = hashFunc(&blueOffsetY_, sizeof(blueOffsetY_), hash_);
}

void RSDispersionShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (visualEffectContainer == nullptr) {
        return;
    }
    if (mask_ == nullptr) {
        ROSEN_LOGW("RSDispersionShaderFilter::GenerateGEVisualEffect mask_ is nullptr.");
    }

    auto dispersionShaderFilter = std::make_shared<Drawing::GEVisualEffect>(
        Drawing::GE_FILTER_DISPERSION, Drawing::DrawingPaintType::BRUSH);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_MASK,
        mask_ != nullptr ? mask_->GenerateGEShaderMask() : nullptr);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_OPACITY, opacity_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_RED_OFFSET_X, redOffsetX_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_RED_OFFSET_Y, redOffsetY_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_GREEN_OFFSET_X, greenOffsetX_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_GREEN_OFFSET_Y, greenOffsetY_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_BLUE_OFFSET_X, blueOffsetX_);
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_BLUE_OFFSET_Y, blueOffsetY_);
    visualEffectContainer->AddToChainedFilter(dispersionShaderFilter);
}

const std::shared_ptr<Rosen::RSShaderMask>& RSDispersionShaderFilter::GetMask() const
{
    return mask_;
}

} // namespace Rosen
} // namespace OHOS