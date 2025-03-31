/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rosen_text/text_style.h"

#include <sstream>

namespace OHOS {
namespace Rosen {
void FontFeatures::SetFeature(std::string tag, int value)
{
    featureSet_.emplace_back(std::make_pair(tag, value));
}

std::string FontFeatures::GetFeatureSettings() const
{
    if (featureSet_.empty()) {
        return "";
    }

    std::stringstream ss;
    for (const auto &[tag, value] : featureSet_) {
        if (ss.tellp()) {
            ss << ',';
        }
        ss << tag << '=' << value;
    }
    return ss.str();
}

const std::vector<std::pair<std::string, int>>& FontFeatures::GetFontFeatures() const
{
    return featureSet_;
}

bool FontFeatures::operator ==(const FontFeatures& rhs) const
{
    return featureSet_ == rhs.featureSet_;
}

void FontFeatures::Clear()
{
    featureSet_.clear();
}

void FontVariations::SetAxisValue(const std::string& tag, float value)
{
    axis_[tag] = value;
}

const std::map<std::string, float>& FontVariations::GetAxisValues() const
{
    return axis_;
}

bool FontVariations::operator ==(const FontVariations& rhs) const
{
    return axis_ == rhs.axis_;
}

void FontVariations::Clear()
{
    axis_.clear();
}

TextShadow::TextShadow()
{
}

TextShadow::TextShadow(Drawing::Color shadowColor, Drawing::Point shadowOffset, double shadowBlurRadius)
{
    color = shadowColor;
    offset = shadowOffset;
    blurRadius = shadowBlurRadius;
}

bool TextShadow::operator ==(const TextShadow& rhs) const
{
    return color == rhs.color && offset == rhs.offset && blurRadius == rhs.blurRadius;
}

bool TextShadow::operator !=(const TextShadow& rhs) const
{
    return !(*this == rhs);
}

bool TextShadow::HasShadow() const
{
    return offset.GetX() != 0 || offset.GetY() != 0 || fabs(blurRadius) >= DBL_EPSILON;
}

bool RectStyle::operator ==(const RectStyle& rhs) const
{
    return color == rhs.color && leftTopRadius == rhs.leftTopRadius &&
        rightTopRadius == rhs.rightTopRadius &&
        rightBottomRadius == rhs.rightBottomRadius &&
        leftBottomRadius == rhs.leftBottomRadius;
}

bool RectStyle::operator !=(const RectStyle& rhs) const
{
    return !(*this == rhs);
}

bool TextStyle::operator ==(const TextStyle& rhs) const
{
    return color == rhs.color &&
        decoration == rhs.decoration &&
        decorationColor == rhs.decorationColor &&
        decorationStyle == rhs.decorationStyle &&
        decorationThicknessScale == rhs.decorationThicknessScale &&
        fontWeight == rhs.fontWeight &&
        fontStyle == rhs.fontStyle &&
        baseline == rhs.baseline &&
        fontFamilies == rhs.fontFamilies &&
        fontSize == rhs.fontSize &&
        letterSpacing == rhs.letterSpacing &&
        wordSpacing == rhs.wordSpacing &&
        heightScale == rhs.heightScale &&
        halfLeading == rhs.halfLeading &&
        heightOnly == rhs.heightOnly &&
        locale == rhs.locale &&
        foregroundBrush == rhs.foregroundBrush &&
        foregroundPen == rhs.foregroundPen &&
        backgroundBrush == rhs.backgroundBrush &&
        backgroundPen == rhs.backgroundPen &&
        backgroundRect == rhs.backgroundRect &&
        styleId == rhs.styleId &&
        shadows == rhs.shadows &&
        fontFeatures == rhs.fontFeatures &&
        fontVariations == rhs.fontVariations;
}

bool TextStyle::EqualByFonts(const TextStyle &rhs) const
{
    return !isPlaceholder && !rhs.isPlaceholder &&
        fontStyle == rhs.fontStyle &&
        fontFamilies == rhs.fontFamilies &&
        fontFeatures == rhs.fontFeatures &&
        fontVariations == rhs.fontVariations &&
        Drawing::IsScalarAlmostEqual(letterSpacing, rhs.letterSpacing) &&
        Drawing::IsScalarAlmostEqual(wordSpacing, rhs.wordSpacing) &&
        Drawing::IsScalarAlmostEqual(heightScale, rhs.heightScale) &&
        Drawing::IsScalarAlmostEqual(baseLineShift, rhs.baseLineShift) &&
        Drawing::IsScalarAlmostEqual(fontSize, rhs.fontSize) &&
        locale == rhs.locale;
}

bool TextStyle::MatchOneAttribute(StyleType styleType, const TextStyle &rhs) const
{
    switch (styleType) {
        case FOREGROUND:
            return (!foregroundBrush.has_value() && !rhs.foregroundBrush.has_value() &&
                !foregroundPen.has_value() && !rhs.foregroundPen.has_value() && color == rhs.color) ||
                (foregroundBrush == rhs.foregroundBrush &&
                foregroundPen == rhs.foregroundPen);
        case BACKGROUND:
            return backgroundBrush == rhs.backgroundBrush &&
                backgroundPen == rhs.backgroundPen;
        case SHADOW:
            return shadows == rhs.shadows;
        case DECORATIONS:
            return decoration == rhs.decoration &&
                decorationColor == rhs.decorationColor &&
                decorationStyle == rhs.decorationStyle &&
                Drawing::IsScalarAlmostEqual(decorationThicknessScale,
                    rhs.decorationThicknessScale);
        case LETTER_SPACING:
            return Drawing::IsScalarAlmostEqual(letterSpacing, rhs.letterSpacing);

        case WORD_SPACING:
            return Drawing::IsScalarAlmostEqual(wordSpacing, rhs.wordSpacing);

        case ALL_ATTRIBUTES:
            return *this == rhs;

        case FONT:
            return fontStyle == rhs.fontStyle &&
                locale == rhs.locale &&
                fontFamilies == rhs.fontFamilies &&
                Drawing::IsScalarAlmostEqual(fontSize, rhs.fontSize) &&
                Drawing::IsScalarAlmostEqual(heightScale, rhs.heightScale) &&
                halfLeading == rhs.halfLeading &&
                Drawing::IsScalarAlmostEqual(baseLineShift, rhs.baseLineShift);
        default:
            return false;
    }
}
} // namespace Rosen
} // namespace OHOS
