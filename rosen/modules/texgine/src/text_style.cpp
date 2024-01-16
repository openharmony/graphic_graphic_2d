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

#include "texgine/text_style.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
void FontFeatures::SetFeature(const std::string &ftag, int fvalue)
{
    features_[ftag] = fvalue;
}

const std::map<std::string, int> &FontFeatures::GetFeatures() const
{
    return features_;
}

bool FontFeatures::operator ==(const FontFeatures &rhs) const
{
    return features_ == rhs.features_;
}

bool TextShadow::HasShadow() const
{
    return blurLeave > 0 || offsetX != 0 || offsetY != 0;
}

bool TextShadow::operator ==(TextShadow const &rhs) const
{
    return offsetX == rhs.offsetX &&
           offsetY == rhs.offsetY &&
           color == rhs.color &&
           blurLeave == rhs.blurLeave;
}

bool TextStyle::operator ==(TextStyle const &rhs) const
{
    return color == rhs.color &&
           fontSize == rhs.fontSize &&
           fontFamilies == rhs.fontFamilies &&
           halfLeading == rhs.halfLeading &&
           heightOnly == rhs.heightOnly &&
           heightScale == rhs.heightScale &&
           decoration == rhs.decoration &&
           decorationColor == rhs.decorationColor &&
           decorationStyle == rhs.decorationStyle &&
           decorationThicknessScale == rhs.decorationThicknessScale &&
           shadows == rhs.shadows &&
           fontWeight == rhs.fontWeight &&
           fontStyle == rhs.fontStyle &&
           fontFeature == rhs.fontFeature &&
           letterSpacing == rhs.letterSpacing &&
           wordSpacing == rhs.wordSpacing &&
           foreground == rhs.foreground &&
           background == rhs.background &&
           backgroundRect == rhs.backgroundRect &&
           styleId == rhs.styleId &&
           isSymbolGlyph == rhs.isSymbolGlyph &&
           symbol == rhs.symbol;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
