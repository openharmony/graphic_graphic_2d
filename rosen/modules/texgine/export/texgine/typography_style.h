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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_STYLE_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_STYLE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "texgine/font_providers.h"
#include "texgine/text_style.h"
#include "texgine/typography_types.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
/*
 * @brief TypographyStyle is a collection of parameters that control how Typography is displayed,
 *        including parameters for default TextStyle, multi-text, and line style.
 */
struct TypographyStyle {
    const static inline std::u16string ELLIPSIS = u"\u2026";
    // default TextStyle
    FontWeight fontWeight = FontWeight::W400;
    FontStyle fontStyle = FontStyle::NORMAL;
    std::vector<std::string> fontFamilies = {};
    double fontSize = 16.0;
    double heightScale = 1.0;
    bool halfLeading = false;
    bool heightOnly = false;
    std::string locale;

    // multi-text
    size_t maxLines = std::numeric_limits<size_t>::max();
    std::u16string ellipsis;
    BreakStrategy breakStrategy = BreakStrategy::GREEDY;
    WordBreakType wordBreakType = WordBreakType::BREAK_WORD;
    TextAlign align = TextAlign::START;
    TextDirection direction = TextDirection::LTR;
    EllipsisModal ellipsisModal = EllipsisModal::TAIL;

    // lineStyle
    bool useLineStyle = false;
    struct LineStyle {
        bool only = false;
        FontWeight fontWeight = FontWeight::W400;
        FontStyle fontStyle = FontStyle::NORMAL;
        std::vector<std::string> fontFamilies = {};
        bool halfLeading = false;
        bool heightOnly = false;
        double fontSize = 16.0;
        double heightScale = 1;
        std::optional<double> spacingScale = std::nullopt;
    } lineStyle;

    float textSplitRatio = 0.5f;

    /*
     * @brief Returns the equivalent align by TextAlign and TextDirection.
     */
    TextAlign GetEquivalentAlign() const;

    /*
     * @brief Returns the default TextStyle.
     */
    TextStyle ConvertToTextStyle() const;
    bool ellipsizedForNDK = false;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_STYLE_H
