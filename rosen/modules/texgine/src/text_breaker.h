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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXT_BREAKER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXT_BREAKER_H

#include <vector>

#include "variant_span.h"
#include "word_breaker.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TextBreaker {
public:
    int WordBreak(std::vector<VariantSpan> &spans, const TypographyStyle &ys,
        const std::unique_ptr<FontProviders> &fontProviders);

    std::shared_ptr<FontCollection> GenerateFontCollection(const TypographyStyle &ys,
       const TextStyle &xs, const std::unique_ptr<FontProviders> &fontProviders) noexcept(false);

    int Measure(const TextStyle &xs, const std::vector<uint16_t> &u16vect, FontCollection &fontCollection,
        CharGroups &cgs, std::vector<Boundary> &boundaries) noexcept(false);

    void BreakWord(const CharGroups &wordcgs, const TypographyStyle &ys,
        const TextStyle &xs, std::vector<VariantSpan> &spans) noexcept(false);

    void GenerateSpan(const CharGroups &currentCgs, const TypographyStyle &ys,
        const TextStyle &xs, std::vector<VariantSpan> &spans) noexcept(false);

    double preBreak_ = 0;
    double postBreak_ = 0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXT_BREAKER_H
