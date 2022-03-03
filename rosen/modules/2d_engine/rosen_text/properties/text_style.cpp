/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include "txt/text_style.h"

#include "rosen_text/properties/rosen_converter_txt.h"
#include "rosen_text/properties/text_style.h"
#include "txt/platform.h"

namespace rosen {
void FontFeatures::SetFeature(std::string tag, int value)
{
    featureMap_[tag] = value;
}

std::string FontFeatures::GetFeatureSettings() const
{
    // set font variant (its map, need set one by one)
    txt::FontFeatures features;
    if (!featureMap_.empty()) {
        for (auto iter = featureMap_.begin(); iter != featureMap_.end(); ++iter) {
            features.SetFeature(iter->first, iter->second);
        }
    }
    return features.GetFeatureSettings();
}

TextShadow::TextShadow(OHOS::Rosen::Drawing::Color color, OHOS::Rosen::Drawing::Point offset, double blurRadius)
    : color_(color),
    offset_(offset),
    blurRadius_(blurRadius)
{
}

bool TextShadow::operator==(const TextShadow& rhs) const
{
    if (color_ != rhs.color_ || offset_ != rhs.offset_ || blurRadius_ != rhs.blurRadius_) {
        return false;
    }
    return true;
}

bool TextShadow::operator != (const TextShadow& rhs) const
{
    return !(*this == rhs);
}

bool TextShadow::hasShadow() const
{
    if (offset_.GetX()  != 0.0 || offset_.GetY() != 0.0 || blurRadius_ != 0.0) {
        return true;
    }
    return false;
}

TextStyle::TextStyle()
{
    fontFamilies_ = std::vector<std::string>(1, txt::GetDefaultFontFamily());
}

bool TextStyle::equals(const TextStyle& rhs) const
{
    txt::TextStyle curTextStyle;
    txt::TextStyle rhsTextStyle;
    RosenConvertTxtStyle(*this, curTextStyle);
    RosenConvertTxtStyle(rhs, rhsTextStyle);
    return curTextStyle.equals(rhsTextStyle);
}
} // namespace rosen
