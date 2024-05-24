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
#include "ge_hash.h"

#include <cstring>

#include "ge_hash_refer.h"

namespace OHOS {
namespace GraphicsEffectEngine {
uint32_t Hash32Next(uint32_t hash, const void* data, size_t bytes, uint32_t seed)
{
    return static_cast<uint32_t>(GEHash(&hash, sizeof(hash), GEHash(data, bytes, seed, SECRET), SECRET));
}
uint64_t Hash64Next(uint64_t hash, const void* data, size_t bytes, uint64_t seed)
{
    return GEHash(&hash, sizeof(hash), GEHash(data, bytes, seed, SECRET), SECRET);
}
} // namespace GraphicsEffectEngine
} // namespace OHOS
