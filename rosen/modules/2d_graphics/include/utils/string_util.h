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

#ifndef GRAPHIC_GRAPHIC_2D_STRING_UTIL_H
#define GRAPHIC_GRAPHIC_2D_STRING_UTIL_H

#include <string>

namespace OHOS {
namespace Rosen {

bool IsUtf8(const char* text, int len);

std::u16string Str8ToStr16ByIcu(const std::string& str);
std::u16string Str32ToStr16ByIcu(const int32_t* data, size_t len);
}
} // namespace OHOS

#endif // GRAPHIC_GRAPHIC_2D_STRING_UTIL_H
