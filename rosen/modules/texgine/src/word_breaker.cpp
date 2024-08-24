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

#include "word_breaker.h"

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define LIMIT_DATA 10000
void WordBreaker::SetLocale(const icu::Locale &locale)
{
    locale_ = locale;
}

void WordBreaker::SetRange(size_t start, size_t end)
{
    startIndex_ = start;
    endIndex_ = end;
}

std::vector<Boundary> WordBreaker::GetBoundary(const std::vector<uint16_t> &u16str, bool isWordInstance)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::BreakIterator *wbi = nullptr;
    if (isWordInstance) {
        wbi = icu::BreakIterator::createWordInstance(locale_, status);
    } else {
        wbi = icu::BreakIterator::createLineInstance(locale_, status);
    }

    if (wbi == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "create BreakIterator failed";
        return {};
    }

    // > U_ZERO_ERROR: error, < U_ZERO_ERROR: warning
    if (status > U_ZERO_ERROR) {
        LOGEX_FUNC_LINE(ERROR) << "status is error";
        return {};
    }

    if (endIndex_ <= startIndex_) {
        LOGEX_FUNC_LINE(ERROR) << "endIndex_ <= startIndex_";
        return {};
    }

    if (endIndex_ > u16str.size()) {
        LOGEX_FUNC_LINE(ERROR) << "endIndex_ > u16str.size()";
        return {};
    }

    auto u16Data = u16str.data();
    const icu::UnicodeString ustr = {u16Data + startIndex_, static_cast<int32_t>(endIndex_ - startIndex_)};
    wbi->setText(ustr);

    std::vector<Boundary> boundaries;
    if (u16str.size() > LIMIT_DATA) {
        boundaries.emplace_back(wbi->first(), u16str.size());
        delete wbi;
        wbi = nullptr;
        return boundaries;
    }
    auto beg = wbi->first();
    for (auto end = wbi->next(); end != wbi->DONE; end = wbi->next()) {
        boundaries.emplace_back(beg, end);
        beg = end;
    }

    delete wbi;
    wbi = nullptr;
    return boundaries;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
