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
    // default TextStyle
    FontWeight fontWeight_ = FontWeight::W400;
    FontStyle fontStyle_ = FontStyle::NORMAL;
    std::vector<std::string> fontFamilies_ = {};
    double fontSize_ = 16.0;
    double heightScale_ = 1.0;
    bool heightOnly_ = false;
    std::string locale_;

    // multi-text
    size_t maxLines_ = 1e9;
    std::u16string ellipsis_ = u"...";
    BreakStrategy breakStrategy_ = BreakStrategy::GREEDY;
    WordBreakType wordBreakType_ = WordBreakType::BREAK_WORD;
    TextAlign align_ = TextAlign::START;
    TextDirection direction_ = TextDirection::LTR;

    // lineStyle
    bool useLineStyle_ = false;
    struct LineStyle {
        bool only_ = false;
        FontWeight fontWeight_ = FontWeight::W400;
        FontStyle fontStyle_ = FontStyle::NORMAL;
        std::vector<std::string> fontFamilies_ = {};
        bool heightOnly_ = false;
        double fontSize_ = 16.0;
        double heightScale_ = 1;
        std::optional<double> spacingScale_ = std::nullopt;
    } lineStyle_;

    /*
     * @brief Returns the equivalent align by TextAlign and TextDirection.
     */
    TextAlign GetEquivalentAlign() const;

    /*
     * @brief Returns the default TextStyle.
     */
    TextStyle ConvertToTextStyle() const;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_TYPOGRAPHY_STYLE_H
