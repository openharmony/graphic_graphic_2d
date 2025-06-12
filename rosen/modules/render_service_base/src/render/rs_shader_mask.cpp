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
#include "ge_radial_gradient_shader_mask.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

#include "common/rs_vector4.h"
#include "platform/common/rs_log.h"
#include "render/rs_render_pixel_map_mask.h"
#include "render/rs_render_radial_gradient_mask.h"
#include "render/rs_render_ripple_mask.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

RSShaderMask::RSShaderMask(const std::shared_ptr<RSRenderMaskPara>& renderMask)
{
    renderMask_ = (renderMask != nullptr ? renderMask->LimitedDeepCopy() : nullptr);
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
        case RSUIFilterType::RADIAL_GRADIENT_MASK: {
            auto radialGradientMask = std::static_pointer_cast<RSRenderRadialGradientMaskPara>(renderMask_);
            if (!radialGradientMask) {
                ROSEN_LOGE("RSShaderMask::GenerateGEShaderMask radialGradientMask null");
                return nullptr;
            }
            auto center = radialGradientMask->GetAnimatRenderProperty<Vector2f>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_CENTER);
            auto radiusX = radialGradientMask->GetAnimatRenderProperty<float>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSX);
            auto radiusY = radialGradientMask->GetAnimatRenderProperty<float>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSY);
            auto colors = radialGradientMask->GetAnimatRenderProperty<std::vector<float>>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_COLORS);
            auto positions = radialGradientMask->GetAnimatRenderProperty<std::vector<float>>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_POSITIONS);
            if (!center || !radiusX || !radiusY || !colors || !positions) {
                ROSEN_LOGE("RSShaderMask::GenerateGEShaderMask radialGradientMask property null");
                return nullptr;
            }
            Drawing::GERadialGradientShaderMaskParams maskParam { std::make_pair(center->Get().x_, center->Get().y_),
                radiusX->Get(), radiusY->Get(), colors->Get(), positions->Get() };
            return std::make_shared<Drawing::GERadialGradientShaderMask>(maskParam);
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
            auto pixelMapMaskHash = pixelMapMask->CalcHash();
            hash_ = hashFunc(&pixelMapMaskHash, sizeof(pixelMapMaskHash), hash_);
            break;
        }
        case RSUIFilterType::RADIAL_GRADIENT_MASK: {
            auto radialGradientMask = std::static_pointer_cast<RSRenderRadialGradientMaskPara>(renderMask_);
            if (!radialGradientMask) {
                return;
            }
            auto centerProp = radialGradientMask->GetAnimatRenderProperty<Vector2f>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_CENTER);
            auto radiusXProp = radialGradientMask->GetAnimatRenderProperty<float>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSX);
            auto radiusYProp = radialGradientMask->GetAnimatRenderProperty<float>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSY);
            auto colorsProp = radialGradientMask->GetAnimatRenderProperty<std::vector<float>>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_COLORS);
            auto positionsProp = radialGradientMask->GetAnimatRenderProperty<std::vector<float>>(
                RSUIFilterType::RADIAL_GRADIENT_MASK_POSITIONS);
            if (centerProp && radiusXProp && radiusYProp && colorsProp && positionsProp) {
                auto center = centerProp->Get();
                auto radiusX = radiusXProp->Get();
                auto radiusY = radiusYProp->Get();
                auto colors = colorsProp->Get();
                auto positions = positionsProp->Get();
                hash_ = hashFunc(&center, sizeof(center), hash_);
                hash_ = hashFunc(&radiusX, sizeof(radiusX), hash_);
                hash_ = hashFunc(&radiusY, sizeof(radiusY), hash_);
                hash_ = hashFunc(&colors, sizeof(colors), hash_);
                hash_ = hashFunc(&positions, sizeof(positions), hash_);
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