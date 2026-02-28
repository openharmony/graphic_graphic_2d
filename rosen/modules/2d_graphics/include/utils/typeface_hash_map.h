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

#ifndef TYPEFACE_HASH_MAP_H
#define TYPEFACE_HASH_MAP_H

#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <tuple>

#include "text/typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API TypefaceHashMap {
public:
    static TypefaceHashMap& GetInstance();

    static std::shared_ptr<Drawing::Typeface> GetTypefaceByFullHash(uint64_t fullHash);
    static void InsertTypefaceByFullHash(uint64_t fullHash, const std::shared_ptr<Drawing::Typeface>& typeface);
    static void Release(uint64_t fullHash);

private:
    TypefaceHashMap() = default;
    TypefaceHashMap(const TypefaceHashMap&) = delete;
    const TypefaceHashMap& operator=(const TypefaceHashMap&) = delete;
    static std::shared_ptr<Drawing::Typeface> GetTypefaceByFullHashInner(uint64_t fullHash);

    std::unordered_map<uint64_t, std::tuple<std::weak_ptr<Drawing::Typeface>, uint32_t>> typefaceMap_;
    std::shared_mutex mutex_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
