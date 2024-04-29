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

#include "txt/platform.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
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

TextStyle::TextStyle() : fontFamilies(GetDefaultFontFamilies()) {}

bool TextStyle::operator==(TextStyle const& other) const
{
    return color == other.color &&
        decoration == other.decoration &&
        decorationColor == other.decorationColor &&
        decorationStyle == other.decorationStyle &&
        decorationThicknessMultiplier == other.decorationThicknessMultiplier &&
        fontWeight == other.fontWeight &&
        fontStyle == other.fontStyle &&
        letterSpacing == other.letterSpacing &&
        wordSpacing == other.wordSpacing &&
        height == other.height &&
        heightOverride == other.heightOverride &&
        halfLeading == other.halfLeading &&
        locale == other.locale &&
        styleId == other.styleId &&
        backgroundRect == other.backgroundRect &&
        foreground == other.foreground &&
        fontFamilies == other.fontFamilies &&
        textShadows == other.textShadows;
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
