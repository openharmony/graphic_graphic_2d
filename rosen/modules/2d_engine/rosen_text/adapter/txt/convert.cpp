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
namespace AdapterTxt {
std::shared_ptr<OHOS::Rosen::AdapterTxt::FontCollection> Convert(
    const std::shared_ptr<OHOS::Rosen::FontCollection> &fontCollection)
{
    return std::static_pointer_cast<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection);
}

IndexAndAffinity Convert(const txt::Paragraph::PositionWithAffinity &pos)
{
    return { pos.position, static_cast<Affinity>(pos.affinity) };
}

Boundary Convert(const txt::Paragraph::Range<size_t> &range)
{
    return { range.start, range.end };
}

TextRect Convert(const txt::Paragraph::TextBox &box)
{
    Drawing::RectF rect(box.rect.fLeft, box.rect.fTop, box.rect.fRight, box.rect.fBottom);
    return { rect, static_cast<TextDirection>(box.direction) };
}

txt::Paragraph::RectHeightStyle Convert(const TextRectHeightStyle &style)
{
    return static_cast<txt::Paragraph::RectHeightStyle>(style);
}

txt::Paragraph::RectWidthStyle Convert(const TextRectWidthStyle &style)
{
    return static_cast<txt::Paragraph::RectWidthStyle>(style);
}

txt::ParagraphStyle Convert(const TypographyStyle &style)
{
    return {
        .font_weight = static_cast<txt::FontWeight>(style.fontWeight),
        .font_style = static_cast<txt::FontStyle>(style.fontStyle),
        .font_family = style.fontFamily,
        .font_size = style.fontSize,
        .height = style.heightScale,
        .has_height_override = style.heightOnly,
        .strut_enabled = style.useLineStyle,
        .strut_font_weight = static_cast<txt::FontWeight>(style.lineStyleFontWeight),
        .strut_font_style = static_cast<txt::FontStyle>(style.lineStyleFontStyle),
        .strut_font_families = style.lineStyleFontFamilies,
        .strut_font_size = style.lineStyleFontSize,
        .strut_height = style.lineStyleHeightScale,
        .strut_has_height_override = style.lineStyleHeightOnly,
        .strut_leading = style.lineStyleSpacingScale,
        .force_strut_height = style.lineStyleOnly,
        .text_align = static_cast<txt::TextAlign>(style.textAlign),
        .text_direction = static_cast<txt::TextDirection>(style.textDirection),
        .max_lines = style.maxLines,
        .ellipsis = style.ellipsis,
        .locale = style.locale,
        .break_strategy = static_cast<minikin::BreakStrategy>(style.breakStrategy),
    };
}

txt::PlaceholderRun Convert(const PlaceholderSpan &run)
{
    return {
        run.width, run.height,
        static_cast<txt::PlaceholderAlignment>(run.alignment),
        static_cast<txt::TextBaseline>(run.baseline),
        run.baselineOffset,
    };
}

txt::TextStyle Convert(const TextStyle &style)
{
    txt::TextStyle textStyle;
    auto color = SkColorSetARGB(
        style.color.GetAlpha(), style.color.GetRed(), style.color.GetGreen(), style.color.GetBlue());
    textStyle.color = color;
    textStyle.decoration = style.decoration;
    auto decorationColor = SkColorSetARGB(style.decorationColor.GetAlpha(),
        style.decorationColor.GetRed(), style.decorationColor.GetGreen(), style.decorationColor.GetBlue());
    textStyle.decoration_color = decorationColor;
    textStyle.decoration_style = static_cast<txt::TextDecorationStyle>(style.decorationStyle);
    textStyle.decoration_thickness_multiplier = style.decorationThicknessScale;
    textStyle.font_weight = static_cast<txt::FontWeight>(style.fontWeight);
    textStyle.font_style = static_cast<txt::FontStyle>(style.fontStyle);
    textStyle.text_baseline = static_cast<txt::TextBaseline>(style.baseline);
    textStyle.font_families = style.fontFamilies;
    textStyle.font_size = style.fontSize;
    textStyle.letter_spacing = style.letterSpacing;
    textStyle.word_spacing = style.wordSpacing;
    textStyle.height = style.heightScale;
    textStyle.has_height_override = style.heightOnly;
    textStyle.locale = style.locale;
    textStyle.has_background = style.background.has_value();
    textStyle.background = style.background.value_or(SkPaint());
    textStyle.has_foreground = style.foreground.has_value();
    textStyle.foreground = style.foreground.value_or(SkPaint());

    for (const auto &[color, offset, radius] : style.shadows) {
        auto shadowColor = SkColorSetARGB(color.GetAlpha(), color.GetRed(),
                                          color.GetGreen(), color.GetBlue());
        auto shadowOffset = SkPoint::Make(offset.GetX(), offset.GetY());
        textStyle.text_shadows.emplace_back(shadowColor, shadowOffset, radius);
    }

    for (const auto &[tag, value] : style.fontFeatures.GetFontFeatures()) {
        textStyle.font_features.SetFeature(tag, value);
    }
    return textStyle;
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
