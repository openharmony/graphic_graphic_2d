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

#include "text_shaper.h"

#include "measurer.h"
#include "texgine_font_manager.h"
#include "texgine_exception.h"
#include "texgine_font.h"
#include "texgine_text_blob_builder.h"
#include "texgine/typography_types.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define DOUBLE 2

void TextShaper::PartFontPropertySet(TexgineFont& font, const CharGroup &cg) const
{
    font.SetEdging(TexgineFont::FontEdging::ANTIALIAS);
    font.SetSubpixel(true);
    font.SetHinting(TexgineFont::TexgineFontHinting::SLIGHT);
    if (cg.typeface->Get()->DetectRawInformation() || cg.typeface->DetectionItalic()) {
        font.SetSkewX();
    }
    if (cg.typeface->DetectionFakeBold()) {
        font.SetBold();
    }
    font.SetTypeface(cg.typeface->Get());
}

int TextShaper::FilterTextSpan(std::shared_ptr<TextSpan> ts)
{
    if (!ts->cgs_.IsValid()) {
        LOGEX_FUNC_LINE(ERROR) << "cgs is inValid";
        return 1;
    }

    if (ts->cgs_.GetSize() <= 0) {
        LOGEX_FUNC_LINE(ERROR) << "cgs have no cg";
        return 1;
    }

    if (ts->cgs_.Get(0).typeface == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "first cgs have no typeface";
        return 1;
    }

    return 0; // Shape successed
}

int TextShaper::Shape(const VariantSpan &span, const TypographyStyle &ys,
    const std::shared_ptr<FontProviders> &fontProviders)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("TextShaper::ShapeLineSpans");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "TextShaper::ShapeLineSpans");
    if (span == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "span is nullptr";
        return 1; // Shape failed
    }

    if (span.TryToAnySpan() != nullptr) {
        return 0; // Shape successed
    }

    auto xs = span.GetTextStyle();
    std::shared_ptr<TextSpan> ts = span.TryToTextSpan();
    if (ts->cgs_.JudgeOnlyHardBreak()) {
        xs = ys.ConvertToTextStyle();
    }

    auto ret = DoShape(ts, xs, ys, fontProviders);
    if (ret) {
        LOGEX_FUNC_LINE(ERROR) << "DoShape failed";
        return 1;
    }

    if (FilterTextSpan(ts)) {
        return 1; // Shape failed
    }

    TexgineFont font;
    font.SetSize(xs.fontSize);
    font.GetMetrics(ts->tmetrics_);

    auto blob = GenerateTextBlob(font, ts->cgs_, ts->width_, ts->glyphWidths_);
    if (blob == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "Generate text blob is failed";
        return 1;
    }

    ts->textBlob_ = blob;
    return 0;
}

int TextShaper::DoShape(std::shared_ptr<TextSpan> &span, const TextStyle &xs,
    const TypographyStyle &ys, const std::shared_ptr<FontProviders> &fontProviders)
{
    if (fontProviders == nullptr || span == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "providers or span is nullptr";
        return 1;
    }

    auto families = xs.fontFamilies;
    if (families.empty()) {
        families = ys.fontFamilies;
    }

    auto fontCollection = fontProviders->GenerateFontCollection(families);
    if (fontCollection == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "fontCollection is nullptr";
        return 1;
    }

    std::vector<uint16_t> u16vect = span->cgs_.ToUTF16();
    span->u16vect_ = u16vect;
    auto measurer = Measurer::Create(span->u16vect_, *fontCollection);
    if (measurer == nullptr) {
        return 1;
    }
    measurer->SetLocale(xs.locale);
    measurer->SetRTL(span->rtl_);
    FontStyles style(xs.fontWeight, xs.fontStyle);
    measurer->SetFontStyle(style);
    measurer->SetSize(xs.fontSize);
    measurer->SetFontFeatures(xs.fontFeature);
    measurer->SetRange(0, span->u16vect_.size());
    measurer->SetSpacing(xs.letterSpacing, xs.wordSpacing);
    if (measurer->Measure(span->cgs_)) {
        LOGEX_FUNC_LINE(ERROR) << "Measurer::Measure failed";
        return 1;
    }

    return 0;
}

std::shared_ptr<TexgineTextBlob> TextShaper::GenerateTextBlob(TexgineFont &font, const CharGroups &cgs,
    double &spanWidth, std::vector<double> &glyphWidths) const
{
    TexgineTextBlobBuilder builder;
    auto offset = 0.0;
    for (const auto &cg : cgs) {
        PartFontPropertySet(font, cg);

        glyphWidths.push_back(cg.GetWidth());
        auto drawingOffset = offset;
        offset += cg.GetWidth();

        const auto& runBuffer = builder.AllocRunPos(font, cg.glyphs.size());
        auto index = 0;
        for (const auto &[cp, ax, ay, ox, oy] : cg.glyphs) {
            runBuffer.glyphs[index] = cp;
            runBuffer.pos[index * DOUBLE] = drawingOffset + ox;
            runBuffer.pos[index * DOUBLE + 1] = ay - oy;
            index++;
            drawingOffset += ax;
        }
    }

    spanWidth = offset;
    return builder.Make();
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
