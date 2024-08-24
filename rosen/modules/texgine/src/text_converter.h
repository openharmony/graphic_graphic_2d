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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXT_CONVERTER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXT_CONVERTER_H
#include <string>
#include <vector>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
using UTF8String = std::vector<uint8_t>;
using UTF16String = std::vector<uint16_t>;
using UTF32String = std::vector<uint32_t>;

class TextConverter {
public:
    static UTF8String  ToUTF8(const UTF16String &utf16Text);
    static std::string ToStr(const UTF16String &utf16Text);
    static UTF16String ToUTF16(const std::string &utf8Text);
    static UTF16String ToUTF16(const UTF8String &utf8Text);
    static UTF16String ToUTF16(const UTF32String &utf32Text);
    static UTF32String ToUTF32(const UTF16String &utf16Text);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXT_CONVERTER_H
