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

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <codecvt>
#include <locale>
#include <memory.h>
#include <securec.h>
#include <string.h>

namespace OHOS {
namespace Rosen {
namespace Drawing {

[[maybe_unused]] static bool IsUtf8(const char* text, uint32_t len)
{
    uint32_t n = 0;
    for (uint32_t i = 0; i < len; i++) {
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
        for (uint32_t j = 0; j < n && i < len; j++) {
            // 0x80 and 0xC0 is the range of utf-8
            i++;
            if ((i == len) || ((text[i] & 0xC0) != 0x80)) {
                return false;
            }
        }
    }
    return true;
}

[[maybe_unused]] static bool ConvertToString(const uint8_t* data, size_t len, std::string& fullNameString)
{
    if (data == nullptr || len == 0) {
        return false;
    }

    size_t utf16Len = len / sizeof(char16_t);
    std::unique_ptr<char16_t[]> utf16Str = std::make_unique<char16_t[]>(utf16Len);

    errno_t ret = memcpy_s(utf16Str.get(), utf16Len * sizeof(char16_t), data, len);
    if (ret != EOK) {
        return false;
    }

    std::u16string utf16String(utf16Str.get(), utf16Len);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    fullNameString = converter.to_bytes(utf16String);

    return true;
}
}
}
}
#endif