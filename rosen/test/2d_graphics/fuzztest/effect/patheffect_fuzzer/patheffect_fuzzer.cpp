/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "patheffect_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "effect/path_effect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t MAX_ARRAY_SIZE = 5000;
} // namespace
bool PathEffectFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar radius = GetObject<scalar>();
    PathEffect::CreateCornerPathEffect(radius);
    Path path;
    scalar advance = GetObject<scalar>();
    scalar phase = GetObject<scalar>();
    uint32_t style = GetObject<uint32_t>();
    PathEffect::CreatePathDashEffect(path, advance, phase, static_cast<PathDashStyle>(style));
    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    float* intervals = new float[count];
    if (intervals == nullptr) {
        return false;
    }
    for (size_t i = 0; i < count; i++) {
        intervals[i] = GetObject<float>();
    }
    PathEffect::CreateDashPathEffect(intervals, count, phase);
    PathEffect::CreateCornerPathEffect(GetObject<scalar>());
    scalar segLength = GetObject<scalar>();
    scalar dev = GetObject<scalar>();
    uint32_t seedAssist = GetObject<uint32_t>();
    PathEffect::CreateDiscretePathEffect(segLength, dev, seedAssist);
    PathEffect::PathEffectType type = GetObject<PathEffect::PathEffectType>();
    PathEffect pathEffect = PathEffect(type);
    PathEffect pathEffectTWO = PathEffect(type);
    PathEffect::CreateSumPathEffect(pathEffect, pathEffectTWO);
    PathEffect::CreateComposePathEffect(pathEffect, pathEffectTWO);
    pathEffect.GetType();
    pathEffect.GetDrawingType();
    PathEffect(GetObject<PathEffect::PathEffectType>(), intervals, count, phase);
    style = GetObject<uint32_t>();
    PathEffect(GetObject<PathEffect::PathEffectType>(), path, GetObject<scalar>(), GetObject<scalar>(),
        static_cast<PathDashStyle>(style));
    PathEffect(GetObject<PathEffect::PathEffectType>(), GetObject<scalar>());
    PathEffect(GetObject<PathEffect::PathEffectType>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<uint32_t>());
    PathEffect(GetObject<PathEffect::PathEffectType>(), pathEffect, pathEffectTWO);
    if (intervals != nullptr) {
        delete [] intervals;
        intervals = nullptr;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PathEffectFuzzTest(data, size);
    return 0;
}
