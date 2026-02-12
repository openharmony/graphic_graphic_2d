/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "text_style.h"

#include <sstream>

#include "include/TextStyle.h"
#include "txt/platform.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;

void FontFeatures::SetFeature(std::string tag, int value)
{
    features_.emplace_back(std::make_pair(tag, value));
}

std::string FontFeatures::GetFeatureSettings() const
{
    if (features_.empty()) {
        return "";
    }

    std::ostringstream stream;

    for (const auto& kv : features_) {
        if (stream.tellp()) {
            stream << ',';
        }
        stream << kv.first << '=' << kv.second;
    }

    return stream.str();
}

const std::vector<std::pair<std::string, int>>& FontFeatures::GetFontFeatures() const
{
    return features_;
}

void FontVariations::SetAxisValue(std::string tag, float value)
{
    axis_[tag] = value;
}

const std::map<std::string, float>& FontVariations::GetAxisValues() const
{
    return axis_;
}

TextShadow::TextShadow() {}

TextShadow::TextShadow(SkColor color, SkPoint offset, double blurSigma)
    : color(color), offset(offset), blurSigma(blurSigma)
{}

bool TextShadow::operator==(const TextShadow& other) const
{
    return color == other.color &&
        offset == other.offset &&
        blurSigma == other.blurSigma;
}

bool TextShadow::operator!=(const TextShadow& other) const
{
    return !(*this == other);
}

bool TextShadow::HasShadow() const
{
    constexpr double blurThreshold = 0.5;
    return !offset.isZero() || blurSigma > blurThreshold;
}

TextStyle::TextStyle() : fontFamilies(SPText::DefaultFamilyNameMgr::GetInstance().GetDefaultFontFamilies()) {}

bool TextStyle::operator==(TextStyle const& other) const
{
    return color == other.color &&
        colorPlaceholder == other.colorPlaceholder &&
        decoration == other.decoration &&
        decorationColor == other.decorationColor &&
        decorationStyle == other.decorationStyle &&
        decorationColorPlaceholder == other.decorationColorPlaceholder &&
        skt::nearlyEqual(decorationThicknessMultiplier, other.decorationThicknessMultiplier) &&
        fontWeight == other.fontWeight &&
        fontWidth == other.fontWidth &&
        fontStyle == other.fontStyle &&
        skt::nearlyEqual(letterSpacing, other.letterSpacing) &&
        skt::nearlyEqual(wordSpacing, other.wordSpacing) &&
        skt::nearlyEqual(height, other.height) &&
        heightOverride == other.heightOverride &&
        halfLeading == other.halfLeading &&
        locale == other.locale &&
        styleId == other.styleId &&
        backgroundRect == other.backgroundRect &&
        foreground == other.foreground &&
        fontFamilies == other.fontFamilies &&
        textShadows == other.textShadows &&
        skt::nearlyEqual(fontSize, other.fontSize) &&
        skt::nearlyEqual(maxLineHeight, other.maxLineHeight) &&
        skt::nearlyEqual(minLineHeight, other.minLineHeight) &&
        lineHeightStyle == other.lineHeightStyle &&
        badgeType == other.badgeType &&
        fontEdging == other.fontEdging;
}

bool TextStyle::operator!=(TextStyle const& other) const
{
    return !(*this == other);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
