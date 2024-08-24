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

#ifndef ROSEN_MODULES_TEXGINE_SRC_WORD_BREAKER_H
#define ROSEN_MODULES_TEXGINE_SRC_WORD_BREAKER_H

#include <memory>
#include <vector>

#include <unicode/brkiter.h>

#include "texgine/typography.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class WordBreaker {
public:
    void SetLocale(const icu::Locale &locale);
    void SetRange(size_t start, size_t end);
    std::vector<Boundary> GetBoundary(const std::vector<uint16_t> &u16str, bool isWordInstance = false);

private:
    icu::Locale locale_ = icu::Locale::getDefault();
    size_t startIndex_ = 0;
    size_t endIndex_ = 0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_WORD_BREAKER_H
