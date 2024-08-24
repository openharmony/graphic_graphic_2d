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

#include "text_converter.h"

#include <unicode/utf8.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
UTF8String TextConverter::ToUTF8(const UTF16String &utf16Text)
{
    size_t utf16Index = 0;
    size_t codePoint = 0;
    UTF8String utf8Text;
    while (utf16Index < utf16Text.size()) {
        U16_NEXT(utf16Text.data(), utf16Index, utf16Text.size(), codePoint);
        size_t utf8Index = 0;
        int error = 0;
        const int size = 8;
        uint8_t utf8Str[size] = {};
        U8_APPEND(utf8Str, utf8Index, sizeof(utf8Str), codePoint, error);
        if (utf8Index >= size) {
            return utf8Text;
        }

        for (int i = 0; i < static_cast<int>(utf8Index); i++) {
            utf8Text.push_back(utf8Str[i]);
        }
    }
    utf8Text.push_back(0);
    return utf8Text;
}

std::string TextConverter::ToStr(const UTF16String &utf16Text)
{
    return reinterpret_cast<char *>(ToUTF8(utf16Text).data());
}

UTF16String TextConverter::ToUTF16(const std::string &utf8Text)
{
    size_t utf8Index = 0;
    size_t codePoint = 0;
    UTF16String utf16Text;
    while (utf8Index < utf8Text.size()) {
        U8_NEXT(utf8Text.c_str(), utf8Index, utf8Text.size(), codePoint);
        if (U16_LENGTH(codePoint) == 1) {
            utf16Text.push_back(codePoint);
        } else {
            utf16Text.push_back(U16_LEAD(codePoint));
            utf16Text.push_back(U16_TRAIL(codePoint));
        }
    }
    return utf16Text;
}

UTF16String TextConverter::ToUTF16(const UTF8String &utf8Text)
{
    size_t utf8Index = 0;
    size_t codePoint = 0;
    UTF16String utf16Text;
    while (utf8Index < utf8Text.size()) {
        U8_NEXT(utf8Text.data(), utf8Index, utf8Text.size(), codePoint);
        if (U16_LENGTH(codePoint) == 1) {
            utf16Text.push_back(codePoint);
        } else {
            utf16Text.push_back(U16_LEAD(codePoint));
            utf16Text.push_back(U16_TRAIL(codePoint));
        }
    }
    return utf16Text;
}

UTF16String TextConverter::ToUTF16(const UTF32String &utf32Text)
{
    size_t utf32Index = 0;
    size_t codePoint = 0;
    int error = 0;
    UTF16String utf16Text;
    while (utf32Index < utf32Text.size()) {
        UTF32_NEXT_CHAR_SAFE(utf32Text.data(), utf32Index, utf32Text.size(), codePoint, error);
        if (U16_LENGTH(codePoint) == 1) {
            utf16Text.push_back(codePoint);
        } else {
            utf16Text.push_back(U16_LEAD(codePoint));
            utf16Text.push_back(U16_TRAIL(codePoint));
        }
    }
    return utf16Text;
}

UTF32String TextConverter::ToUTF32(const UTF16String &utf16Text)
{
    size_t utf16Index = 0;
    size_t codePoint = 0;
    UTF32String utf32Text;
    while (utf16Index < utf16Text.size()) {
        U16_NEXT(utf16Text.data(), utf16Index, utf16Text.size(), codePoint);
        utf32Text.push_back(codePoint);
    }
    return utf32Text;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
