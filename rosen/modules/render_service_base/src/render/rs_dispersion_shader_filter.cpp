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
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
RSDispersionShaderFilter::RSDispersionShaderFilter(const DispersionShaderFilterParams& param)
    : mask_(param.mask), opacity_(param.opacity), redOffset_(param.redOffset), greenOffset_(param.greenOffset),
      blueOffset_(param.blueOffset)
{
    type_ = ShaderFilterType::DISPERSION;

#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    if (mask_) {
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
    }
    hash_ = hashFunc(&opacity_, sizeof(opacity_), hash_);
    hash_ = hashFunc(&redOffset_, sizeof(redOffset_), hash_);
    hash_ = hashFunc(&greenOffset_, sizeof(greenOffset_), hash_);
    hash_ = hashFunc(&blueOffset_, sizeof(blueOffset_), hash_);
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
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_RED_OFFSET,
        std::make_pair(redOffset_[0], redOffset_[1]));
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_GREEN_OFFSET,
        std::make_pair(greenOffset_[0], greenOffset_[1]));
    dispersionShaderFilter->SetParam(Drawing::GE_FILTER_DISPERSION_BLUE_OFFSET,
        std::make_pair(blueOffset_[0], blueOffset_[1]));
    visualEffectContainer->AddToChainedFilter(dispersionShaderFilter);
}

const std::shared_ptr<Rosen::RSShaderMask>& RSDispersionShaderFilter::GetMask() const
{
    return mask_;
}

} // namespace Rosen
} // namespace OHOS