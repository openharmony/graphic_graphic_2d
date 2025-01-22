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

#ifndef SPTEXT_STRING_UTIL_H
#define SPTEXT_STRING_UTIL_H

#include <string>

namespace OHOS {
namespace Rosen {
namespace SPText {

class Utf16Utils {
public:
    static bool IsUTF16LowSurrogate(uint16_t ch);
    static bool IsUTF16HighSurrogate(uint16_t ch);
    static void HandleIncompleteSurrogatePairs(std::u16string& str);
};

} // namespace SPText
} // namespace Rosen
} // namespace OHOS
#endif // SPTEXT_STRING_UTIL_H