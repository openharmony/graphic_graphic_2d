/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

namespace Texgine {
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

bool TextShadow::operator ==(TextShadow const &rhs) const
{
    return offsetX_ == rhs.offsetX_ &&
           offsetY_ == rhs.offsetY_ &&
           color_ == rhs.color_ &&
           blurLeave_ == rhs.blurLeave_;
}

bool TextStyle::operator ==(TextStyle const &rhs) const
{
    return color_ == rhs.color_ &&
           fontSize_ == rhs.fontSize_ &&
           fontFamilies_ == rhs.fontFamilies_ &&
           heightOnly_ == rhs.heightOnly_ &&
           heightScale_ == rhs.heightScale_ &&
           decoration_ == rhs.decoration_ &&
           decorationColor_ == rhs.decorationColor_ &&
           decorationStyle_ == rhs.decorationStyle_ &&
           decorationThicknessScale_ == rhs.decorationThicknessScale_ &&
           shadows_ == rhs.shadows_ &&
           foreground_ == rhs.foreground_ &&
           background_ == rhs.background_ &&
           fontWeight_ == rhs.fontWeight_ &&
           fontStyle_ == rhs.fontStyle_ &&
           fontFeature_ == rhs.fontFeature_ &&
           letterSpacing_ == rhs.letterSpacing_ &&
           wordSpacing_ == rhs.wordSpacing_;
}
} // namespace Texgine
