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

#include <unicode/uchar.h>

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
#define CN_LEFT_QUOTE 0x201C
#define CN_RIGHT_QUOTE 0x201D
#define EN_QUOTE 0x22

void TextBreaker::SetWidthLimit(const double widthLimit)
{
    widthLimit_ = widthLimit;
}

int TextBreaker::WordBreak(std::vector<VariantSpan> &spans, const TypographyStyle &ys,
    const std::shared_ptr<FontProviders> &fontProviders)
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
            currentWidth_ += vspan.GetWidth();
            if (currentWidth_ >= widthLimit_) {
                currentWidth_ = 0;
            }
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

        if (ys.wordBreakType != WordBreakType::NORMAL) {
            GenNewBoundryByWidth(cgs, boundaries);
        }
        GenNewBoundryByHardBreak(cgs, boundaries);
        GenNewBoundryByTypeface(cgs, boundaries);
        GenNewBoundryByQuote(cgs, boundaries);

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
    const TextStyle &xs, const std::shared_ptr<FontProviders> &fontProviders) noexcept(false)
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

void TextBreaker::GenNewBoundryByTypeface(CharGroups cgs, std::vector<Boundary> &boundaries)
{
    std::vector<Boundary> newBoundary;
    for (auto &[start, end] : boundaries) {
        size_t newStart = start;
        size_t newEnd = start;
        const auto &wordCgs = cgs.GetSubFromU16RangeAll(start, end);
        auto typeface = wordCgs.Get(0).typeface;
        for (auto cg = wordCgs.begin(); cg != wordCgs.end(); cg++) {
            if (typeface == cg->typeface) {
                newEnd++;
                continue;
            }

            newBoundary.push_back({newStart, newEnd});
            newStart = newEnd;
            typeface = cg->typeface;
        }

        newBoundary.push_back({newStart, end});
    }

    boundaries = newBoundary;
}

bool TextBreaker::IsQuote(const uint16_t c)
{
    return ((c == EN_QUOTE) || (c == CN_LEFT_QUOTE) || (c == CN_RIGHT_QUOTE));
}

void TextBreaker::GenNewBoundryByQuote(CharGroups cgs, std::vector<Boundary> &boundaries)
{
    std::vector<Boundary> newBoundary = {{0, 0}};
    auto boundary = boundaries.begin();
    bool isEndQuote = false;
    for (; boundary < boundaries.end() - 1; boundary++) {
        const auto &prevWordCgs = cgs.GetSubFromU16RangeAll(boundary->leftIndex, boundary->rightIndex);
        bool isQuote = IsQuote(prevWordCgs.GetBack().chars[0]);
        if (isQuote && newBoundary.back().rightIndex == boundary->rightIndex) {
            isEndQuote = false;
        }

        if (isQuote && !isEndQuote) {
            newBoundary.push_back({boundary->leftIndex, boundary->rightIndex - 1});
            newBoundary.push_back({boundary->rightIndex -1, (boundary + 1)->rightIndex});
            isEndQuote = true;
            boundary++;
        }

        if (isQuote && isEndQuote) {
            if (newBoundary.back().rightIndex == boundary->rightIndex) {
                isEndQuote = false;
                continue;
            }
            newBoundary.back().rightIndex += boundary->rightIndex;
            isEndQuote = false;
            continue;
        } else {
            newBoundary.push_back({boundary->leftIndex, boundary->rightIndex});
        }
    }

    if (boundary != boundaries.end()) {
        newBoundary.push_back({boundary->leftIndex, boundary->rightIndex});
    }
    newBoundary.erase(newBoundary.begin());
    boundaries = newBoundary;
}

void TextBreaker::GenNewBoundryByWidth(CharGroups cgs, std::vector<Boundary> &boundaries)
{
    std::vector<Boundary> newBoundary;
    for (auto &[start, end] : boundaries) {
        size_t newStart = start;
        size_t newEnd = start;
        const auto &wordCgs = cgs.GetSubFromU16RangeAll(start, end);
        double wordWith = 0;
        for (auto &cg : wordCgs) {
            wordWith += cg.GetWidth();
        }

        if (currentWidth_ + wordWith > widthLimit_) {
            currentWidth_ = 0;
        }

        currentWidth_ += wordCgs.begin()->GetWidth();
        auto prevCg = wordCgs.begin();
        for (auto cg = wordCgs.begin() + 1; cg != wordCgs.end(); cg++) {
            if (currentWidth_ + cg->GetWidth() >= widthLimit_) {
                newEnd += prevCg->chars.size();
                newBoundary.push_back({newStart, newEnd});
                currentWidth_ = cg->GetWidth();
                newStart = newEnd;
            } else {
                newEnd += prevCg->chars.size();
                currentWidth_ += cg->GetWidth();
            }
            prevCg = cg;
        }

        if (newEnd != end) {
            newBoundary.push_back({newStart, end});
        }
    }

    boundaries = newBoundary;
}

void TextBreaker::GenNewBoundryByHardBreak(CharGroups cgs, std::vector<Boundary> &boundaries)
{
    std::vector<Boundary> newBoundary;
    for (auto &[start, end] : boundaries) {
        size_t newStart = start;
        size_t newEnd = start;
        const auto &wordCgs = cgs.GetSubFromU16RangeAll(start, end);
        for (auto cg = wordCgs.begin(); cg != wordCgs.end(); cg++) {
            if (cg->IsHardBreak() && newStart != newEnd) {
                newBoundary.push_back({newStart, newEnd});
            }

            if (cg->IsHardBreak()) {
                newBoundary.push_back({newEnd, newEnd + cg->chars.size()});
                newStart = newEnd + cg->chars.size();
            }

            newEnd += cg->chars.size();
        }

        if (newStart == start) {
            newBoundary.push_back({newStart, end});
        }
    }

    boundaries = newBoundary;
}

void TextBreaker::BreakWord(const CharGroups &wordcgs, const TypographyStyle &ys,
    const TextStyle &xs, std::vector<VariantSpan> &spans)
{
    size_t rangeOffset = 0;
    for (size_t i = 0; i < wordcgs.GetNumberOfCharGroup(); i++) {
        auto &cg = wordcgs.Get(i);
        postBreak_ += cg.GetWidth();
        if (u_isWhitespace(cg.chars[0]) == 0 || cg.IsHardBreak()) {
            // not white space
            preBreak_ = postBreak_;
        }

        LOGEX_FUNC_LINE_DEBUG() << "Now: [" << i << "] '" << TextConverter::ToStr(cg.chars) << "'"
            << " preBreak_: " << preBreak_ << ", postBreak_: " << postBreak_;

        const auto &breakType = ys.wordBreakType == WordBreakType::NORMAL ?
            WordBreakType::BREAK_WORD : ys.wordBreakType;
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
    double spanWidth = 0.0;
    for (const auto &cg : currentCgs) {
        spanWidth += cg.GetWidth();
    }
    newSpan->width_ = spanWidth;
    VariantSpan vs(newSpan);
    vs.SetTextStyle(xs);
    spans.push_back(vs);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
