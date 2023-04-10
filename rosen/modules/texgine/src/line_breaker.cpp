/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "texgine/utils/trace.h"
#include "text_merger.h"
#include "text_span.h"

namespace Texgine {
std::vector<LineMetrics> LineBreaker::BreakLines(std::vector<VariantSpan> &spans,
                                                 const TypographyStyle &tstyle,
                                                 const double widthLimit)
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "BreakLines");
    auto ss = GenerateScoreSpans(spans);
    DoBreakLines(ss, widthLimit, tstyle);
    auto lineBreaks = GenerateBreaks(ss);
    return GenerateLineMetrics(spans, lineBreaks);
}

std::vector<struct ScoredSpan> LineBreaker::GenerateScoreSpans(std::vector<VariantSpan> &spans)
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "GenerateScoreSpans");
    std::vector<struct ScoredSpan> scoredSpans = {{}};
    double offset = 0;
    double preBreak = 0.0;
    double postBreak = 0.0;
    CharGroups lastcgs;
    for (const auto &span : spans) {
        if (span == nullptr) {
            throw TEXGINE_EXCEPTION(InvalidArgument);
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

        LOG2EX_DEBUG() << "[" << scoredSpans.size() << "]"
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

void LineBreaker::DoBreakLines(std::vector<struct ScoredSpan> &scoredSpans,
                               const double widthLimit,
                               const TypographyStyle &ys)
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "UpadateLineBreaksData");
    scoredSpans.emplace(scoredSpans.cbegin());
    for (size_t i = 1; i < scoredSpans.size(); i++) {
        auto &is = scoredSpans[i];
        is.prev = scoredSpans[i - 1].prev;
        LOG2EX_DEBUG() << "[" << i << "]: is.preBreak: " << is.preBreak
            << ", prev.postBreak: " << scoredSpans[is.prev].postBreak;
        if (FLOATING_GT(is.preBreak - scoredSpans[is.prev].postBreak, widthLimit)) {
            is.prev = i - 1;
            LOG2EX_DEBUG() << "  -> [" << is.prev
                << "]: prev.postBreak: " << scoredSpans[is.prev].postBreak;
        }

        if (ys.breakStrategy_ == BreakStrategy::GREEDY) {
            continue;
        }

        LOGSCOPED(sl1, LOG2EX_DEBUG(), "algo");
        double delta = widthLimit - (is.preBreak - scoredSpans[is.prev].postBreak);
        is.score = delta * delta + scoredSpans[is.prev].score;

        std::stringstream ss;
        ss << i;
        LOGSCOPED(sl, LOG2EX_DEBUG(), ss.str());
        for (size_t j = 0; j < i; j++) {
            auto &js = scoredSpans[j];
            double jdelta = widthLimit - (is.preBreak - js.postBreak);
            if (jdelta < 0) {
                continue;
            }

            double jscore = js.score + jdelta * jdelta;
            LOG2EX_DEBUG() << "[" << j << "]"
                << " s(" << jscore << ")" << " = js(" << js.score << ")"
                << " + dd(" << jdelta * jdelta << ")" << " (jdelta: " << jdelta << ")";

            if (jscore < is.score) {
                is.score = jscore;
                is.prev = j;
            }
        }
        LOG2EX_DEBUG() << "[" << i << "] Any{" << is.prev << "<-" << " b(" << is.preBreak << ", "
                      << is.postBreak << ")" << " s(" << is.score << ")}";
    }
    scoredSpans.erase(scoredSpans.begin());
}

std::vector<int32_t> LineBreaker::GenerateBreaks(std::vector<struct ScoredSpan> &scoredSpans)
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "GenerateBreaks");

    std::vector<int32_t> lineBreaks;
    for (int i = scoredSpans.size(); i > 0; i = scoredSpans[i - 1].prev) {
        if (scoredSpans[i - 1].prev >= i) {
            throw TEXGINE_EXCEPTION(ErrorStatus);
        }

        LOG2EX_DEBUG() << "break at " << i;
        lineBreaks.push_back(i);
    }
    std::reverse(lineBreaks.begin(), lineBreaks.end());
    return lineBreaks;
}

std::vector<LineMetrics> LineBreaker::GenerateLineMetrics(std::vector<VariantSpan> &spans,
                                                          std::vector<int32_t> &breaks)
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "GenerateLineMetrics");
    LOG2EX_DEBUG() << "breaks.size(): " << breaks.size();

    std::vector<LineMetrics> lineMetrics;
    auto prev = 0;
    if (!breaks.empty() && breaks.back() > spans.size()) {
        throw TEXGINE_EXCEPTION(OutOfRange);
    }

    for (const auto &lb : breaks) {
        if (lb <= prev) {
            throw TEXGINE_EXCEPTION(ErrorStatus);
        }

        std::stringstream ss;
        ss << "[" << prev << ", " << lb << ")";
        LOGSCOPED(sl, LOG2EX_DEBUG(), ss.str());

        std::vector<VariantSpan> vss;
        for (int32_t i = prev; i < lb; i++) {
            spans[i].Dump();
            vss.push_back(spans[i]);
        }

        lineMetrics.push_back({
            .lineSpans_ = vss,
        });
        prev = lb;
    }

    return lineMetrics;
}
} // namespace Texgine
