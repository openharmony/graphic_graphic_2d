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
    featureMap_[tag] = value;
}

std::string FontFeatures::GetFeatureSettings() const
{
    if (featureMap_.empty()) {
        return "";
    }

    std::stringstream ss;
    for (const auto &[tag, value] : featureMap_) {
        if (ss.tellp()) {
            ss << ',';
        }
        ss << tag << '=' << value;
    }
    return ss.str();
}

const std::map<std::string, int>& FontFeatures::GetFontFeatures() const
{
    return featureMap_;
}

bool FontFeatures::operator ==(const FontFeatures& rhs) const
{
    return featureMap_ == rhs.featureMap_;
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
        fontFeatures == rhs.fontFeatures;
}
} // namespace Rosen
} // namespace OHOS
