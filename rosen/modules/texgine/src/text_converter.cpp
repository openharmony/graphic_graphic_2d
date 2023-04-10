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

#include "text_converter.h"

#include <unicode/utf8.h>

namespace Texgine {
UTF8String TextConverter::ToUTF8(const UTF16String &u16text)
{
    int32_t u16index = 0;
    int codepoint = 0;
    UTF8String u8text;
    const int32_t length = u16text.size();
    while (u16index < length) {
        U16_NEXT(u16text.data(), u16index, length, codepoint);
        size_t u8index = 0;
        int error = 0;
        uint8_t u8str[8] = {};
        U8_APPEND(u8str, u8index, sizeof(u8str), codepoint, error);
        for (int i = 0; i < u8index; i++) {
            u8text.push_back(u8str[i]);
        }
    }
    u8text.push_back(0);
    return u8text;
}

std::string TextConverter::ToStr(const UTF16String &u16text)
{
    return reinterpret_cast<char *>(ToUTF8(u16text).data());
}

UTF16String TextConverter::ToUTF16(const std::string &u8text)
{
    int32_t u8index = 0;
    uint32_t codepoint = 0;
    UTF16String u16text;
    const int32_t length = u8text.size();
    while (u8index < length) {
        U8_NEXT(u8text.c_str(), u8index, length, codepoint);
        if (1 == U16_LENGTH(codepoint)) {
            u16text.push_back(codepoint);
        } else {
            u16text.push_back(U16_LEAD(codepoint));
            u16text.push_back(U16_TRAIL(codepoint));
        }
    }
    return u16text;
}

UTF16String TextConverter::ToUTF16(const UTF8String &u8text)
{
    int32_t u8index = 0;
    uint32_t codepoint = 0;
    UTF16String u16text;
    const int32_t length = u8text.size();
    while (u8index < length) {
        U8_NEXT(u8text.data(), u8index, length, codepoint);
        if (1 == U16_LENGTH(codepoint)) {
            u16text.push_back(codepoint);
        } else {
            u16text.push_back(U16_LEAD(codepoint));
            u16text.push_back(U16_TRAIL(codepoint));
        }
    }
    return u16text;
}

UTF16String TextConverter::ToUTF16(const UTF32String &u32text)
{
    int32_t u32index = 0;
    int32_t codepoint = 0;
    int error = 0;
    UTF16String u16text;
    const int32_t length = u32text.size();
    while (u32index < length) {
        UTF32_NEXT_CHAR_SAFE(u32text.data(), u32index, length, codepoint, error);
        if (1 == U16_LENGTH(codepoint)) {
            u16text.push_back(codepoint);
        }
        else {
            u16text.push_back(U16_LEAD(codepoint));
            u16text.push_back(U16_TRAIL(codepoint));
        }
    }
    return u16text;
}

UTF32String TextConverter::ToUTF32(const UTF16String &u16text)
{
    int32_t u16index = 0;
    int64_t codepoint = 0;
    UTF32String u32text;
    const int32_t length = u16text.size();
    while (u16index < length) {
        U16_NEXT(u16text.data(), u16index, length, codepoint);
        u32text.push_back(codepoint);
    }
    return u32text;
}
} // namespace Texgine
