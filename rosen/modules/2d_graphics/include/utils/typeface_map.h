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

#ifndef TYPEFACE_MAP_H
#define TYPEFACE_MAP_H

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "text/typeface.h"

namespace OHOS::Rosen::Drawing {
class DRAWING_API TypefaceMap {
public:
    static TypefaceMap& GetInstance();
    static std::shared_ptr<Typeface> GetTypefaceByHash(uint32_t hash);
    static std::shared_ptr<Typeface> GetTypefaceByUniqueId(uint32_t id);
    static void InsertTypeface(uint32_t hash, const std::shared_ptr<Typeface>& typeface);

private:
    TypefaceMap() = default;
    TypefaceMap(const TypefaceMap&) = delete;
    const TypefaceMap& operator=(const TypefaceMap&) = delete;
    static std::shared_ptr<Typeface> GetTypefaceByHashInner(uint32_t hash);

    std::unordered_map<uint32_t, uint32_t> uniqueToHash_;
    std::unordered_map<uint32_t, std::weak_ptr<Typeface>> typefaceMap_;
    std::shared_mutex mutex_;
};
} // namespace OHOS::Rosen::Drawing
#endif