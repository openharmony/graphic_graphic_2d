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

namespace OHOS {
namespace Rosen {
// namesapce
namespace {
    constexpr float BLUR_SIGMA_SCALE = 0.57735f;

    // card blur style
    constexpr int STYLE_CARD_THIN_LIGHT = 1;
    constexpr int STYLE_CARD_LIGHT = 2;
    constexpr int STYLE_CARD_THICK_LIGHT = 3;
    constexpr int STYLE_CARD_THIN_DARK = 4;
    constexpr int STYLE_CARD_DARK = 5;
    constexpr int STYLE_CARD_THICK_DARK = 6;

    // background blur style
    constexpr int STYLE_BACKGROUND_SMALL_LIGHT = 101;
    constexpr int STYLE_BACKGROUND_MEDIUM_LIGHT = 102;
    constexpr int STYLE_BACKGROUND_LARGE_LIGHT = 103;
    constexpr int STYLE_BACKGROUND_XLARGE_LIGHT = 104;
    constexpr int STYLE_BACKGROUND_SMALL_DARK = 105;
    constexpr int STYLE_BACKGROUND_MEDIUM_DARK = 106;
    constexpr int STYLE_BACKGROUND_LARGE_DARK = 107;
    constexpr int STYLE_BACKGROUND_XLARGE_DARK = 108;

    // card blur params
    constexpr MaterialParam CARD_THIN_LIGHT = {75.0f, 1.22, 0x6BF0F0F0};
    constexpr MaterialParam CARD_LIGHT = {50.0f, 1.8, 0x99FAFAFA};
    constexpr MaterialParam CARD_THICK_LIGHT = {75.0f, 2.4, 0xB8FAFAFA};
    constexpr MaterialParam CARD_THIN_DARK = {75.0f, 1.35, 0x6B1A1A1A};
    constexpr MaterialParam CARD_DARK = {50.0f, 2.15, 0xD11F1F1F};
    constexpr MaterialParam CARD_THICK_DARK = {75.0f, 2.15, 0xD11F1F1F};

    // background blur params
    constexpr MaterialParam BACKGROUND_SMALL_LIGHT = {15.0f, 1.2, 0x4C666666};
    constexpr MaterialParam BACKGROUND_MEDIUM_LIGHT = {55.0f, 1.5, 0x4C262626};
    constexpr MaterialParam BACKGROUND_LARGE_LIGHT = {75.0f, 1.5, 0x4D262626};
    constexpr MaterialParam BACKGROUND_XLARGE_LIGHT = {120.0f, 1.3, 0x4C666666};
    constexpr MaterialParam BACKGROUND_SMALL_DARK = {15.0f, 1.1, 0x800D0D0D};
    constexpr MaterialParam BACKGROUND_MEDIU_MDARK = {55.0f, 1.15, 0x800D0D0D};
    constexpr MaterialParam BACKGROUND_LARGE_DARK = {75.0f, 1.5, 0x800D0D0D};
    constexpr MaterialParam BACKGROUND_XLARGE_DARK = {130.0f, 1.3, 0x800D0D0D};

    // style2materialparam map
    std::unordered_map<int, MaterialParam> materialParams_ {
        {STYLE_CARD_THIN_LIGHT, CARD_THIN_LIGHT},
        {STYLE_CARD_LIGHT, CARD_LIGHT},
        {STYLE_CARD_THICK_LIGHT, CARD_THICK_LIGHT},
        {STYLE_CARD_THIN_DARK, CARD_THIN_DARK},
        {STYLE_CARD_DARK, CARD_DARK},
        {STYLE_CARD_THICK_DARK, CARD_THICK_DARK},
        {STYLE_BACKGROUND_SMALL_LIGHT, BACKGROUND_SMALL_LIGHT},
        {STYLE_BACKGROUND_MEDIUM_LIGHT, BACKGROUND_MEDIUM_LIGHT},
        {STYLE_BACKGROUND_LARGE_LIGHT, BACKGROUND_LARGE_LIGHT},
        {STYLE_BACKGROUND_XLARGE_LIGHT, BACKGROUND_XLARGE_LIGHT},
        {STYLE_BACKGROUND_SMALL_DARK, BACKGROUND_SMALL_DARK},
        {STYLE_BACKGROUND_MEDIUM_DARK, BACKGROUND_MEDIU_MDARK},
        {STYLE_BACKGROUND_LARGE_DARK, BACKGROUND_LARGE_DARK},
        {STYLE_BACKGROUND_XLARGE_DARK, BACKGROUND_XLARGE_DARK}
    };
}

RSMaterialFilter::RSMaterialFilter(int style, float dipScale)
    : RSSkiaFilter(RSMaterialFilter::CreateMaterialStyle(style, dipScale)), style_(style), dipScale_(dipScale)
{
    type_ = FilterType::MATERIAL;
}

RSMaterialFilter::~RSMaterialFilter() {}

int RSMaterialFilter::GetStyle() const
{
    return style_;
}

float RSMaterialFilter::GetDipScale() const
{
    return dipScale_;
}

float RSMaterialFilter::RadiusVp2Sigma(float radiusVp, float dipScale) const
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

sk_sp<SkImageFilter> RSMaterialFilter::CreateMaterialStyle(int style, float dipScale)
{
    if (materialParams_.find(style) != materialParams_.end()) {
        MaterialParam materialParam = materialParams_[style];
        return RSMaterialFilter::CreateMaterialFilter(
            RSMaterialFilter::RadiusVp2Sigma(materialParam.radius, dipScale),
            materialParam.saturation, materialParam.maskColor);
    }
    return nullptr;
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
