/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "convert.h"

#include "txt/paint_record.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
std::shared_ptr<OHOS::Rosen::AdapterTxt::FontCollection> Convert(
    const std::shared_ptr<OHOS::Rosen::FontCollection>& fontCollection)
{
    return std::static_pointer_cast<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection);
}

IndexAndAffinity Convert(const SPText::PositionWithAffinity& pos)
{
    return { pos.position, static_cast<Affinity>(pos.affinity) };
}

Boundary Convert(const SPText::Range<size_t>& range)
{
    return { range.start, range.end };
}

TextRect Convert(const SPText::TextBox& box)
{
    Drawing::RectF rect(box.rect.fLeft, box.rect.fTop, box.rect.fRight, box.rect.fBottom);
    return { rect, static_cast<TextDirection>(box.direction) };
}

SPText::RectHeightStyle Convert(const TextRectHeightStyle& style)
{
    return static_cast<SPText::RectHeightStyle>(style);
}

SPText::RectWidthStyle Convert(const TextRectWidthStyle& style)
{
    return static_cast<SPText::RectWidthStyle>(style);
}

SPText::ParagraphStyle Convert(const TypographyStyle& style)
{
    return {
        .fontWeight = static_cast<SPText::FontWeight>(style.fontWeight),
        .fontStyle = static_cast<SPText::FontStyle>(style.fontStyle),
        .wordBreakType = static_cast<SPText::WordBreakType>(style.wordBreakType),
        .fontFamily = style.fontFamily,
        .fontSize = style.fontSize,
        .height = style.heightScale,
        .heightOverride = style.heightOnly,
        .strutEnabled = style.useLineStyle,
        .strutFontWeight = static_cast<SPText::FontWeight>(style.lineStyleFontWeight),
        .strutFontStyle = static_cast<SPText::FontStyle>(style.lineStyleFontStyle),
        .strutFontFamilies = style.lineStyleFontFamilies,
        .strutFontSize = style.lineStyleFontSize,
        .strutHeight = style.lineStyleHeightScale,
        .strutHeightOverride = style.lineStyleHeightOnly,
        .strutHalfLeading = style.lineStyleHalfLeading,
        .strutLeading = style.lineStyleSpacingScale,
        .forceStrutHeight = style.lineStyleOnly,
        .textAlign = static_cast<SPText::TextAlign>(style.textAlign),
        .textDirection = static_cast<SPText::TextDirection>(style.textDirection),
        .ellipsisModal = static_cast<SPText::EllipsisModal>(style.ellipsisModal),
        .maxLines = style.maxLines,
        .ellipsis = style.ellipsis,
        .locale = style.locale,
        .textSplitRatio = style.textSplitRatio,
        .textOverflower = style.Ellipsized(),
        .spTextStyle = Convert(style.insideTextStyle),
        .customSpTextStyle = style.customTextStyle,
        .textHeightBehavior = static_cast<SPText::TextHeightBehavior>(style.textHeightBehavior),
        .hintingIsOn = style.hintingIsOn,
    };
}

SPText::PlaceholderRun Convert(const PlaceholderSpan& run)
{
    return {
        run.width,
        run.height,
        static_cast<SPText::PlaceholderAlignment>(run.alignment),
        static_cast<SPText::TextBaseline>(run.baseline),
        run.baselineOffset,
    };
}

static std::string RemoveQuotes(const std::string& str)
{
    if (str.empty() || str.front() != '\"' || str.back() != '\"') {
        return str;
    }
    auto first = str.find_first_of('\"');
    auto end = str.find_last_of('\"');
    return str.substr(first + 1, end - first - 1);
}

SPText::TextStyle Convert(const TextStyle& style)
{
    SPText::TextStyle textStyle;
    textStyle.color = style.color.CastToColorQuad();
    textStyle.decoration = static_cast<SPText::TextDecoration>(style.decoration);
    auto decorationColor = SkColorSetARGB(style.decorationColor.GetAlpha(), style.decorationColor.GetRed(),
        style.decorationColor.GetGreen(), style.decorationColor.GetBlue());
    textStyle.decorationColor = decorationColor;
    textStyle.decorationStyle = static_cast<SPText::TextDecorationStyle>(style.decorationStyle);
    textStyle.decorationThicknessMultiplier = style.decorationThicknessScale;
    textStyle.fontWeight = static_cast<SPText::FontWeight>(style.fontWeight);
    textStyle.fontStyle = static_cast<SPText::FontStyle>(style.fontStyle);
    textStyle.baseline = static_cast<SPText::TextBaseline>(style.baseline);
    textStyle.halfLeading = style.halfLeading;
    textStyle.fontFamilies = style.fontFamilies;
    textStyle.fontSize = style.fontSize;
    textStyle.letterSpacing = style.letterSpacing;
    textStyle.wordSpacing = style.wordSpacing;
    textStyle.height = style.heightScale;
    textStyle.heightOverride = style.heightOnly;
    textStyle.locale = style.locale;
    textStyle.backgroundRect = { style.backgroundRect.color, style.backgroundRect.leftTopRadius,
        style.backgroundRect.rightTopRadius, style.backgroundRect.rightBottomRadius,
        style.backgroundRect.leftBottomRadius };
    textStyle.styleId = style.styleId;
    textStyle.isSymbolGlyph = style.isSymbolGlyph;
    textStyle.baseLineShift = style.baseLineShift;

    if (style.isSymbolGlyph) {
        textStyle.symbol.SetRenderColor(style.symbol.GetRenderColor());
        textStyle.symbol.SetRenderMode(style.symbol.GetRenderMode());
        textStyle.symbol.SetSymbolEffect(style.symbol.GetEffectStrategy());
    }
    if (style.backgroundBrush.has_value() || style.backgroundPen.has_value()) {
        textStyle.background = SPText::PaintRecord(style.backgroundBrush, style.backgroundPen);
    }
    if (style.foregroundBrush.has_value() || style.foregroundPen.has_value()) {
        textStyle.foreground = SPText::PaintRecord(style.foregroundBrush, style.foregroundPen);
    }

    for (const auto& [color, offset, radius] : style.shadows) {
        auto shadowColor = SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue());
        auto shadowOffset = SkPoint::Make(offset.GetX(), offset.GetY());
        textStyle.textShadows.emplace_back(shadowColor, shadowOffset, radius);
    }

    for (const auto& [tag, value] : style.fontFeatures.GetFontFeatures()) {
        textStyle.fontFeatures.SetFeature(RemoveQuotes(tag), value);
    }
    return textStyle;
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
