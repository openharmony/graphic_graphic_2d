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

#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <vector>
#include <cstdint>
#include <cstddef>
#include "text/font_types.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

size_t GetStrLength(const void* text, size_t byteLength, TextEncoding encoding);

bool DecodeTextToCodepoints(const void* text, size_t byteLength, TextEncoding encoding, std::vector<int32_t>& out);

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // TEXT_UTILS_H