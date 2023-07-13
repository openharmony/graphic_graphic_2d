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

#include "text_breaker.h"

#include <cassert>

#include "measurer.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "text_span.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_converter.h"
#include "word_breaker.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
int TextBreaker::WordBreak(std::vector<VariantSpan> &spans, const TypographyStyle &ys,
    const std::unique_ptr<FontProviders> &fontProviders)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("TextBreaker::WordBreak");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "WordBreak");
    std::vector<VariantSpan> visitingSpans;
    std::swap(visitingSpans, spans);
    for (const auto &vspan : visitingSpans) {
        auto span = vspan.TryToTextSpan();
        if (span == nullptr) {
            spans.push_back(vspan);
            continue;
        }

        auto xs = vspan.GetTextStyle();
        auto fontCollection = GenerateFontCollection(ys, xs, fontProviders);
        if (fontCollection == nullptr) {
            // WordBreak failed
            return 1;
        }

        CharGroups cgs;
        std::vector<Boundary> boundaries;
        auto ret = Measure(xs, span->u16vect_, *fontCollection, cgs, boundaries);
        if (ret) {
            return 1;
        }

        LOGSCOPED(sl2, LOGEX_FUNC_LINE_DEBUG(), "TextBreaker::doWordBreak algo");
        preBreak_ = 0;
        postBreak_ = 0;
        for (auto &[start, end] : boundaries) {
            const auto &wordcgs = cgs.GetSubFromU16RangeAll(start, end);
            std::stringstream ss;
            ss << "u16range: [" << start << ", " << end << "), range: " << wordcgs.GetRange();
            LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());
            BreakWord(wordcgs, ys, xs, spans);
        }
    }
    // WordBreak successed
    return 0;
}

std::shared_ptr<FontCollection> TextBreaker::GenerateFontCollection(const TypographyStyle &ys,
    const TextStyle &xs, const std::unique_ptr<FontProviders> &fontProviders) noexcept(false)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "TextBreaker::GenerateFontCollection");
    auto families = xs.fontFamilies;
    if (families.empty()) {
        families = ys.fontFamilies;
    }

    if (fontProviders == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "fontProviders is nullptr";
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    return fontProviders->GenerateFontCollection(families);
}

int TextBreaker::Measure(const TextStyle &xs, const std::vector<uint16_t> &u16vect,
    const FontCollection &fontCollection, CharGroups &cgs, std::vector<Boundary> &boundaries) noexcept(false)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "TextBreaker::doMeasure");
    auto measurer = Measurer::Create(u16vect, fontCollection);
    if (measurer == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "Measurer::Create return nullptr";
        throw TEXGINE_EXCEPTION(API_FAILED);
    }

    measurer->SetLocale(xs.locale);
    measurer->SetRTL(false);
    measurer->SetSize(xs.fontSize);
    FontStyles style(xs.fontWeight, xs.fontStyle);
    measurer->SetFontStyle(style);
    measurer->SetRange(0, u16vect.size());
    measurer->SetSpacing(xs.letterSpacing, xs.wordSpacing);
    auto ret = measurer->Measure(cgs);
    if (ret != 0) {
        LOGEX_FUNC_LINE(ERROR) << "Measure failed!";
        return ret;
    }
    boundaries = measurer->GetWordBoundary();
    if (boundaries.size() == 0) {
        LOGEX_FUNC_LINE(ERROR) << "Measurer GetWordBoundary failed!";
        return 1;
    }
    return 0;
}

void TextBreaker::BreakWord(const CharGroups &wordcgs, const TypographyStyle &ys,
    const TextStyle &xs, std::vector<VariantSpan> &spans)
{
    size_t rangeOffset = 0;
    for (size_t i = 0; i < wordcgs.GetNumberOfCharGroup(); i++) {
        const auto &cg = wordcgs.Get(i);
        postBreak_ += cg.GetWidth();
        if (u_isWhitespace(cg.chars[0]) == 0) {
            // not white space
            preBreak_ = postBreak_;
        }

        LOGEX_FUNC_LINE_DEBUG() << "Now: [" << i << "] '" << TextConverter::ToStr(cg.chars) << "'"
            << " preBreak_: " << preBreak_ << ", postBreak_: " << postBreak_;

        const auto &breakType = ys.wordBreakType;
        bool isBreakAll = (breakType == WordBreakType::BREAK_ALL);
        bool isBreakWord = (breakType == WordBreakType::BREAK_WORD);
        bool isFinalCharGroup = (i == wordcgs.GetNumberOfCharGroup() - 1);
        bool needGenerateSpan = isBreakAll;
        needGenerateSpan = needGenerateSpan || (isBreakWord && isFinalCharGroup);
        if (needGenerateSpan == false) {
            continue;
        }

        auto currentCgs = wordcgs.GetSub(rangeOffset, i + 1);
        GenerateSpan(currentCgs, ys, xs, spans);
        rangeOffset = i + 1;
    }
}

void TextBreaker::GenerateSpan(const CharGroups &currentCgs, const TypographyStyle &ys,
    const TextStyle &xs, std::vector<VariantSpan> &spans)
{
    if (!currentCgs.IsValid() || currentCgs.GetSize() == 0) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    LOGEX_FUNC_LINE_DEBUG(Logger::SetToNoReturn) << "AddWord " << spans.size()
        << " " << currentCgs.GetRange() << ": \033[40m'";
    LOGCEX_DEBUG() << TextConverter::ToStr(currentCgs.ToUTF16()) << "'\033[0m";
    auto newSpan = std::make_shared<TextSpan>();
    newSpan->cgs_ = currentCgs;
    newSpan->postBreak_ = postBreak_;
    newSpan->preBreak_ = preBreak_;
    newSpan->typeface_ = currentCgs.Get(0).typeface;

    for ([[maybe_unused]] const auto &cg : currentCgs) {
        assert(cg.typeface == newSpan->typeface_);
    }
    VariantSpan vs(newSpan);
    vs.SetTextStyle(xs);
    spans.push_back(vs);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
