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

#ifndef USE_GRAPHIC_TEXT_GINE
TextShadow::TextShadow(Drawing::Color color, Drawing::Point offset, double blurRadius)
#else
TextShadow::TextShadow(Drawing::Color shadowColor, Drawing::Point shadowOffset, double shadowBlurRadius)
#endif
{
#ifndef USE_GRAPHIC_TEXT_GINE
    color_ = color;
    offset_ = offset;
    blurRadius_ = blurRadius;
#else
    color = shadowColor;
    offset = shadowOffset;
    blurRadius = shadowBlurRadius;
#endif
}

bool TextShadow::operator ==(const TextShadow& rhs) const
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return color_ == rhs.color_ && offset_ == rhs.offset_ && blurRadius_ == rhs.blurRadius_;
#else
    return color == rhs.color && offset == rhs.offset && blurRadius == rhs.blurRadius;
#endif
}

bool TextShadow::operator !=(const TextShadow& rhs) const
{
    return !(*this == rhs);
}

bool TextShadow::HasShadow() const
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return offset_.GetX() != 0 || offset_.GetY() != 0 || blurRadius_ != 0.0;
#else
    return offset.GetX() != 0 || offset.GetY() != 0 || fabs(blurRadius) >= DBL_EPSILON;
#endif
}

bool TextStyle::operator ==(const TextStyle& rhs) const
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return color_ == rhs.color_ &&
           decoration_ == rhs.decoration_ &&
           decorationColor_ == rhs.decorationColor_ &&
           decorationStyle_ == rhs.decorationStyle_ &&
           decorationThicknessScale_ == rhs.decorationThicknessScale_ &&
           fontWeight_ == rhs.fontWeight_ &&
           fontStyle_ == rhs.fontStyle_ &&
           baseline_ == rhs.baseline_ &&
           fontFamilies_ == rhs.fontFamilies_ &&
           fontSize_ == rhs.fontSize_ &&
           letterSpacing_ == rhs.letterSpacing_ &&
           wordSpacing_ == rhs.wordSpacing_ &&
           heightScale_ == rhs.heightScale_ &&
           heightOnly_ == rhs.heightOnly_ &&
           locale_ == rhs.locale_ &&
           background_ == rhs.background_ &&
           foreground_ == rhs.foreground_ &&
           shadows_ == rhs.shadows_ &&
           fontFeatures_ == rhs.fontFeatures_;
#else
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
        heightOnly == rhs.heightOnly &&
        locale == rhs.locale &&
        background == rhs.background &&
        foreground == rhs.foreground &&
        shadows == rhs.shadows &&
        fontFeatures == rhs.fontFeatures;
#endif
}
} // namespace Rosen
} // namespace OHOS
