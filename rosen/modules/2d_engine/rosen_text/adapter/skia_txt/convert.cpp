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
const std::string WGHT_AXIS = "wght";
constexpr float FONT_WEIGHT_MULTIPLE = 100.0;

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
        .fontWidth = static_cast<SPText::FontWidth>(style.fontWidth),
        .fontStyle = static_cast<SPText::FontStyle>(style.fontStyle),
        .wordBreakType = static_cast<SPText::WordBreakType>(style.wordBreakType),
        .fontFamily = style.fontFamily,
        .fontSize = style.fontSize,
        .height = style.heightScale,
        .heightOverride = style.heightOnly,
        .strutEnabled = style.useLineStyle,
        .strutFontWeight = static_cast<SPText::FontWeight>(style.lineStyleFontWeight),
        .strutFontWidth = static_cast<SPText::FontWidth>(style.lineStyleFontWidth),
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
        .breakStrategy = static_cast<SPText::BreakStrategy>(style.breakStrategy),
        .tab = Convert(style.tab),
        .paragraphSpacing = style.paragraphSpacing,
        .isEndAddParagraphSpacing = style.isEndAddParagraphSpacing,
        .relayoutChangeBitmap = style.relayoutChangeBitmap,
        .defaultTextStyleUid = style.defaultTextStyleUid,
        .halfLeading = style.halfLeading,
        .isTrailingSpaceOptimized = style.isTrailingSpaceOptimized,
        .enableAutoSpace = style.enableAutoSpace,
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
    const int start = 1; // The starting position of string.
    const int end = static_cast<int>(str.size()) - 2; // End position of string.
    return str.substr(start, end); // Remove quotation marks from both ends.
}

void CopyTextStyleSymbol(const TextStyle& style, SPText::TextStyle& textStyle)
{
    textStyle.symbol.SetSymbolType(style.symbol.GetSymbolType());
    textStyle.symbol.SetRenderColor(style.symbol.GetRenderColor());
    textStyle.symbol.SetRenderMode(style.symbol.GetRenderMode());
    textStyle.symbol.SetSymbolEffect(style.symbol.GetEffectStrategy());
    textStyle.symbol.SetAnimationMode(style.symbol.GetAnimationMode());
    textStyle.symbol.SetRepeatCount(style.symbol.GetRepeatCount());
    textStyle.symbol.SetAnimationStart(style.symbol.GetAnimationStart());
    textStyle.symbol.SetCommonSubType(style.symbol.GetCommonSubType());
    textStyle.symbol.SetSymbolUid(style.symbol.GetSymbolUid());
    textStyle.symbol.SetSymbolBitmap(style.symbol.GetSymbolBitmap());
    for (auto [tag, value] : style.symbol.GetVisualMap()) {
        textStyle.fontFeatures.SetFeature(RemoveQuotes(tag), value);
    }
}

void SplitTextStyleConvert(SPText::TextStyle& textStyle, const TextStyle& style)
{
    if (style.isSymbolGlyph) {
        CopyTextStyleSymbol(style, textStyle);
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

    if (!style.fontVariations.GetAxisValues().empty()) {
        for (const auto& [axis, value] : style.fontVariations.GetAxisValues()) {
            textStyle.fontVariations.SetAxisValue(axis, value);
        }
    } else {
        textStyle.fontVariations.SetAxisValue(WGHT_AXIS,
            (static_cast<float>(style.fontWeight) + 1.0) * FONT_WEIGHT_MULTIPLE);
    }
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
    textStyle.fontWidth = static_cast<SPText::FontWidth>(style.fontWidth);
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
    textStyle.textStyleUid = style.textStyleUid;
    textStyle.isSymbolGlyph = style.isSymbolGlyph;
    textStyle.baseLineShift = style.baseLineShift;
    textStyle.isPlaceholder = style.isPlaceholder;
    textStyle.relayoutChangeBitmap = style.relayoutChangeBitmap;
    SplitTextStyleConvert(textStyle, style);

    return textStyle;
}

void CopyTextStyleSymbol(const SPText::TextStyle& style, TextStyle& textStyle)
{
    textStyle.symbol.SetRenderColor(style.symbol.GetRenderColor());
    textStyle.symbol.SetRenderMode(style.symbol.GetRenderMode());
    textStyle.symbol.SetSymbolEffect(style.symbol.GetEffectStrategy());
    textStyle.symbol.SetAnimationMode(style.symbol.GetAnimationMode());
    textStyle.symbol.SetRepeatCount(style.symbol.GetRepeatCount());
    textStyle.symbol.SetAnimationStart(style.symbol.GetAnimationStart());
    textStyle.symbol.SetCommonSubType(style.symbol.GetCommonSubType());
    textStyle.symbol.SetSymbolUid(style.symbol.GetSymbolUid());
}

void SplitTextStyleConvert(TextStyle& textStyle, const SPText::TextStyle& style)
{
    if (style.isSymbolGlyph) {
        CopyTextStyleSymbol(style, textStyle);
    }

    if (style.background.has_value()) {
        textStyle.backgroundBrush = style.background->brush;
        textStyle.backgroundPen = style.background->pen;
    }

    if (style.foreground.has_value()) {
        textStyle.foregroundBrush = style.foreground->brush;
        textStyle.foregroundPen = style.foreground->pen;
    }

    for (const auto& [color, offset, radius] : style.textShadows) {
        Drawing::Color shadowColor;
        shadowColor.SetColorQuad(color);
        Drawing::Point shadowOffset(offset.x(), offset.y());
        textStyle.shadows.emplace_back(shadowColor, shadowOffset, radius);
    }

    for (const auto& [tag, value] : style.fontFeatures.GetFontFeatures()) {
        textStyle.fontFeatures.SetFeature(RemoveQuotes(tag), value);
    }

    if (!style.fontVariations.GetAxisValues().empty()) {
        for (const auto& [axis, value] : style.fontVariations.GetAxisValues()) {
            textStyle.fontVariations.SetAxisValue(axis, value);
        }
    }
}

TextStyle Convert(const SPText::TextStyle& style)
{
    TextStyle textStyle;
    textStyle.color.SetColorQuad(style.color);
    textStyle.decoration = static_cast<TextDecoration>(style.decoration);
    textStyle.decorationColor.SetColorQuad(style.decorationColor);
    textStyle.decorationStyle = static_cast<TextDecorationStyle>(style.decorationStyle);
    textStyle.decorationThicknessScale = style.decorationThicknessMultiplier;
    textStyle.fontWeight = static_cast<FontWeight>(style.fontWeight);
    textStyle.fontWidth = static_cast<FontWidth>(style.fontWidth);
    textStyle.fontStyle = static_cast<FontStyle>(style.fontStyle);
    textStyle.baseline = static_cast<TextBaseline>(style.baseline);

    textStyle.halfLeading = style.halfLeading;
    textStyle.fontFamilies = style.fontFamilies;
    textStyle.fontSize = style.fontSize;
    textStyle.letterSpacing = style.letterSpacing;
    textStyle.wordSpacing = style.wordSpacing;
    textStyle.heightScale = style.height;
    textStyle.heightOnly = style.heightOverride;
    textStyle.locale = style.locale;
    textStyle.backgroundRect = { style.backgroundRect.color, style.backgroundRect.leftTopRadius,
        style.backgroundRect.rightTopRadius, style.backgroundRect.rightBottomRadius,
        style.backgroundRect.leftBottomRadius };
    textStyle.styleId = style.styleId;
    textStyle.textStyleUid = style.textStyleUid;
    textStyle.isSymbolGlyph = style.isSymbolGlyph;
    textStyle.baseLineShift = style.baseLineShift;
    textStyle.isPlaceholder = style.isPlaceholder;
    SplitTextStyleConvert(textStyle, style);

    return textStyle;
}

SPText::TextTab Convert(const TextTab& tab)
{
    return {
        static_cast<SPText::TextAlign>(tab.alignment),
        tab.location,
    };
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
