/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "render/rs_material_filter.h"

#include <unordered_map>

#include "include/effects/SkBlurImageFilter.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float BLUR_SIGMA_SCALE = 0.57735f;
// material blur style params
struct MaterialParam {
    float radius;
    float saturation;
    SkColor maskColor;
};
// style to MaterialParam map
std::unordered_map<MATERIAL_BLUR_STYLE, MaterialParam> materialParams_ {
    // card blur params
    { STYLE_CARD_THIN_LIGHT,         { 75.0f,  1.22, 0x6BF0F0F0 } },
    { STYLE_CARD_LIGHT,              { 50.0f,  1.8,  0x99FAFAFA } },
    { STYLE_CARD_THICK_LIGHT,        { 75.0f,  2.4,  0xB8FAFAFA } },
    { STYLE_CARD_THIN_DARK,          { 75.0f,  1.35, 0x6B1A1A1A } },
    { STYLE_CARD_DARK,               { 50.0f,  2.15, 0xD11F1F1F } },
    { STYLE_CARD_THICK_DARK,         { 75.0f,  2.15, 0xD11F1F1F } },
    // background blur params
    { STYLE_BACKGROUND_SMALL_LIGHT,  { 15.0f,  1.2,  0x4C666666 } },
    { STYLE_BACKGROUND_MEDIUM_LIGHT, { 55.0f,  1.5,  0x4C262626 } },
    { STYLE_BACKGROUND_LARGE_LIGHT,  { 75.0f,  1.5,  0x4D262626 } },
    { STYLE_BACKGROUND_XLARGE_LIGHT, { 120.0f, 1.3,  0x4C666666 } },
    { STYLE_BACKGROUND_SMALL_DARK,   { 15.0f,  1.1,  0x800D0D0D } },
    { STYLE_BACKGROUND_MEDIUM_DARK,  { 55.0f,  1.15, 0x800D0D0D } },
    { STYLE_BACKGROUND_LARGE_DARK,   { 75.0f,  1.5,  0x800D0D0D } },
    { STYLE_BACKGROUND_XLARGE_DARK,  { 130.0f, 1.3,  0x800D0D0D } },
};
} // namespace

RSMaterialFilter::RSMaterialFilter(int style, float dipScale, BLUR_COLOR_MODE mode)
    : RSSkiaFilter(RSMaterialFilter::CreateMaterialStyle(static_cast<MATERIAL_BLUR_STYLE>(style), dipScale)),
      dipScale_(dipScale), style_(static_cast<MATERIAL_BLUR_STYLE>(style)), colorMode_(mode)
{
    type_ = FilterType::MATERIAL;
}

RSMaterialFilter::~RSMaterialFilter() = default;

float RSMaterialFilter::RadiusVp2Sigma(float radiusVp, float dipScale)
{
    float radiusPx = radiusVp * dipScale;
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + SK_ScalarHalf : 0.0f;
}

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialFilter(float radius, float sat, SkColor maskColor)
{
    maskColor_ = maskColor;
    sk_sp<SkImageFilter> blurFilter = SkBlurImageFilter::Make(radius, radius, nullptr, nullptr,
        SkBlurImageFilter::kClamp_TileMode); // blur
    SkColorMatrix cm;
    cm.setSaturation(sat);
    sk_sp<SkColorFilter> satFilter = SkColorFilters::Matrix(cm); // saturation

    return SkImageFilters::ColorFilter(satFilter, blurFilter);
}

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialStyle(MATERIAL_BLUR_STYLE style, float dipScale)
{
    if (materialParams_.find(style) != materialParams_.end()) {
        MaterialParam materialParam = materialParams_[style];
        return RSMaterialFilter::CreateMaterialFilter(RSMaterialFilter::RadiusVp2Sigma(materialParam.radius, dipScale),
            materialParam.saturation, materialParam.maskColor);
    }
    // ??
    return nullptr;
}

void RSMaterialFilter::PreProcess(sk_sp<SkImage> imageSnapshot)
{
    if (colorMode_ == AVERAGE && imageSnapshot != nullptr) {
        // update maskColor while persevere alpha
        maskColor_ = SkColorSetA(RSPropertiesPainter::CalcAverageColor(imageSnapshot), SkColorGetA(maskColor_));
    }
}

void RSMaterialFilter::PostProcess(RSPaintFilterCanvas& canvas)
{
    SkPaint paint;
    paint.setColor(maskColor_);
    canvas.drawPaint(paint);
}

std::shared_ptr<RSFilter> RSMaterialFilter::Add(const std::shared_ptr<RSFilter>& rhs)
{
    return shared_from_this();
}

std::shared_ptr<RSFilter> RSMaterialFilter::Sub(const std::shared_ptr<RSFilter>& rhs)
{
    return shared_from_this();
}

std::shared_ptr<RSFilter> RSMaterialFilter::Multiply(float rhs)
{
    return shared_from_this();
}

std::shared_ptr<RSFilter> RSMaterialFilter::Negate()
{
    return shared_from_this();
}
} // namespace Rosen
} // namespace OHOS
