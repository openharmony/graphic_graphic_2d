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

#include "line_breaker.h"

#include "char_groups.h"
#include "floating.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_merger.h"
#include "text_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::vector<LineMetrics> LineBreaker::BreakLines(std::vector<VariantSpan> &spans,
    const TypographyStyle &tstyle, const double widthLimit)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "BreakLines");
    auto ss = GenerateScoreSpans(spans);
    DoBreakLines(ss, widthLimit, tstyle);
    auto lineBreaks = GenerateBreaks(ss);
    return GenerateLineMetrics(spans, lineBreaks);
}

std::vector<struct ScoredSpan> LineBreaker::GenerateScoreSpans(const std::vector<VariantSpan> &spans)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "GenerateScoreSpans");
    std::vector<struct ScoredSpan> scoredSpans = {{}};
    double offset = 0;
    double preBreak = 0.0;
    double postBreak = 0.0;
    CharGroups lastcgs;
    for (const auto &span : spans) {
        if (span == nullptr) {
            throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
        }

        if (auto ts = span.TryToTextSpan(); ts != nullptr) {
            if (lastcgs.IsSameCharGroups(ts->cgs_) == false) {
                // not same word break
                offset = scoredSpans.back().postBreak;
                lastcgs = ts->cgs_;
            }
            preBreak = ts->GetPreBreak();
            postBreak = ts->GetPostBreak();
        }

        if (auto as = span.TryToAnySpan(); as != nullptr) {
            offset = scoredSpans.back().postBreak;
            preBreak = as->GetWidth();
            postBreak = preBreak;
            // prevent after as is a SameCharGroups ts with before this as
            lastcgs = {};
        }

        LOGEX_FUNC_LINE_DEBUG() << "[" << scoredSpans.size() << "]"
            << ": offset: " << offset
            << ", preBreak: " << preBreak
            << ", postBreak: " << postBreak;
        scoredSpans.push_back({
            .span = span,
            .preBreak = offset + preBreak,
            .postBreak = offset + postBreak,
            .score = 0,
            .prev = 0,
        });
    }
    scoredSpans.erase(scoredSpans.begin());
    return scoredSpans;
}

void LineBreaker::DoBreakLines(std::vector<struct ScoredSpan> &scoredSpans, const double widthLimit,
    const TypographyStyle &tstyle)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "UpadateLineBreaksData");
    scoredSpans.emplace(scoredSpans.cbegin());
    for (size_t i = 1; i < scoredSpans.size(); i++) {
        auto &is = scoredSpans[i];
        is.prev = scoredSpans[i - 1].prev;
        LOGEX_FUNC_LINE_DEBUG() << "[" << i << "]: is.preBreak: " << is.preBreak
            << ", prev.postBreak: " << scoredSpans[is.prev].postBreak;
        if (FLOATING_GT(is.preBreak - scoredSpans[is.prev].postBreak, widthLimit)) {
            is.prev = static_cast<int>(i - 1);
            LOGEX_FUNC_LINE_DEBUG() << "  -> [" << is.prev
                << "]: prev.postBreak: " << scoredSpans[is.prev].postBreak;
        }

        if (tstyle.breakStrategy == BreakStrategy::GREEDY) {
            continue;
        }

        LOGSCOPED(sl1, LOGEX_FUNC_LINE_DEBUG(), "algo");
        double delta = widthLimit - (is.preBreak - scoredSpans[is.prev].postBreak);
        is.score = delta * delta + scoredSpans[is.prev].score;

        std::stringstream ss;
        ss << i;
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());
        for (size_t j = 0; j < i; j++) {
            const auto &js = scoredSpans[j];
            double jdelta = widthLimit - (is.preBreak - js.postBreak);
            if (jdelta < 0) {
                continue;
            }

            double jscore = js.score + jdelta * jdelta;
            LOGEX_FUNC_LINE_DEBUG() << "[" << j << "]"
                << " s(" << jscore << ")" << " = js(" << js.score << ")"
                << " + dd(" << jdelta * jdelta << ")" << " (jdelta: " << jdelta << ")";

            if (jscore < is.score) {
                is.score = jscore;
                is.prev = static_cast<int>(j);
            }
        }
        LOGEX_FUNC_LINE_DEBUG() << "[" << i << "] Any{" << is.prev << "<-" << " b(" << is.preBreak << ", "
                      << is.postBreak << ")" << " s(" << is.score << ")}";
    }
    scoredSpans.erase(scoredSpans.begin());
}

std::vector<int32_t> LineBreaker::GenerateBreaks(const std::vector<struct ScoredSpan> &scoredSpans)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "GenerateBreaks");

    std::vector<int32_t> lineBreaks;
    for (int i = static_cast<int>(scoredSpans.size()); i > 0; i = scoredSpans[i - 1].prev) {
        if (scoredSpans[i - 1].prev >= i) {
            throw TEXGINE_EXCEPTION(ERROR_STATUS);
        }

        LOGEX_FUNC_LINE_DEBUG() << "break at " << i;
        lineBreaks.push_back(i);
    }
    std::reverse(lineBreaks.begin(), lineBreaks.end());
    return lineBreaks;
}

std::vector<LineMetrics> LineBreaker::GenerateLineMetrics(std::vector<VariantSpan> &spans,
    std::vector<int32_t> &breaks)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "GenerateLineMetrics");
    LOGEX_FUNC_LINE_DEBUG() << "breaks.size(): " << breaks.size();

    std::vector<LineMetrics> lineMetrics;
    auto prev = 0;
    if (!breaks.empty() && breaks.back() > static_cast<int>(spans.size())) {
        throw TEXGINE_EXCEPTION(OUT_OF_RANGE);
    }

    for (const auto &lb : breaks) {
        if (lb <= prev) {
            throw TEXGINE_EXCEPTION(ERROR_STATUS);
        }

        std::stringstream ss;
        ss << "[" << prev << ", " << lb << ")";
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());

        std::vector<VariantSpan> vss;
        for (int32_t i = prev; i < lb; i++) {
            spans[i].Dump();
            vss.push_back(spans[i]);
        }

        lineMetrics.push_back({
            .lineSpans = vss,
        });
        prev = lb;
    }

    return lineMetrics;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
