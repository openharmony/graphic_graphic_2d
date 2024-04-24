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

#include "text_reverser.h"

#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
void TextReverser::ReverseRTLText(std::vector<VariantSpan> &lineSpans)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("Shaper::ReverseRTLText");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "ReverseRTLText");
    const auto &endit = lineSpans.end();
    auto rtlSpansBeginIt = endit;
    auto rtlSpansEndIt = endit;
    bool rtl = false;
    for (auto it = lineSpans.begin(); it != lineSpans.end(); it++) {
        auto ts = it->TryToTextSpan();
        if (ts != nullptr) {
            rtl = ts->IsRTL();
        }

        if (rtl) {
            rtlSpansBeginIt = rtlSpansBeginIt == endit ? it : rtlSpansBeginIt;
            rtlSpansEndIt = it;
            LOGCEX_DEBUG() << "is rtl";
            continue;
        } else {
            LOGCEX_DEBUG() << "no rtl";
        }

        if (rtlSpansBeginIt == endit) {
            continue;
        }

        rtlSpansEndIt++;
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "reverse");
        for (auto i = rtlSpansBeginIt; i != rtlSpansEndIt; i++) {
            i->Dump();
        }
        std::reverse(rtlSpansBeginIt, rtlSpansEndIt);
        rtlSpansBeginIt = endit;
        rtlSpansEndIt = endit;
    }

    if (rtlSpansBeginIt != endit) {
        rtlSpansEndIt++;
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "reverse");
        for (auto it = rtlSpansBeginIt; it != rtlSpansEndIt; it++) {
            it->Dump();
        }
        std::reverse(rtlSpansBeginIt, rtlSpansEndIt);
    }
}

void TextReverser::ReverseConDirectionText(std::vector<VariantSpan> &lineSpans, int begin, int end)
{
    while (begin < end) {
        if (lineSpans[begin].GetVisibleWidth() == 0) {
            begin++;
        }

        if (lineSpans[end].GetVisibleWidth() == 0) {
            end--;
        }

        if (begin >= end) {
            break;
        }

        auto temp = lineSpans[begin];
        lineSpans[begin] = lineSpans[end];
        lineSpans[end] = temp;
        begin++;
        end--;
    }
}

void TextReverser::ProcessTypoDirection(std::vector<VariantSpan> &lineSpans, const TextDirection dir)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("ProcessTypoDirection");
#endif
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "ProcessTypoDirection");
    if (dir == TextDirection::LTR) {
        return;
    }

    bool isConDirection = false;
    int index = 0;
    ReverseConDirectionText(lineSpans, 0, static_cast<int>(lineSpans.size()) - 1);

    for (auto i = 0; i < static_cast<int>(lineSpans.size()) - 1; i++) {
        if (lineSpans[i].GetVisibleWidth() == 0 || lineSpans[i + 1].GetVisibleWidth() == 0 ||
            lineSpans[i].IsRTL() == lineSpans[i + 1].IsRTL()) {
            isConDirection = true;
            continue;
        }

        if (isConDirection == true) {
            ReverseConDirectionText(lineSpans, index, i);
            index = i + 1;
            isConDirection = false;
        } else {
            index++;
        }
    }

    if (isConDirection) {
        ReverseConDirectionText(lineSpans, index, static_cast<int>(lineSpans.size()) - 1);
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
