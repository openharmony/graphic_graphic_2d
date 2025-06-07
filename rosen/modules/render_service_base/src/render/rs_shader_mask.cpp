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

#include "ge_pixel_map_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

#include "common/rs_vector4.h"
#include "platform/common/rs_log.h"
#include "render/rs_render_pixel_map_mask.h"
#include "render/rs_render_ripple_mask.h"
#include "utils/rect.h"

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
            if (!rippleMask) {
                ROSEN_LOGE("RSShaderMask::GenerateGEShaderMask rippleMask null");
                return nullptr;
            }
            auto center = rippleMask->GetAnimatRenderProperty<Vector2f>(RSUIFilterType::RIPPLE_MASK_CENTER);
            auto radius = rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_RADIUS);
            auto width  = rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_WIDTH);
            auto widthCenterOffset =
                rippleMask->GetAnimatRenderProperty<float>(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET);
            if (!center || !radius || !width || !widthCenterOffset) {
                ROSEN_LOGE("RSShaderMask::GenerateGEShaderMask property null");
                return nullptr;
            }
            Drawing::GERippleShaderMaskParams maskParam { std::make_pair(center->Get().x_, center->Get().y_),
                radius->Get(), width->Get(), widthCenterOffset->Get() };
            return std::make_shared<Drawing::GERippleShaderMask>(maskParam);
        }
        case RSUIFilterType::PIXEL_MAP_MASK: {
            auto pixelMapMask = std::static_pointer_cast<RSRenderPixelMapMaskPara>(renderMask_);
            auto image = pixelMapMask->GetImage();
            auto srcProp = pixelMapMask->GetRenderAnimatableProperty<Vector4f>(RSUIFilterType::PIXEL_MAP_MASK_SRC);
            auto dstProp = pixelMapMask->GetRenderAnimatableProperty<Vector4f>(RSUIFilterType::PIXEL_MAP_MASK_DST);
            auto fillColorProp =
                pixelMapMask->GetRenderAnimatableProperty<Vector4f>(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
            if (image == nullptr || srcProp == nullptr || dstProp == nullptr || fillColorProp == nullptr) {
                ROSEN_LOGE("RSShaderMask::GenerateGEShaderMask pixel map mask some property not found");
                return nullptr;
            }
            Drawing::RectF srcRect(srcProp->Get().x_, srcProp->Get().y_, srcProp->Get().z_, srcProp->Get().w_);
            Drawing::RectF dstRect(dstProp->Get().x_, dstProp->Get().y_, dstProp->Get().z_, dstProp->Get().w_);
            Drawing::GEPixelMapMaskParams maskParam { image, srcRect, dstRect, fillColorProp->Get() };
            return std::make_shared<Drawing::GEPixelMapShaderMask>(maskParam);
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
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif

    switch (renderMask_->GetType()) {
        case RSUIFilterType::RIPPLE_MASK: {
            auto rippleMask = std::static_pointer_cast<RSRenderRippleMaskPara>(renderMask_);
            if (!rippleMask) { return; }
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
        case RSUIFilterType::PIXEL_MAP_MASK: {
            auto pixelMapMask = std::static_pointer_cast<RSRenderPixelMapMaskPara>(renderMask_);
            auto image = pixelMapMask->GetImage();
            auto srcProp = pixelMapMask->GetRenderAnimatableProperty<Vector4f>(RSUIFilterType::PIXEL_MAP_MASK_SRC);
            auto dstProp = pixelMapMask->GetRenderAnimatableProperty<Vector4f>(RSUIFilterType::PIXEL_MAP_MASK_DST);
            auto fillColorProp =
                pixelMapMask->GetRenderAnimatableProperty<Vector4f>(RSUIFilterType::PIXEL_MAP_MASK_FILL_COLOR);
            if (image == nullptr || srcProp == nullptr || dstProp == nullptr || fillColorProp == nullptr) {
                ROSEN_LOGE("RSShaderMask::CalHash pixel map mask some property not found");
                return;
            }
            auto imageUniqueID = image->GetUniqueID();
            auto src = srcProp->Get();
            auto dst = dstProp->Get();
            auto fillColor = fillColorProp->Get();
            hash_ = hashFunc(&imageUniqueID, sizeof(imageUniqueID), hash_);
            hash_ = hashFunc(&src, sizeof(src), hash_);
            hash_ = hashFunc(&dst, sizeof(dst), hash_);
            hash_ = hashFunc(&fillColor, sizeof(fillColor), hash_);
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