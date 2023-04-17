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
        .fontWeight_ = Convert(style.fontWeight_),
        .fontStyle_ = Convert(style.fontStyle_),
        .fontFamilies_ = { style.fontFamily_ },
        .fontSize_ = style.fontSize_,
        .heightScale_ = style.heightScale_,
        .heightOnly_ = style.heightOnly_,
        .locale_ = style.locale_,
        .maxLines_ = style.maxLines_,
        .ellipsis_ = style.ellipsis_,
        .breakStrategy_ = Convert(style.breakStrategy_),
        .wordBreakType_ = Convert(style.wordBreakType_),
        .align_ = Convert(style.textAlign_),
        .direction_ = Convert(style.textDirection_),
        .useLineStyle_ = style.useLineStyle_,
        .lineStyle_ = {
            .only_ = style.lineStyleOnly_,
            .fontWeight_ = Convert(style.lineStyleFontWeight_),
            .fontStyle_ = Convert(style.lineStyleFontStyle_),
            .fontFamilies_ = style.lineStyleFontFamilies_,
            .heightOnly_ = style.lineStyleHeightOnly_,
            .fontSize_ = style.lineStyleFontSize_,
            .heightScale_ = style.lineStyleHeightScale_,
        },
    };
    if (style.lineStyleSpacingScale_ >= 0) {
        ys.lineStyle_.spacingScale_ = style.lineStyleSpacingScale_;
    }
    return ys;
}

TextEngine::TextStyle Convert(const TextStyle &style)
{
    auto color = SkColorSetARGB(style.color_.GetAlpha(),
                                style.color_.GetRed(),
                                style.color_.GetGreen(),
                                style.color_.GetBlue());
    auto decorationColor = SkColorSetARGB(style.decorationColor_.GetAlpha(),
                                          style.decorationColor_.GetRed(),
                                          style.decorationColor_.GetGreen(),
                                          style.decorationColor_.GetBlue());
    auto foreground = std::make_shared<TextEngine::TexginePaint>();
    foreground->SetPaint(*style.foreground_);
    auto background = std::make_shared<TextEngine::TexginePaint>();
    background->SetPaint(*style.background_);
    TextEngine::TextStyle xs = {
        .fontWeight_ = Convert(style.fontWeight_),
        .fontStyle_ = Convert(style.fontStyle_),
        .fontFamilies_ = style.fontFamilies_,
        .fontSize_ = style.fontSize_,
        .decoration_ = Convert(style.decoration_),
        .decorationColor_ = decorationColor,
        .decorationStyle_ = Convert(style.decorationStyle_),
        .decorationThicknessScale_ = style.decorationThicknessScale_,
        .color_ = color,
        .baseline_ = Convert(style.baseline_),
        .locale_ = style.locale_,
        .heightOnly_ = style.heightOnly_,
        .heightScale_ = style.heightScale_,
        .letterSpacing_ = style.letterSpacing_,
        .wordSpacing_ = style.wordSpacing_,
        .foreground_ = *foreground,
        .background_ = *background,
    };

    for (const auto &[tag, value] : style.fontFeatures_.GetFontFeatures()) {
        xs.fontFeature_.SetFeature(tag, value);
    }

    for (const auto &[color, offset, radius] : style.shadows_) {
        // 24, 16, 8, 0: How many bits are moved to the right
        auto shadowColor = (color.GetAlpha() << 24) | (color.GetRed() << 16) |
            (color.GetGreen() << 8) | (color.GetBlue() << 0);
        TextEngine::TextShadow shadow = {
            .offsetX_ = offset.GetX(),
            .offsetY_ = offset.GetY(),
            .color_ = shadowColor,
            .blurLeave_ = radius,
        };
        xs.shadows_.emplace_back(shadow);
    }
    return xs;
}

IndexAndAffinity Convert(const TextEngine::IndexAndAffinity &pos)
{
    return { pos.index_, Convert(pos.affinity_) };
}

Boundary Convert(const TextEngine::Boundary &range)
{
    return { range.leftIndex_, range.rightIndex_ };
}

TextRect Convert(const TextEngine::TextRect &box)
{
    Drawing::RectF rect(*box.rect_.fLeft_, *box.rect_.fTop_, *box.rect_.fRight_, *box.rect_.fBottom_);
    return { rect, Convert(box.direction_) };
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
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS
