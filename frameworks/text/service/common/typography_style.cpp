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
TypographyStyle::TypographyStyle(const TypographyStyle& other)
    : fontWeight(other.fontWeight),
      fontWidth(other.fontWidth),
      fontStyle(other.fontStyle),
      fontFamily(other.fontFamily),
      fontSize(other.fontSize),
      heightScale(other.heightScale),
      halfLeading(other.halfLeading),
      heightOnly(other.heightOnly),
      useLineStyle(other.useLineStyle),
      lineStyleFontWeight(other.lineStyleFontWeight),
      lineStyleFontWidth(other.lineStyleFontWidth),
      lineStyleFontStyle(other.lineStyleFontStyle),
      lineStyleFontFamilies(other.lineStyleFontFamilies),
      lineStyleFontSize(other.lineStyleFontSize),
      lineStyleHeightScale(other.lineStyleHeightScale),
      lineStyleHeightOnlyInit(other.lineStyleHeightOnlyInit),
      lineStyleHeightOnly(other.lineStyleHeightOnly),
      lineStyleHalfLeading(other.lineStyleHalfLeading),
      lineStyleSpacingScale(other.lineStyleSpacingScale),
      lineStyleOnly(other.lineStyleOnly),
      textAlign(other.textAlign),
      textDirection(other.textDirection),
      maxLines(other.maxLines),
      ellipsis(other.ellipsis),
      locale(other.locale),
      breakStrategy(other.breakStrategy),
      wordBreakType(other.wordBreakType),
      ellipsisModal(other.ellipsisModal),
      textSplitRatio(other.textSplitRatio),
      paragraphSpacing(other.paragraphSpacing),
      isEndAddParagraphSpacing(other.isEndAddParagraphSpacing),
      isTrailingSpaceOptimized(other.isTrailingSpaceOptimized),
      enableAutoSpace(other.enableAutoSpace),
      verticalAlignment(other.verticalAlignment),
      insideTextStyle(other.insideTextStyle),
      customTextStyle(other.customTextStyle),
      textHeightBehavior(other.textHeightBehavior),
      hintingIsOn(other.hintingIsOn),
      tab(other.tab),
      relayoutChangeBitmap(other.relayoutChangeBitmap),
      defaultTextStyleUid(other.defaultTextStyleUid) {}

TypographyStyle& TypographyStyle::operator=(const TypographyStyle& other)
{
    if (this == &other) {
        return *this;
    }

    fontWeight = other.fontWeight;
    fontWidth = other.fontWidth;
    fontStyle = other.fontStyle;
    fontFamily = other.fontFamily;
    fontSize = other.fontSize;
    heightScale = other.heightScale;
    halfLeading = other.halfLeading;
    heightOnly = other.heightOnly;
    useLineStyle = other.useLineStyle;
    lineStyleFontWeight = other.lineStyleFontWeight;
    lineStyleFontWidth = other.lineStyleFontWidth;
    lineStyleFontStyle = other.lineStyleFontStyle;
    lineStyleFontFamilies = other.lineStyleFontFamilies;
    lineStyleFontSize = other.lineStyleFontSize;
    lineStyleHeightScale = other.lineStyleHeightScale;
    lineStyleHeightOnlyInit = other.lineStyleHeightOnlyInit;
    lineStyleHeightOnly = other.lineStyleHeightOnly;
    lineStyleHalfLeading = other.lineStyleHalfLeading;
    lineStyleSpacingScale = other.lineStyleSpacingScale;
    lineStyleOnly = other.lineStyleOnly;
    textAlign = other.textAlign;
    textDirection = other.textDirection;
    maxLines = other.maxLines;
    ellipsis = other.ellipsis;
    locale = other.locale;
    breakStrategy = other.breakStrategy;
    wordBreakType = other.wordBreakType;
    ellipsisModal = other.ellipsisModal;
    textSplitRatio = other.textSplitRatio;
    paragraphSpacing = other.paragraphSpacing;
    isEndAddParagraphSpacing = other.isEndAddParagraphSpacing;
    isTrailingSpaceOptimized = other.isTrailingSpaceOptimized;
    enableAutoSpace = other.enableAutoSpace;
    verticalAlignment = other.verticalAlignment;
    insideTextStyle = other.insideTextStyle;
    customTextStyle = other.customTextStyle;
    textHeightBehavior = other.textHeightBehavior;
    hintingIsOn = other.hintingIsOn;
    tab = other.tab;
    relayoutChangeBitmap = other.relayoutChangeBitmap;
    defaultTextStyleUid = other.defaultTextStyleUid;

    return *this;
}

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
