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

#include "rsanimationratedecider_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "animation/rs_animation_rate_decider.h"
#include "modifier/rs_render_property.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    float width = GetData<float>();
    float height = GetData<float>();
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    uint64_t id = GetData<uint64_t>();
    int min = GetData<int>();
    int max = GetData<int>();
    int preferred = GetData<int>();
    PropertyValue velocity;
    FrameRateRange range(min, max, preferred);
    auto frameRateGetFunc =
        [](const RSPropertyUnit unit, float velocity, int32_t area, int32_t length) -> int32_t { return 0; };
    RSAnimationRateDecider rsAnimationRateDecider;
    rsAnimationRateDecider.SetEnable(true);
    rsAnimationRateDecider.SetNodeSize(width, height);
    rsAnimationRateDecider.SetNodeScale(scaleX, scaleY);
    rsAnimationRateDecider.Reset();
    rsAnimationRateDecider.AddDecisionElement(id, velocity, range);
    rsAnimationRateDecider.MakeDecision(frameRateGetFunc);
    rsAnimationRateDecider.GetFrameRateRange();
    PropertyValue propertyVector4F = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(
        Vector4f(), 1, RSPropertyUnit::PIXEL_POSITION);
    rsAnimationRateDecider.CalculatePreferredRate(propertyVector4F, frameRateGetFunc);
    PropertyValue propertyVector2F = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        Vector2f(), 1, RSPropertyUnit::PIXEL_POSITION);
    rsAnimationRateDecider.CalculatePreferredRate(propertyVector2F, frameRateGetFunc);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
