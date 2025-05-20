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

#include "render/rs_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#include "render/rs_render_ripple_mask.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSShaderMask::RSShaderMask(const std::shared_ptr<RSRenderMaskPara>& renderMask) : renderMask_(renderMask)
{
    CalHash();
}

std::shared_ptr<Drawing::GEShaderMask> RSShaderMask::GenerateGEShaderMask() const
{
    if (renderMask_ == nullptr) {
        return nullptr;
    }
    switch (renderMask_->GetType()) {
        case RSUIFilterType::RIPPLE_MASK: {
            auto rippleMask = std::static_pointer_cast<RSRenderRippleMaskPara>(renderMask_);
            auto center = rippleMask->GetAnimatRenderProperty<Vector2f>(RSUIFilterType::RIPPLE_MASK_CENTER)->Get();
            auto radius = rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_RADIUS)->Get();
            auto width  = rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_WIDTH)->Get();
            auto widthCenterOffset =
                rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET)->Get();
            Drawing::GERippleShaderMaskParams maskParam { std::make_pair(center.x_, center.y_), radius, width,
                widthCenterOffset };
            return std::make_shared<Drawing::GERippleShaderMask>(maskParam);
        }
        default: {
            return nullptr;
        }
    }
}

void RSShaderMask::CalHash()
{
    if (!renderMask_) {
        return;
    }
#ifndef ENABLE_M133_SKIA
    const auto hashFunc = SkOpts::hash;
#else
    const auto hashFunc = SkChecksum::Hash32;
#endif

    switch (renderMask_->GetType()) {
        case RSUIFilterType::RIPPLE_MASK: {
            auto rippleMask = std::static_pointer_cast<RSRenderRippleMaskPara>(renderMask_);
            auto centerProp = rippleMask->GetAnimatRenderProperty<Vector2f>(RSUIFilterType::RIPPLE_MASK_CENTER);
            auto radiusProp = rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_RADIUS);
            auto widthProp = rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_WIDTH);
            auto widthCenterOffsetProp =
                rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET);
            if (centerProp && radiusProp && widthProp && widthCenterOffsetProp) {
                auto center = centerProp->Get();
                auto radius = radiusProp->Get();
                auto width = widthProp->Get();
                auto widthCenterOffset = widthCenterOffsetProp->Get();
                hash_ = hashFunc(&center, sizeof(center), hash_);
                hash_ = hashFunc(&radius, sizeof(radius), hash_);
                hash_ = hashFunc(&width, sizeof(width), hash_);
                hash_ = hashFunc(&widthCenterOffset, sizeof(widthCenterOffset), hash_);
            }
            break;
        }
        default: {
            break;
        }
    }
    return;
}
} // namespace Rosen
} // namespace OHOS