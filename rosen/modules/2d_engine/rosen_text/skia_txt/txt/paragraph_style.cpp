/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "paragraph_style.h"

#include <vector>

namespace OHOS {
namespace Rosen {
namespace SPText {
TextStyle ParagraphStyle::ConvertToTextStyle() const
{
    TextStyle result;
    result.fontWeight = fontWeight;
    result.fontStyle = fontStyle;
    result.fontFamilies = std::vector<std::string>({ fontFamily });
    if (fontSize >= 0) {
        result.fontSize = fontSize;
    }
    result.locale = locale;
    result.height = height;
    result.heightOverride = heightOverride;
    return result;
}

TextAlign ParagraphStyle::GetEquivalentAlign() const
{
    if (textAlign == TextAlign::START) {
        return (textDirection == TextDirection::LTR) ? TextAlign::LEFT : TextAlign::RIGHT;
    } else if (textAlign == TextAlign::END) {
        return (textDirection == TextDirection::LTR) ? TextAlign::RIGHT : TextAlign::LEFT;
    } else {
        return textAlign;
    }
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
