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
#include <cmath>
#include <chrono>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

// ge
#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

// rs
#include "effect/rs_render_shape_base.h"
#include "render/rs_sdf_effect_filter.h"
// rs helper func
#include "platform/common/rs_log.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif


namespace OHOS {
namespace Rosen {
RSSDFEffectFilter::RSSDFEffectFilter(std::shared_ptr<RSNGRenderShapeBase> SDFShape)
    : RSDrawingFilterOriginal(nullptr)
{
    type_ = FilterType::SDF_EFFECT;
    SDFShape_ = SDFShape;

    std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = SDFShape_ ? SDFShape_->GenerateGEVisualEffect() : nullptr;
    std::shared_ptr<Drawing::GEShaderShape> geShape = geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
    geFilter_ =
        std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_SDF, Drawing::DrawingPaintType::BRUSH);
    
    geContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    geContainer_->AddToChainedFilter(geFilter_);

    geRender_ = std::make_shared<GraphicsEffectEngine::GERender>();

#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&type_, sizeof(type_), 0);
    if (SDFShape_) {
        auto shapeHash = SDFShape_->CalculateHash();
        hash_ = hashFunc(&shapeHash, sizeof(shapeHash), hash_);
    }
}

RSSDFEffectFilter::~RSSDFEffectFilter() = default;

std::string RSSDFEffectFilter::GetDescription()
{
    return "RSSDFEffect";
}

void RSSDFEffectFilter::SetBorder(const Drawing::Color& color, float width)
{
    borderColor_ = color;
    borderWidth_ = width;
    hasBorder_ = true;
}

const Drawing::Color& RSSDFEffectFilter::GetBorderColor() const
{
    return borderColor_;
}

float RSSDFEffectFilter::GetBorderWidth() const
{
    return borderWidth_;
}

bool RSSDFEffectFilter::HasBorder() const
{
    return hasBorder_;
}

void RSSDFEffectFilter::SetShadow(
    const Drawing::Color& color, float offsetX, float offsetY, float radius, Drawing::Path path, bool isFilled)
{
    shadowColor_ = color;
    shadowOffsetX_ = offsetX;
    shadowOffsetY_ = offsetY;
    shadowRadius_ = radius;
    shadowPath_ = path;
    isShadowFilled_ = isFilled;
    hasShadow_ = true;
}

const Drawing::Color& RSSDFEffectFilter::GetShadowColor() const
{
    return shadowColor_;
}

float RSSDFEffectFilter::GetShadowOffsetX() const
{
    return shadowOffsetX_;
}

float RSSDFEffectFilter::GetShadowOffsetY() const
{
    return shadowOffsetY_;
}

float RSSDFEffectFilter::GetShadowRadius() const
{
    return shadowRadius_;
}

Drawing::Path RSSDFEffectFilter::GetShadowPath() const
{
    return shadowPath_;
}

bool RSSDFEffectFilter::GetShadowIsFill() const
{
    return isShadowFilled_;
}

bool RSSDFEffectFilter::HasShadow() const
{
    return hasShadow_;
}

void RSSDFEffectFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        ROSEN_LOGE("RSSDFEffectFilter::image error");
        return;
    }

    if (!SDFShape_) {
        return;
    }

    if (!geContainer_) {
        ROSEN_LOGE("RSSDFEffectFilter::GEContainer is null");
    }

    auto outImage = geRender_->ApplyImageEffect(
        canvas, *geContainer_, image, src, dst, Drawing::SamplingOptions());
    if (!outImage) {
        ROSEN_LOGE("RSSDFEffectFilter::DrawImageRect outImage is Null");
    }
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*outImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();

    return;
}

void RSSDFEffectFilter::OnSync()
{
    std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = SDFShape_ ? SDFShape_->GenerateGEVisualEffect() : nullptr;
    std::shared_ptr<Drawing::GEShaderShape> geShape = geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
    geFilter_->SetParam(Drawing::GE_FILTER_SDF_SHAPE, geShape);

    if (HasBorder()) {
        Drawing::GESDFBorderParams border;
        border.color = GetBorderColor();
        border.width = GetBorderWidth();
        geFilter_->SetParam(Drawing::GE_FILTER_SDF_SHAPE, border);
    }

    if (HasShadow()) {
        Drawing::GESDFShadowParams shadow;
        shadow.color = GetShadowColor();
        shadow.offsetX = GetShadowOffsetX();
        shadow.offsetY = GetShadowOffsetY();
        shadow.radius = GetShadowRadius();
        shadow.path = GetShadowPath();
        shadow.isFilled = GetShadowIsFill();
        geFilter_->SetParam(Drawing::GE_FILTER_SDF_SHAPE, shadow);
    }

    geContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    geContainer_->AddToChainedFilter(geFilter_);
}
}
}