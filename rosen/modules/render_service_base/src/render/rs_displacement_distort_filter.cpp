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
#include "render/rs_displacement_distort_filter.h"

#include "common/rs_vector2.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
RSDisplacementDistortFilter::RSDisplacementDistortFilter(std::shared_ptr<RSShaderMask> mask, Vector2f factor)
    : mask_(mask), factor_(factor)
{
    type_ = ShaderFilterType::DISPLACEMENT_DISTORT;
#ifndef ENABLE_M133_SKIA
    const auto hashFunc = SkOpts::hash;
#else
    const auto hashFunc = SkChecksum::Hash32;
#endif
    hash_ = hashFunc(&factor_, sizeof(factor_), hash_);
    if (mask_) {
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
    }
}

void RSDisplacementDistortFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto distortFilter = std::make_shared<Drawing::GEVisualEffect>("DISPLACEMENT_DISTORT",
        Drawing::DrawingPaintType::BRUSH);
    distortFilter->SetParam(GE_FILTER_DISPLACEMENT_DISTORT_FACTOR, std::make_pair(factor_[0], factor_[1]));
    distortFilter->SetParam(GE_FILTER_DISPLACEMENT_DISTORT_MASK, mask_->GenerateGEShaderMask());
    visualEffectContainer->AddToChainedFilter(distortFilter);
}

const Vector2f& RSDisplacementDistortFilter::GetFactor() const
{
    return factor_;
}

const std::shared_ptr<RSShaderMask>& RSDisplacementDistortFilter::GetMask() const
{
    return mask_;
}
} // Rosen
} // OHOS