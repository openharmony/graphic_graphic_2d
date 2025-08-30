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

#include "modules/skparagraph/include/TextStyle.h"

namespace OHOS {
namespace Rosen {
bool TypographyStyle::operator==(const TypographyStyle &rhs) const
{
    return
        this->verticalAlignment == rhs.verticalAlignment &&
        this->ELLIPSIS == rhs.ELLIPSIS &&
        this->fontWeight == rhs.fontWeight &&
        this->fontWidth == rhs.fontWidth &&
        this->fontStyle == rhs.fontStyle &&
        this->fontFamily == rhs.fontFamily &&
        skia::textlayout::nearlyEqual(this->fontSize, rhs.fontSize) &&
        skia::textlayout::nearlyEqual(this->heightScale, rhs.heightScale) &&
        this->halfLeading == rhs.halfLeading &&
        this->heightOnly == rhs.heightOnly &&
        this->useLineStyle == rhs.useLineStyle &&
        this->lineStyleFontWidth == rhs.lineStyleFontWidth &&
        this->lineStyleFontWeight == rhs.lineStyleFontWeight &&
        this->lineStyleFontStyle == rhs.lineStyleFontStyle &&
        this->lineStyleFontFamilies == rhs.lineStyleFontFamilies &&
        skia::textlayout::nearlyEqual(this->lineStyleFontSize, rhs.lineStyleFontSize) &&
        skia::textlayout::nearlyEqual(this->lineStyleHeightScale, rhs.lineStyleHeightScale) &&
        this->lineStyleHeightOnlyInit == rhs.lineStyleHeightOnlyInit &&
        this->lineStyleHeightOnly == rhs.lineStyleHeightOnly &&
        this->lineStyleHalfLeading == rhs.lineStyleHalfLeading &&
        skia::textlayout::nearlyEqual(this->lineStyleSpacingScale, rhs.lineStyleSpacingScale) &&
        this->lineStyleOnly == rhs.lineStyleOnly &&
        this->textAlign == rhs.textAlign &&
        this->textDirection == rhs.textDirection &&
        this->maxLines == rhs.maxLines &&
        this->ellipsis == rhs.ellipsis &&
        this->locale == rhs.locale &&
        this->breakStrategy == rhs.breakStrategy &&
        this->wordBreakType == rhs.wordBreakType &&
        this->ellipsisModal == rhs.ellipsisModal &&
        skia::textlayout::nearlyEqual(this->textSplitRatio, rhs.textSplitRatio) &&
        this->defaultTextStyleUid == rhs.defaultTextStyleUid &&
        this->tab == rhs.tab &&
        this->paragraphSpacing == rhs.paragraphSpacing &&
        this->isEndAddParagraphSpacing == rhs.isEndAddParagraphSpacing &&
        this->isTrailingSpaceOptimized == rhs.isTrailingSpaceOptimized &&
        this->enableAutoSpace == rhs.enableAutoSpace;
}

TextStyle TypographyStyle::GetTextStyle() const
{
    TextStyle style;
    style.fontWeight = fontWeight;
    style.fontWidth = fontWidth;
    style.fontStyle = fontStyle;
    style.fontFamilies = { fontFamily };
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

bool TextTab::operator==(const TextTab& rhs) const
{
    return (this->alignment == rhs.alignment) && (skia::textlayout::nearlyEqual(this->location, rhs.location));
}
} // namespace Rosen
} // namespace OHOS
