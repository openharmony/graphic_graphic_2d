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
#ifndef GRAPHICS_EFFECT_GE_HASH_H
#define GRAPHICS_EFFECT_GE_HASH_H
#include <cstdint>
#include <memory>

#include "ge_hash_refer.h"
namespace OHOS {
namespace GraphicsEffectEngine {
uint32_t Hash32Next(uint32_t previousHash, const void* data, size_t bytes, uint32_t seed = 0);
uint64_t Hash64Next(uint64_t previousHash, const void* data, size_t bytes, uint64_t seed = 0);
} // namespace GraphicsEffectEngine
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_HASH_H
