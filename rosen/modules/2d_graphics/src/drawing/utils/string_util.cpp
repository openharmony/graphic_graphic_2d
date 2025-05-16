/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "utils/string_util.h"

#include <cstdint>
#include <string>

#include "unicode/unistr.h"

namespace OHOS {
namespace Rosen {
bool IsUtf8(const char* text, int len)
{
    int n;
    int i = 0;
    while (i < len) {
        uint32_t c = text[i];
        if (c <= 0x7F) { // 0x00 and 0x7F is the range of utf-8
            n = 0;
        } else if ((c & 0xE0) == 0xC0) { // 0xE0 and 0xC0 is the range of utf-8
            n = 1;
        } else if (c == 0xED && i < (len - 1) && (text[i + 1] & 0xA0) == 0xA0) { // 0xA0 and 0xED is the range of utf-8
            return false;
        } else if ((c & 0xF0) == 0xE0) { // 0xE0 and 0xF0 is the range of utf-8
            n = 2;                       // 2 means the size of range
        } else if ((c & 0xF8) == 0xF0) { // 0xF0 and 0xF8 is the range of utf-8
            n = 3;                       // 3 means the size of range
        } else {
            return false;
        }

        for (int j = 0; j < n && i < len; j++) {
            // 0x80 and 0xC0 is the range of utf-8
            if ((++i == len) || ((text[i] & 0xC0) != 0x80)) {
                return false;
            }
        }
        i++; // move to the next character
    }
    return true;
}

// Illegal bytes are replaced with U+FFFD
std::u16string Str8ToStr16ByIcu(const std::string& str)
{
    if (str.empty()) {
        return u"";
    }
    icu::UnicodeString uString = icu::UnicodeString::fromUTF8(str);
    return std::u16string(uString.getBuffer(), static_cast<size_t>(uString.length()));
}

// Illegal bytes are replaced with U+FFFD
std::u16string Str32ToStr16ByIcu(const int32_t* data, size_t len)
{
    icu::UnicodeString uString = icu::UnicodeString::fromUTF32(data, len);
    return std::u16string(uString.getBuffer(), static_cast<size_t>(uString.length()));
}
} // namespace Rosen
} // namespace OHOS
