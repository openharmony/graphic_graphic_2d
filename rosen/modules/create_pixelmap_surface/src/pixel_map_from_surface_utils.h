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

#ifndef PIXEL_MAP_FROM_SURFACE_UTILS_H
#define PIXEL_MAP_FROM_SURFACE_UTILS_H

#include <cstdint>

namespace OHOS {
namespace Rosen {
template<typename RectType>
inline bool IsSrcRectValid(int32_t bufferWidth, int32_t bufferHeight, const RectType& srcRect)
{
    if (bufferWidth <= 0 || bufferHeight <= 0 || srcRect.left < 0 || srcRect.top < 0 ||
        srcRect.width <= 0 || srcRect.height <= 0) {
        return false;
    }
    const int64_t right = static_cast<int64_t>(srcRect.left) + static_cast<int64_t>(srcRect.width);
    const int64_t bottom = static_cast<int64_t>(srcRect.top) + static_cast<int64_t>(srcRect.height);
    return right <= static_cast<int64_t>(bufferWidth) && bottom <= static_cast<int64_t>(bufferHeight);
}
} // namespace Rosen
} // namespace OHOS

#endif // PIXEL_MAP_FROM_SURFACE_UTILS_H
