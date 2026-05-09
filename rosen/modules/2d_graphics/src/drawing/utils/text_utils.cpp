/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "utils/text_utils.h"
#include <string>

#ifdef USE_M133_SKIA
#include "src/base/SkUTF.h"
#else
#include "src/utils/SkUTF.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr size_t UTF16_UNIT_SIZE = 2;
constexpr size_t UTF32_UNIT_SIZE = 4;
size_t GetStrLength(const void* text, size_t byteLength, TextEncoding encoding)
{
    switch (encoding) {
        case TextEncoding::GLYPH_ID:
            return byteLength;
        case TextEncoding::UTF8:
            return std::char_traits<char>::length(static_cast<const char*>(text))  * sizeof(char);
        case TextEncoding::UTF16:
            return std::char_traits<char16_t>::length(static_cast<const char16_t*>(text)) * sizeof(char16_t);
        case TextEncoding::UTF32:
            return std::char_traits<char32_t>::length(static_cast<const char32_t*>(text)) * sizeof(char32_t);
        default:
            return 0;
    }
}

bool DecodeUTF8ToCodepoints(const char* data, size_t byteLength, std::vector<int32_t>& out)
{
    const char* ptr = data;
    const char* end = data + byteLength;
    while (ptr < end) {
        SkUnichar cp = SkUTF::NextUTF8(&ptr, end);
        out.push_back(static_cast<int32_t>(cp));
    }
    return !out.empty();
}

bool DecodeUTF16ToCodepoints(const uint16_t* data, size_t byteLength, std::vector<int32_t>& out)
{
    const uint16_t* ptr = data;
    const uint16_t* end = data + (byteLength / UTF16_UNIT_SIZE);
    while (ptr < end) {
        SkUnichar cp = SkUTF::NextUTF16(&ptr, end);
        out.push_back(static_cast<int32_t>(cp));
    }
    return !out.empty();
}

bool DecodeTextToCodepoints(const void* text, size_t byteLength, TextEncoding encoding, std::vector<int32_t>& out)
{
    if (text == nullptr) {
        return false;
    }
    switch (encoding) {
        case TextEncoding::UTF32: {
            size_t count = byteLength / UTF32_UNIT_SIZE;
            const int32_t* cpData = static_cast<const int32_t*>(text);
            out.assign(cpData, cpData + count);
            return true;
        }
        case TextEncoding::UTF8: {
            return DecodeUTF8ToCodepoints(static_cast<const char*>(text), byteLength, out);
        }
        case TextEncoding::UTF16: {
            if (byteLength % UTF16_UNIT_SIZE != 0) {
                return false;
            }
            return DecodeUTF16ToCodepoints(static_cast<const uint16_t*>(text), byteLength, out);
        }
        default:
            return false;
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS