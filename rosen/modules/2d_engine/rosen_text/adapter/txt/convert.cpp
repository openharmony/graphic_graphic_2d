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
        .font_weight = static_cast<txt::FontWeight>(style.fontWeight_),
        .font_style = static_cast<txt::FontStyle>(style.fontStyle_),
        .font_family = style.fontFamily_,
        .font_size = style.fontSize_,
        .height = style.heightScale_,
        .has_height_override = style.heightOnly_,
        .strut_enabled = style.useLineStyle_,
        .strut_font_weight = static_cast<txt::FontWeight>(style.lineStyleFontWeight_),
        .strut_font_style = static_cast<txt::FontStyle>(style.lineStyleFontStyle_),
        .strut_font_families = style.lineStyleFontFamilies_,
        .strut_font_size = style.lineStyleFontSize_,
        .strut_height = style.lineStyleHeightScale_,
        .strut_has_height_override = style.lineStyleHeightOnly_,
        .strut_leading = style.lineStyleSpacingScale_,
        .force_strut_height = style.lineStyleOnly_,
        .text_align = static_cast<txt::TextAlign>(style.textAlign_),
        .text_direction = static_cast<txt::TextDirection>(style.textDirection_),
        .max_lines = style.maxLines_,
        .ellipsis = style.ellipsis_,
        .locale = style.locale_,
        .break_strategy = static_cast<minikin::BreakStrategy>(style.breakStrategy_),
        .word_break_type = static_cast<minikin::WordBreakType>(style.wordBreakType_),
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
    auto color = SkColorSetARGB(style.color_.GetAlpha(),
                                style.color_.GetRed(),
                                style.color_.GetGreen(),
                                style.color_.GetBlue());
    textStyle.color = color;
    textStyle.decoration = style.decoration_;
    auto decorationColor = SkColorSetARGB(style.decorationColor_.GetAlpha(),
                                          style.decorationColor_.GetRed(),
                                          style.decorationColor_.GetGreen(),
                                          style.decorationColor_.GetBlue());
    textStyle.decoration_color = decorationColor;
    textStyle.decoration_style = static_cast<txt::TextDecorationStyle>(style.decorationStyle_);
    textStyle.decoration_thickness_multiplier = style.decorationThicknessScale_;
    textStyle.font_weight = static_cast<txt::FontWeight>(style.fontWeight_);
    textStyle.font_style = static_cast<txt::FontStyle>(style.fontStyle_);
    textStyle.text_baseline = static_cast<txt::TextBaseline>(style.baseline_);
    textStyle.font_families = style.fontFamilies_;
    textStyle.font_size = style.fontSize_;
    textStyle.letter_spacing = style.letterSpacing_;
    textStyle.word_spacing = style.wordSpacing_;
    textStyle.height = style.heightScale_;
    textStyle.has_height_override = style.heightOnly_;
    textStyle.locale = style.locale_;
    textStyle.has_background = style.background_.has_value();
    textStyle.background = style.background_.value_or(SkPaint());
    textStyle.has_foreground = style.foreground_.has_value();
    textStyle.foreground = style.foreground_.value_or(SkPaint());

    for (const auto &[color, offset, radius] : style.shadows_) {
        auto shadowColor = SkColorSetARGB(color.GetAlpha(), color.GetRed(),
                                          color.GetGreen(), color.GetBlue());
        auto shadowOffset = SkPoint::Make(offset.GetX(), offset.GetY());
        textStyle.text_shadows.emplace_back(shadowColor, shadowOffset, radius);
    }

    for (const auto &[tag, value] : style.fontFeatures_.GetFontFeatures()) {
        textStyle.font_features.SetFeature(tag, value);
    }
    return textStyle;
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
