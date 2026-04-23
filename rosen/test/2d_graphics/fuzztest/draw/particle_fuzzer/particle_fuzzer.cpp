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

#include "particle_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/particle_builder.h"
#include "effect/particle_effect.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
constexpr size_t FILTERMODE_SIZE = 2;
} // namespace
namespace Drawing {

bool ParticleFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return false;
    }
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    builder->AddConfigData(std::string(dataText), std::string(dataText), GetObject<uint32_t>());
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }

    return true;
}

bool ParticleFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return false;
    }
    Bitmap bitmap;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    Image image;
    bool retTwo = image.BuildFromBitmap(bitmap);
    if (!retTwo) {
        return false;
    }
    uint32_t fm = GetObject<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(fm % FILTERMODE_SIZE));
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    builder->AddConfigImage(std::string(dataText), image, samplingOptions);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool ParticleFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return false;
    }
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    builder->SetUpdateCode(std::string(dataText));
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool ParticleFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return false;
    }

    builder->MakeParticleEffect(GetObject<uint32_t>());
    return true;
}

struct ConfigInfoTest {
    std::string name;
    std::string str;
    uint32_t configSize = 0;
    uint32_t offset = 0;
};

struct ParticleSystemConfigTest {
    float color[4] = {1, 0, 0, 1};
    float position[2] = { 1000, 1000 };
    float emitSize[2] = { 10, 10 };
    float lifeTime[2] = { 3, 3 };
    int shapeType = 0;
    uint32_t maxParticleCount = 10000;
    uint32_t emitRate = 10000;
    float radius = 5.f;
    float deltaT = 0.016667f;
    float aspect = 1;
    float mode = 3;
    float sizeGrowTime = 0.0;  // Particle growth time (seconds)
    float sizeShrinkTime = 5;  // Particle Shrink Time (seconds)
    float sizeMin = 0.7;       // Minimum particle size ratio
};

ConfigInfoTest GetParticleSysConfigTest()
{
    const std::string sysConf = R"(
        vec4 color;
        vec2 position;
        vec2 emitSize;
        vec2 lifeTime;
        int shapeType; // RECT = 0, CIRCLE, ELLIPSE, ANNULUS
        uint maxParticleCount;
        uint emitRate;
        float radius;
        float deltaT;
	    float aspect; // 横竖屏的形态
	    float mode; // 场的模式
	    // 粒子大小变化参数
        float sizeGrowTime;  // 粒子增长时间（秒）
        float sizeShrinkTime; // 粒子缩小时间（秒）
        float sizeMin;       // 粒子最小大小比例
    )";
    ConfigInfoTest info;
    info.name = "ParticleSystemConfig";
    info.str = sysConf;
    info.offset = 0;
    info.configSize = sizeof(ParticleSystemConfigTest);
    return info;
}

static const std::string mainFunc = R"(
void main() {
    uint index = sk_GlobalInvocationID.x;
    if (index >= ParticleSystemConfig.maxParticleCount) {
        return;
    }
    bool isEmit = false;
    uint emitEnd = EmitInfo.emitStart + EmitInfo.emitCount;
    if (emitEnd <= ParticleSystemConfig.maxParticleCount) {
        isEmit = (index >= EmitInfo.emitStart && index < emitEnd);
    } else {
        isEmit = (index >= EmitInfo.emitStart && index < ParticleSystemConfig.maxParticleCount);
    }
    Particle p = particles.v[index];
    if (isEmit) {
        p.position.x = (ParticleSystemConfig.position.x + ParticleSystemConfig.emitSize.x) / 2.0;
        p.position.y = (ParticleSystemConfig.position.y + ParticleSystemConfig.emitSize.y) / 2.0;
        p.activeTime = (ParticleSystemConfig.lifeTime.x + ParticleSystemConfig.lifeTime.y) / 2.0;
        p.color = ParticleSystemConfig.color;
        p.scale = ParticleSystemConfig.radius;
    } else if (p.activeTime > 0.0) {
        p.position += 50 * ParticleSystemConfig.deltaT;
        p.activeTime -= ParticleSystemConfig.deltaT;
        p.color = vec4(0, 1.0, 0, 1.0);
    }
    particles.v[index] = p;
})";

bool ParticleFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return false;
    }
    auto sysConfig = GetParticleSysConfigTest();
    builder->AddConfigData(sysConfig.name, sysConfig.str, sysConfig.configSize);

    builder->SetUpdateCode(mainFunc);
    uint32_t maxParticleCount = 10;
    auto effect = builder->MakeParticleEffect(maxParticleCount);
    if (!effect) {
        std::cout << "ParticleFuzzTest005 failed, effect faild." << std::endl;
        return false;
    }
    ParticleSystemConfigTest sysData;
    sysData.maxParticleCount = GetObject<uint32_t>();
    sysData.emitRate = GetObject<uint32_t>();
    sysData.radius = GetObject<float>();
    sysData.position[0] = GetObject<float>();
    sysData.position[1] = GetObject<float>();
    sysData.emitSize[0] = GetObject<float>();
    sysData.emitSize[1] = GetObject<float>();
    sysData.lifeTime[0] = GetObject<float>();
    sysData.lifeTime[1] = GetObject<float>();
    sysData.deltaT = GetObject<float>();
    effect->UpdateConfigData(sysConfig.name, &sysData, GetObject<uint32_t>(), GetObject<uint32_t>());

    return true;
}

bool ParticleFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return false;
    }
    Bitmap bitmap;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    Image image;
    bool retTwo = image.BuildFromBitmap(bitmap);
    if (!retTwo) {
        return false;
    }
    uint32_t fm = GetObject<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(fm % FILTERMODE_SIZE));
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    builder->AddConfigImage(std::string(dataText), image, samplingOptions);
    builder->SetUpdateCode(mainFunc);
    uint32_t maxParticleCount = 10;
    auto effect = builder->MakeParticleEffect(maxParticleCount);
    if (!effect) {
        std::cout << "ParticleFuzzTest006 failed, effect faild." << std::endl;
        return false;
    }
    std::shared_ptr<Image> imagePtr = std::make_shared<Image>(image);
    effect->UpdateConfigImage(std::string(dataText), imagePtr);

    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool ParticleFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto effect = std::make_shared<Drawing::ParticleEffect>();

    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    effect->Deserialize(dataVal);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::ParticleFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ParticleFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ParticleFuzzTest003(data, size);
    OHOS::Rosen::Drawing::ParticleFuzzTest004(data, size);
    OHOS::Rosen::Drawing::ParticleFuzzTest005(data, size);
    OHOS::Rosen::Drawing::ParticleFuzzTest006(data, size);
    OHOS::Rosen::Drawing::ParticleFuzzTest007(data, size);
    return 0;
}
