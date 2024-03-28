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
#include "get_object.h"
#include "effect/runtime_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool RuntimeEffectFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RuntimeEffect> runtimeEffect = RuntimeEffect::CreateForShader("shader");
    runtimeEffect->GetDrawingType();
    std::shared_ptr<Data> uniforms = std::make_shared<Data>();
    ColorQuad color = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(color);
    std::shared_ptr<ShaderEffect> children[] = {shaderEffect};
    Matrix localMatrix = GetObject<Matrix>();
    bool isOpaque = GetObject<bool>();
    runtimeEffect->MakeShader(uniforms, children, 1, &localMatrix, isOpaque);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::RuntimeEffectFuzzTest(data, size);
    return 0;
}
