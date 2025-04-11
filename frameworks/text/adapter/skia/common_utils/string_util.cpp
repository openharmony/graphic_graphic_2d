/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "string_util.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

static constexpr size_t SURROGATE_LENGTH = 2;
static constexpr uint16_t LOW_SURROGATE_MIN = 0xDC00;
static constexpr uint16_t LOW_SURROGATE_MAX = 0xDFFF;
static constexpr uint16_t HIGHT_SURROGATE_MIN = 0xD800;
static constexpr uint16_t HIGHT_SURROGATE_MAX = 0xDBFF;
static constexpr uint16_t UTF16_REPLACEMENT_CHARACTER = 0xFFFD;

bool Utf16Utils::IsUTF16LowSurrogate(uint16_t ch)
{
    return ch >= LOW_SURROGATE_MIN && ch <= LOW_SURROGATE_MAX;
}

bool Utf16Utils::IsUTF16HighSurrogate(uint16_t ch)
{
    return ch >= HIGHT_SURROGATE_MIN && ch <= HIGHT_SURROGATE_MAX;
}

void Utf16Utils::HandleIncompleteSurrogatePairs(std::u16string& str)
{
    size_t i = 0;
    size_t length = str.size();

    while (i < length) {
        char16_t ch = str[i];
        if (IsUTF16HighSurrogate(ch)) {
            if (i + 1 < length && IsUTF16LowSurrogate(str[i + 1])) {
                i += SURROGATE_LENGTH;
                continue;
            }
            str[i] = UTF16_REPLACEMENT_CHARACTER;
        } else if (IsUTF16LowSurrogate(ch)) {
            if (i == 0 || !IsUTF16HighSurrogate(str[i - 1])) {
                str[i] = UTF16_REPLACEMENT_CHARACTER;
            }
        }

        ++i;
    }
}
}
}
}