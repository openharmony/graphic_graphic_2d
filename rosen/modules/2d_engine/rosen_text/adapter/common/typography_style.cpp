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

#include "rosen_text/typography_style.h"

namespace OHOS {
namespace Rosen {
TextStyle TypographyStyle::GetTextStyle() const
{
    TextStyle style = {
#ifndef USE_GRAPHIC_TEXT_GINE
        .fontWeight_ = fontWeight_,
        .fontStyle_ = fontStyle_,
        .fontFamilies_ = { fontFamily_ },
        .fontSize_ = fontSize_,
        .heightScale_ = heightScale_,
        .halfLeading_ = halfLeading_,
        .heightOnly_ = heightOnly_,
        .locale_ = locale_,
#else
        .fontWeight = fontWeight,
        .fontStyle = fontStyle,
        .fontFamilies = { fontFamily },
        .fontSize = fontSize,
        .heightScale = heightScale,
        .halfLeading = halfLeading,
        .heightOnly = heightOnly,
        .locale = locale,
#endif
    };
#ifndef USE_GRAPHIC_TEXT_GINE
    if (fontSize_ >= 0) {
        style.fontSize_ = fontSize_;
#else
    if (fontSize >= 0) {
        style.fontSize = fontSize;
#endif
    }

    return style;
}

TextAlign TypographyStyle::GetEffectiveAlign() const
{
#ifndef USE_GRAPHIC_TEXT_GINE
    if (textAlign_ == TextAlign::START) {
        return (textDirection_ == TextDirection::LTR) ? TextAlign::LEFT : TextAlign::RIGHT;
    } else if (textAlign_ == TextAlign::END) {
        return (textDirection_ == TextDirection::RTL) ? TextAlign::LEFT : TextAlign::RIGHT;
#else
    if (textAlign == TextAlign::START) {
        return (textDirection == TextDirection::LTR) ? TextAlign::LEFT : TextAlign::RIGHT;
    } else if (textAlign == TextAlign::END) {
        return (textDirection == TextDirection::RTL) ? TextAlign::LEFT : TextAlign::RIGHT;
#endif
    } else {
#ifndef USE_GRAPHIC_TEXT_GINE
        return textAlign_;
#else
        return textAlign;
#endif
    }
}

bool TypographyStyle::IsUnlimitedLines() const
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return maxLines_ == 1e9;
#else
    return maxLines == 1e9;     // maximum number of lines
#endif
}

void TypographyStyle::SetTextStyle(TextStyle& textstyle)
{
    customTextStyle = true;
    insideTextStyle = textstyle;
}

bool TypographyStyle::IsEllipsized() const
{
#ifndef USE_GRAPHIC_TEXT_GINE
    return !ellipsis_.empty();
#else
    return !ellipsis.empty();
#endif
}
} // namespace Rosen
} // namespace OHOS
