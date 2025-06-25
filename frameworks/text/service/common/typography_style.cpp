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
    TextStyle style;
    style.fontWeight = fontWeight;
    style.fontStyle = fontStyle;
    style.fontFamilies = { fontFamily };
    style.fontSize = fontSize;
    style.heightScale = heightScale;
    style.halfLeading = halfLeading;
    style.heightOnly = heightOnly;
    style.locale = locale;
    style.textStyleUid = defaultTextStyleUid;

    if (fontSize >= 0) {
        style.fontSize = fontSize;
    }

    return style;
}

TextAlign TypographyStyle::GetEffectiveAlign() const
{
    if (textAlign == TextAlign::START) {
        return (textDirection == TextDirection::LTR) ? TextAlign::LEFT : TextAlign::RIGHT;
    } else if (textAlign == TextAlign::END) {
        return (textDirection == TextDirection::RTL) ? TextAlign::LEFT : TextAlign::RIGHT;
    } else {
        return textAlign;
    }
}

bool TypographyStyle::IsUnlimitedLines() const
{
    return maxLines == 1e9; // maximum number of lines
}

void TypographyStyle::SetTextStyle(TextStyle& textstyle)
{
    customTextStyle = true;
    insideTextStyle = textstyle;
}

bool TypographyStyle::IsEllipsized() const
{
    return !ellipsis.empty();
}
} // namespace Rosen
} // namespace OHOS
