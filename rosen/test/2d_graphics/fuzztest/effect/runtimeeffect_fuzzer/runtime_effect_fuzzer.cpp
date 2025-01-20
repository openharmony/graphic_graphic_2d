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

#include "runtime_effect_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"

#include "effect/runtime_effect.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t BOOL_SIZE = 2;
constexpr size_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {
void RuntimeEffectFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_SIZE + 1;
    uint32_t isES3 = GetObject<uint32_t>();
    RuntimeEffectOptions runeo;
    runeo.forceNoInline = GetObject<bool>();
    runeo.useAF = GetObject<bool>();
    char* s1 = new char[count];
    for (size_t i = 0; i < count; i++) {
        s1[i] = GetObject<char>();
    }
    s1[count - 1] = '\0';
    std::shared_ptr<RuntimeEffect> runtimee = RuntimeEffect::CreateForShader(s1, runeo);
    std::shared_ptr<RuntimeEffect> runtimee1 = RuntimeEffect::CreateForShader(s1);
    std::shared_ptr<RuntimeEffect> runtimee2 = RuntimeEffect::CreateForES3Shader(s1);
    std::shared_ptr<RuntimeEffect> runtimee3 = RuntimeEffect::CreateForBlender(s1);
    RuntimeEffect rune = RuntimeEffect(s1);
    RuntimeEffect rune1 = RuntimeEffect(s1, runeo);
    RuntimeEffect rune2 = RuntimeEffect(s1, runeo, static_cast<bool>(isES3 % BOOL_SIZE));
    RuntimeEffect rune3 = RuntimeEffect(s1, static_cast<bool>(isES3 % BOOL_SIZE));
    rune.GetDrawingType();
    if (s1 != nullptr) {
        delete [] s1;
        s1 = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::RuntimeEffectFuzzTest000(data, size);
    return 0;
}
