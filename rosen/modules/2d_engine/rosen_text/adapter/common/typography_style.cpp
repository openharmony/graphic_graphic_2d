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
        .fontWeight_ = fontWeight_,
        .fontStyle_ = fontStyle_,
        .fontFamilies_ = { fontFamily_ },
        .fontSize_ = fontSize_,
        .heightScale_ = heightScale_,
        .heightOnly_ = heightOnly_,
        .locale_ = locale_,
    };
    if (fontSize_ >= 0) {
        style.fontSize_ = fontSize_;
    }

    return style;
}

TextAlign TypographyStyle::GetEffectiveAlign() const
{
    if (textAlign_ == TextAlign::START) {
        return (textDirection_ == TextDirection::LTR) ? TextAlign::LEFT : TextAlign::RIGHT;
    } else if (textAlign_ == TextAlign::END) {
        return (textDirection_ == TextDirection::RTL) ? TextAlign::LEFT : TextAlign::RIGHT;
    } else {
        return textAlign_;
    }
}

bool TypographyStyle::IsUnlimitedLines() const
{
    return maxLines_ == 1e9;
}

bool TypographyStyle::IsEllipsized() const
{
    return !ellipsis_.empty();
}
} // namespace Rosen
} // namespace OHOS
