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

#include "bidi_processer.h"
#include <unicode/ubidi.h>

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::vector<VariantSpan> BidiProcesser::ProcessBidiText(const std::vector<VariantSpan> &spans, const TextDirection dir)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("BidiProcesser::ProcessBidiText");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "ProcessBidiText");
    std::vector<VariantSpan> newSpans;
    for (auto const &span : spans) {
        auto ts = span.TryToTextSpan();
        if (ts == nullptr) {
            newSpans.emplace_back(span);
            continue;
        }

        span.Dump();
        auto nsis = DoBidiProcess(ts->cgs_, dir);
        for (auto const &nsi : nsis) {
            ts->rtl_ = nsi.rtl;
            VariantSpan vs(ts->CloneWithCharGroups(nsi.cgs));
            vs.SetTextStyle(span.GetTextStyle());
            double spanWidth = 0.0;
            for (const auto &cg : nsi.cgs) {
                spanWidth += cg.GetWidth();
            }
            vs.TryToTextSpan()->width_ = spanWidth;
            newSpans.emplace_back(vs);
        }
    }

    return newSpans;
}

std::vector<NewSpanInfo> BidiProcesser::DoBidiProcess(const CharGroups &cgs, const TextDirection dir)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "BidiProcesser::doBidiProcess");
    if (!cgs.IsValid() || cgs.GetSize() == 0) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    auto deleter = [](UBiDi *p) {ubidi_close(p);};
    std::unique_ptr<UBiDi, decltype(deleter)> bidi(ubidi_open(), deleter);
    if (bidi == nullptr) {
        throw APIFailedException("ubidi_open is failed");
    }

    auto u16vect = cgs.ToUTF16All();
    auto status = U_ZERO_ERROR;
    auto level = dir == TextDirection::RTL ? UBIDI_RTL : UBIDI_LTR;
    ubidi_setPara(bidi.get(), reinterpret_cast<UChar *>(u16vect.data()),
        u16vect.size(), level, nullptr, &status);
    if (!U_SUCCESS(status)) {
        throw APIFailedException("ubidi_setPara failed");
    }

    int size = ubidi_countRuns(bidi.get(), &status);
    std::vector<NewSpanInfo> nsis;
    for (auto i = 0; i < size; i++) {
        NewSpanInfo nsi;
        int start = -1;
        int length = -1;
        nsi.rtl = (ubidi_getVisualRun(bidi.get(), i, &start, &length) == UBIDI_RTL);
        if (start < 0 || length < 0) {
            throw APIFailedException("ubidi_getVisualRun is failed");
        }

        auto cc = cgs.GetSubFromU16RangeAll(start, start + length);
        LOGEX_FUNC_LINE_DEBUG(Logger::SetToNoReturn) <<
            "u16[" << start << ", " << start + length << ")" <<
            " is " << (nsi.rtl ? "rtl" : "ltr") << " ";
        if (cgs.IsIntersect(cc) == false) {
            LOGCEX_DEBUG() << "not intersect";
            continue;
        }

        auto ic = cgs.GetIntersect(cc);
        LOGCEX_DEBUG() << "intersect at cgs" << ic.GetRange();
        nsi.cgs = ic;
        nsis.emplace_back(nsi);
    }
    if (dir == TextDirection::RTL) {
        std::reverse(nsis.begin(), nsis.end());
    }
    return nsis;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
