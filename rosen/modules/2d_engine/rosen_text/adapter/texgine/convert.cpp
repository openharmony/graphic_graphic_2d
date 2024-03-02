/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Rosen {
namespace AdapterTextEngine {
std::shared_ptr<OHOS::Rosen::AdapterTextEngine::FontCollection> Convert(
    const std::shared_ptr<OHOS::Rosen::FontCollection> &fontCollection)
{
    return std::static_pointer_cast<OHOS::Rosen::AdapterTextEngine::FontCollection>(fontCollection);
}

TextEngine::TypographyStyle Convert(const TypographyStyle &style)
{
    TextEngine::TypographyStyle ys = {
        .fontWeight = Convert(style.fontWeight),
        .fontStyle = Convert(style.fontStyle),
        .fontFamilies = { style.fontFamily },
        .fontSize = style.fontSize,
        .heightScale = style.heightScale,
        .halfLeading = style.halfLeading,
        .heightOnly = style.heightOnly,
        .locale = style.locale,
        .maxLines = style.maxLines,
        .ellipsis = style.ellipsis,
        .breakStrategy = Convert(style.breakStrategy),
        .wordBreakType = Convert(style.wordBreakType),
        .align = Convert(style.textAlign),
        .direction = Convert(style.textDirection),
        .ellipsisModal = Convert(style.ellipsisModal),
        .useLineStyle = style.useLineStyle,
        .lineStyle = {
            .only = style.lineStyleOnly,
            .fontWeight = Convert(style.lineStyleFontWeight),
            .fontStyle = Convert(style.lineStyleFontStyle),
            .fontFamilies = style.lineStyleFontFamilies,
            .halfLeading = style.lineStyleHalfLeading,
            .heightOnly = style.lineStyleHeightOnly,
            .fontSize = style.lineStyleFontSize,
            .heightScale = style.lineStyleHeightScale,
        },
        .textSplitRatio = style.textSplitRatio,
    };
    if (style.lineStyleSpacingScale >= 0) {
        ys.lineStyle.spacingScale = style.lineStyleSpacingScale;
    }
    return ys;
}

TextEngine::TextStyle Convert(const TextStyle &style)
{
    auto color = SkColorSetARGB(
        style.color.GetAlpha(), style.color.GetRed(), style.color.GetGreen(), style.color.GetBlue());
    auto decorationColor = SkColorSetARGB(style.decorationColor.GetAlpha(),
        style.decorationColor.GetRed(), style.decorationColor.GetGreen(), style.decorationColor.GetBlue());

    std::optional<TextEngine::TexginePaint> foreground = std::nullopt;
    if (style.foregroundBrush.has_value() || style.foregroundPen.has_value()) {
        foreground = TextEngine::TexginePaint();
        if (style.foregroundBrush.has_value() && style.foregroundPen.has_value()) {
            foreground.value().SetStyle(TextEngine::TexginePaint::Style::STROKEANDFILL);
        } else if (style.foregroundBrush.has_value()) {
            foreground.value().SetStyle(TextEngine::TexginePaint::Style::FILL);
        } else if (style.foregroundPen.has_value()) {
            foreground.value().SetStyle(TextEngine::TexginePaint::Style::STROKE);
        }
        if (style.foregroundBrush.has_value()) {
            foreground.value().SetBrush(style.foregroundBrush.value());
        }
        if (style.foregroundPen.has_value()) {
            foreground.value().SetPen(style.foregroundPen.value());
        }
    }

    std::optional<TextEngine::TexginePaint> background = std::nullopt;
    if (style.backgroundBrush.has_value() || style.backgroundPen.has_value()) {
        background = TextEngine::TexginePaint();
        if (style.backgroundBrush.has_value()) {
            background.value().SetBrush(style.backgroundBrush.value());
        }
        if (style.backgroundPen.has_value()) {
            background.value().SetPen(style.backgroundPen.value());
        }
    }

    TextEngine::TextStyle xs = {
        .fontWeight = Convert(style.fontWeight),
        .fontStyle = Convert(style.fontStyle),
        .fontFamilies = style.fontFamilies,
        .fontSize = style.fontSize,
        .decoration = Convert(style.decoration),
        .decorationColor = decorationColor,
        .decorationStyle = Convert(style.decorationStyle),
        .decorationThicknessScale = style.decorationThicknessScale,
        .color = color,
        .baseline = Convert(style.baseline),
        .locale = style.locale,
        .halfLeading = style.halfLeading,
        .heightOnly = style.heightOnly,
        .heightScale = style.heightScale,
        .letterSpacing = style.letterSpacing,
        .wordSpacing = style.wordSpacing,
        .foreground = foreground,
        .background = background,
        .backgroundRect = Convert(style.backgroundRect),
        .styleId = style.styleId,
        .isSymbolGlyph = style.isSymbolGlyph,
    };
    if (style.isSymbolGlyph) {
        xs.symbol.colorList_ = style.symbol.GetRenderColor();
        xs.symbol.renderMode_ = style.symbol.GetRenderMode();
        xs.symbol.effectStrategy_ = style.symbol.GetEffectStrategy();
    }

    for (const auto &[tag, value] : style.fontFeatures.GetFontFeatures()) {
        xs.fontFeature.SetFeature(tag, value);
    }

    for (const auto &[color, offset, radius] : style.shadows) {
        // 24, 16, 8, 0: How many bits are moved to the right
        auto shadowColor = (color.GetAlpha() << 24) | (color.GetRed() << 16) |
            (color.GetGreen() << 8) | (color.GetBlue() << 0);
        TextEngine::TextShadow shadow = {
            .offsetX = offset.GetX(),
            .offsetY = offset.GetY(),
            .color = shadowColor,
            .blurLeave = radius,
        };
        xs.shadows.emplace_back(shadow);
    }
    return xs;
}

IndexAndAffinity Convert(const TextEngine::IndexAndAffinity &pos)
{
    return { pos.index, Convert(pos.affinity) };
}

Boundary Convert(const TextEngine::Boundary &range)
{
    return { range.leftIndex, range.rightIndex };
}

TextRect Convert(const TextEngine::TextRect &box)
{
    Drawing::RectF rect(*box.rect.fLeft_, *box.rect.fTop_, *box.rect.fRight_, *box.rect.fBottom_);
    return { rect, Convert(box.direction) };
}

Affinity Convert(const TextEngine::Affinity &affinity)
{
    switch (affinity) {
        case TextEngine::Affinity::PREV:
            return Affinity::PREV;
        case TextEngine::Affinity::NEXT:
            return Affinity::NEXT;
    }
    return Affinity::PREV;
}

TextDirection Convert(const TextEngine::TextDirection &direction)
{
    switch (direction) {
        case TextEngine::TextDirection::LTR:
            return TextDirection::LTR;
        case TextEngine::TextDirection::RTL:
            return TextDirection::RTL;
    }
    return TextDirection::LTR;
}

TextEngine::TextRectHeightStyle Convert(const TextRectHeightStyle &style)
{
    switch (style) {
        case TextRectHeightStyle::TIGHT:
            return TextEngine::TextRectHeightStyle::TIGHT;
        case TextRectHeightStyle::COVER_TOP_AND_BOTTOM:
            return TextEngine::TextRectHeightStyle::COVER_TOP_AND_BOTTOM;
        case TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM:
            return TextEngine::TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM;
        case TextRectHeightStyle::COVER_TOP:
            return TextEngine::TextRectHeightStyle::COVER_TOP;
        case TextRectHeightStyle::COVER_BOTTOM:
            return TextEngine::TextRectHeightStyle::COVER_BOTTOM;
        case TextRectHeightStyle::FOLLOW_BY_STRUT:
            return TextEngine::TextRectHeightStyle::FOLLOW_BY_LINE_STYLE;
    }
    return TextEngine::TextRectHeightStyle::TIGHT;
}

TextEngine::TextRectWidthStyle Convert(const TextRectWidthStyle &style)
{
    switch (style) {
        case TextRectWidthStyle::TIGHT:
            return TextEngine::TextRectWidthStyle::TIGHT;
        case TextRectWidthStyle::MAX:
            return TextEngine::TextRectWidthStyle::MAX_WIDTH;
    }
    return TextEngine::TextRectWidthStyle::TIGHT;
}

TextEngine::WordBreakType Convert(const WordBreakType &style)
{
    switch (style) {
        case WordBreakType::NORMAL:
            return TextEngine::WordBreakType::NORMAL;
        case WordBreakType::BREAK_ALL:
            return TextEngine::WordBreakType::BREAK_ALL;
        case WordBreakType::BREAK_WORD:
            return TextEngine::WordBreakType::BREAK_WORD;
    }
    return TextEngine::WordBreakType::NORMAL;
}

TextEngine::BreakStrategy Convert(const BreakStrategy &style)
{
    switch (style) {
        case BreakStrategy::GREEDY:
            return TextEngine::BreakStrategy::GREEDY;
        case BreakStrategy::HIGH_QUALITY:
            return TextEngine::BreakStrategy::HIGH_QUALITY;
        case BreakStrategy::BALANCED:
            return TextEngine::BreakStrategy::BALANCED;
    }
    return TextEngine::BreakStrategy::GREEDY;
}

TextEngine::FontWeight Convert(const FontWeight &style)
{
    switch (style) {
        case FontWeight::W100:
            return TextEngine::FontWeight::W100;
        case FontWeight::W200:
            return TextEngine::FontWeight::W200;
        case FontWeight::W300:
            return TextEngine::FontWeight::W300;
        case FontWeight::W400:
            return TextEngine::FontWeight::W400;
        case FontWeight::W500:
            return TextEngine::FontWeight::W500;
        case FontWeight::W600:
            return TextEngine::FontWeight::W600;
        case FontWeight::W700:
            return TextEngine::FontWeight::W700;
        case FontWeight::W800:
            return TextEngine::FontWeight::W800;
        case FontWeight::W900:
            return TextEngine::FontWeight::W900;
    }
    return TextEngine::FontWeight::W400;
}

TextEngine::FontStyle Convert(const FontStyle &style)
{
    switch (style) {
        case FontStyle::NORMAL:
            return TextEngine::FontStyle::NORMAL;
        case FontStyle::ITALIC:
            return TextEngine::FontStyle::ITALIC;
    }
    return TextEngine::FontStyle::NORMAL;
}

TextEngine::TextAlign Convert(const TextAlign &align)
{
    switch (align) {
        case TextAlign::LEFT:
            return TextEngine::TextAlign::LEFT;
        case TextAlign::RIGHT:
            return TextEngine::TextAlign::RIGHT;
        case TextAlign::CENTER:
            return TextEngine::TextAlign::CENTER;
        case TextAlign::JUSTIFY:
            return TextEngine::TextAlign::JUSTIFY;
        case TextAlign::START:
            return TextEngine::TextAlign::START;
        case TextAlign::END:
            return TextEngine::TextAlign::END;
    }
    return TextEngine::TextAlign::START;
}

TextEngine::TextBaseline Convert(const TextBaseline &baseline)
{
    switch (baseline) {
        case TextBaseline::ALPHABETIC:
            return TextEngine::TextBaseline::ALPHABETIC;
        case TextBaseline::IDEOGRAPHIC:
            return TextEngine::TextBaseline::IDEOGRAPHIC;
    }
    return TextEngine::TextBaseline::ALPHABETIC;
}

TextEngine::TextDirection Convert(const TextDirection &direction)
{
    switch (direction) {
        case TextDirection::LTR:
            return TextEngine::TextDirection::LTR;
        case TextDirection::RTL:
            return TextEngine::TextDirection::RTL;
    }
    return TextEngine::TextDirection::LTR;
}

TextEngine::TextDecorationStyle Convert(const TextDecorationStyle &style)
{
    switch (style) {
        case TextDecorationStyle::SOLID:
            return TextEngine::TextDecorationStyle::SOLID;
        case TextDecorationStyle::DOUBLE:
            return TextEngine::TextDecorationStyle::DOUBLE;
        case TextDecorationStyle::DOTTED:
            return TextEngine::TextDecorationStyle::DOTTED;
        case TextDecorationStyle::DASHED:
            return TextEngine::TextDecorationStyle::DASHED;
        case TextDecorationStyle::WAVY:
            return TextEngine::TextDecorationStyle::WAVY;
    }
    return TextEngine::TextDecorationStyle::SOLID;
}

TextEngine::TextDecoration Convert(const TextDecoration &decoration)
{
    TextEngine::TextDecoration retval = TextEngine::TextDecoration::NONE;
    if (decoration & TextDecoration::UNDERLINE) {
        retval |= TextEngine::TextDecoration::UNDERLINE;
    }
    if (decoration & TextDecoration::OVERLINE) {
        retval |= TextEngine::TextDecoration::OVERLINE;
    }
    if (decoration & TextDecoration::LINE_THROUGH) {
        retval |= TextEngine::TextDecoration::LINE_THROUGH;
    }
    return retval;
}

TextEngine::AnySpanAlignment Convert(const PlaceholderVerticalAlignment &alignment)
{
    switch (alignment) {
        case PlaceholderVerticalAlignment::OFFSET_AT_BASELINE:
            return TextEngine::AnySpanAlignment::OFFSET_AT_BASELINE;
        case PlaceholderVerticalAlignment::ABOVE_BASELINE:
            return TextEngine::AnySpanAlignment::ABOVE_BASELINE;
        case PlaceholderVerticalAlignment::BELOW_BASELINE:
            return TextEngine::AnySpanAlignment::BELOW_BASELINE;
        case PlaceholderVerticalAlignment::TOP_OF_ROW_BOX:
            return TextEngine::AnySpanAlignment::TOP_OF_ROW_BOX;
        case PlaceholderVerticalAlignment::BOTTOM_OF_ROW_BOX:
            return TextEngine::AnySpanAlignment::BOTTOM_OF_ROW_BOX;
        case PlaceholderVerticalAlignment::CENTER_OF_ROW_BOX:
            return TextEngine::AnySpanAlignment::CENTER_OF_ROW_BOX;
    }
    return TextEngine::AnySpanAlignment::OFFSET_AT_BASELINE;
}

TextEngine::EllipsisModal Convert(const EllipsisModal &ellipsisModal)
{
    switch (ellipsisModal) {
        case EllipsisModal::HEAD:
            return TextEngine::EllipsisModal::HEAD;
        case EllipsisModal::MIDDLE:
            return TextEngine::EllipsisModal::MIDDLE;
        case EllipsisModal::TAIL:
        default:
            return TextEngine::EllipsisModal::TAIL;
    }
}

TextEngine::RectStyle Convert(const RectStyle &rectStyle)
{
    return { rectStyle.color, rectStyle.leftTopRadius, rectStyle.rightTopRadius,
        rectStyle.rightBottomRadius, rectStyle.leftBottomRadius };
}
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS
