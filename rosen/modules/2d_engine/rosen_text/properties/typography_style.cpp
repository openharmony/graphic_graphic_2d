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

#include "rosen_text/properties/typography_style.h"

namespace rosen {
TextStyle TypographyStyle::GetTextStyle() const
{
    TextStyle textstyle;
    textstyle.fontWeight_ = fontWeight_;
    textstyle.fontStyle_ = fontStyle_;
    textstyle.fontFamilies_ = std::vector<std::string>({fontFamily_});
    if (fontSize_ >= 0) {
        textstyle.fontSize_ = fontSize_;
    }
    textstyle.locale_ = locale_;
    textstyle.height_ = height_;
    textstyle.hasHeightOverride_ = hasHeightOverride_;
    return textstyle;
}

TextAlign TypographyStyle::EffectiveAlign() const
{
    if (textAlign_ == TextAlign::START) {
        return (textDirection_ == TextDirection::LTR) ? TextAlign::LEFT
                                                     : TextAlign::RIGHT;
    } else if (textAlign_ == TextAlign::END) {
        return (textDirection_ == TextDirection::LTR) ? TextAlign::RIGHT
                                                    : TextAlign::LEFT;
    } else {
        return textAlign_;
    }
}
} // namespace rosen
