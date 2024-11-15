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

#include "rsparticlenoise_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "animation/rs_render_particle.h"
#include "animation/rs_particle_noise_field.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoCalculateFeatherEffect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int r = GetData<int>();
    ShapeType type = ShapeType::CIRCLE;
    float b = GetData<float>();
    float a = GetData<float>();
    Vector2f fiedSize(a, b);
    Vector2f center(b, a);
    uint16_t feather = GetData<uint16_t>();
    float scale = GetData<float>();
    float frequency = GetData<float>();
    float amplitude = GetData<float>();
    ParticleNoiseField noiseField(r, type, fiedSize, center, feather, scale, frequency, amplitude);
    float x = GetData<float>();
    float y = GetData<float>();
    noiseField.CalculateFeatherEffect(x, y);
    return true;
}

bool DoApplyField(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int r = GetData<int>();
    ShapeType type = ShapeType::CIRCLE;
    float b = GetData<float>();
    float a = GetData<float>();
    Vector2f fiedSize(a, b);
    Vector2f center(b, a);
    uint16_t feather = GetData<uint16_t>();
    float scale = GetData<float>();
    float frequency = GetData<float>();
    float amplitude = GetData<float>();
    ParticleNoiseField noiseField(r, type, fiedSize, center, feather, scale, frequency, amplitude);
    float c = GetData<float>();
    float d = GetData<float>();
    Vector2f position(c, d);
    float x = GetData<float>();
    noiseField.ApplyField(position, x);
    return true;
}

bool DoApplyCurlNoise(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int r = GetData<int>();
    ShapeType type = ShapeType::CIRCLE;
    float b = GetData<float>();
    float a = GetData<float>();
    Vector2f fiedSize(a, b);
    Vector2f center(b, a);
    uint16_t feather = GetData<uint16_t>();
    float scale = GetData<float>();
    float frequency = GetData<float>();
    float amplitude = GetData<float>();
    ParticleNoiseField noiseField(r, type, fiedSize, center, feather, scale, frequency, amplitude);
    float c = GetData<float>();
    float d = GetData<float>();
    Vector2f position(c, d);
    noiseField.ApplyCurlNoise(position);
    return true;
}

bool DoPerlinNoise2D(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float a = GetData<float>();
    float b = GetData<float>();
    float c = GetData<float>();
    float d = GetData<float>();
    PerlinNoise2D noise2D(a, b, c);
    float e = GetData<float>();
    float f = GetData<float>();
    float g = GetData<float>();
    noise2D.Noise(d, e);
    noise2D.Curl(f, g);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCalculateFeatherEffect(data, size);
    OHOS::Rosen::DoApplyField(data, size);
    OHOS::Rosen::DoApplyCurlNoise(data, size);
    OHOS::Rosen::DoPerlinNoise2D(data, size);
    return 0;
}
