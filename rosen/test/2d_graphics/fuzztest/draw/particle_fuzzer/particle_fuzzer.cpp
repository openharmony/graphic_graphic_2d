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

#include <fuzzer/FuzzedDataProvider.h>

#include "effect/particle_builder.h"
#include "effect/particle_effect.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr size_t MAX_SIZE = 5000;
constexpr size_t FILTERMODE_SIZE = 2;
const uint8_t DO_PARTICLE_TEST_001 = 0;
const uint8_t DO_PARTICLE_TEST_002 = 1;
const uint8_t DO_PARTICLE_TEST_003 = 2;
const uint8_t DO_PARTICLE_TEST_004 = 3;
const uint8_t DO_PARTICLE_TEST_005 = 4;
const uint8_t DO_PARTICLE_TEST_006 = 5;
const uint8_t DO_PARTICLE_TEST_007 = 6;
const uint8_t TARGET_SIZE = 7;
} // namespace

namespace Drawing {

void DoParticleTest001(FuzzedDataProvider& fdp)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return;
    }
    size_t length = fdp.ConsumeIntegral<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = fdp.ConsumeIntegral<char>();
    }
    dataText[length - 1] = '\0';
    builder->AddConfigData(std::string(dataText), std::string(dataText), fdp.ConsumeIntegral<uint32_t>());
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
}

void DoParticleTest002(FuzzedDataProvider& fdp)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return;
    }
    Bitmap bitmap;
    int width = fdp.ConsumeIntegral<int>() % MAX_SIZE;
    int height = fdp.ConsumeIntegral<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return;
    }
    Image image;
    bool retTwo = image.BuildFromBitmap(bitmap);
    if (!retTwo) {
        return;
    }
    uint32_t fm = fdp.ConsumeIntegral<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(fm % FILTERMODE_SIZE));
    size_t length = fdp.ConsumeIntegral<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = fdp.ConsumeIntegral<char>();
    }
    dataText[length - 1] = '\0';
    builder->AddConfigImage(std::string(dataText), image, samplingOptions);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
}

void DoParticleTest003(FuzzedDataProvider& fdp)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return;
    }
    size_t length = fdp.ConsumeIntegral<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = fdp.ConsumeIntegral<char>();
    }
    dataText[length - 1] = '\0';
    builder->SetUpdateCode(std::string(dataText));
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
}

void DoParticleTest004(FuzzedDataProvider& fdp)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return;
    }
    builder->MakeParticleEffect(fdp.ConsumeIntegral<uint32_t>());
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

void DoParticleTest005(FuzzedDataProvider& fdp)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return;
    }
    auto sysConfig = GetParticleSysConfigTest();
    builder->AddConfigData(sysConfig.name, sysConfig.str, sysConfig.configSize);

    builder->SetUpdateCode(mainFunc);
    uint32_t maxParticleCount = 10;
    auto effect = builder->MakeParticleEffect(maxParticleCount);
    if (!effect) {
        std::cout << "DoParticleTest005 failed, effect faild." << std::endl;
        return;
    }
    ParticleSystemConfigTest sysData;
    sysData.maxParticleCount = fdp.ConsumeIntegral<uint32_t>();
    sysData.emitRate = fdp.ConsumeIntegral<uint32_t>();
    sysData.radius = fdp.ConsumeFloatingPoint<float>();
    sysData.position[0] = fdp.ConsumeFloatingPoint<float>();
    sysData.position[1] = fdp.ConsumeFloatingPoint<float>();
    sysData.emitSize[0] = fdp.ConsumeFloatingPoint<float>();
    sysData.emitSize[1] = fdp.ConsumeFloatingPoint<float>();
    sysData.lifeTime[0] = fdp.ConsumeFloatingPoint<float>();
    sysData.lifeTime[1] = fdp.ConsumeFloatingPoint<float>();
    sysData.deltaT = fdp.ConsumeFloatingPoint<float>();
    effect->UpdateConfigData(
        sysConfig.name, &sysData, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>());
}

void DoParticleTest006(FuzzedDataProvider& fdp)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return;
    }
    Bitmap bitmap;
    int width = fdp.ConsumeIntegral<int>() % MAX_SIZE;
    int height = fdp.ConsumeIntegral<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return;
    }
    Image image;
    bool retTwo = image.BuildFromBitmap(bitmap);
    if (!retTwo) {
        return;
    }
    uint32_t fm = fdp.ConsumeIntegral<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(fm % FILTERMODE_SIZE));
    size_t length = fdp.ConsumeIntegral<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = fdp.ConsumeIntegral<char>();
    }
    dataText[length - 1] = '\0';
    builder->AddConfigImage(std::string(dataText), image, samplingOptions);
    builder->SetUpdateCode(mainFunc);
    uint32_t maxParticleCount = 10;
    auto effect = builder->MakeParticleEffect(maxParticleCount);
    if (!effect) {
        std::cout << "DoParticleTest006 failed, effect faild." << std::endl;
        if (dataText != nullptr) {
            delete [] dataText;
            dataText = nullptr;
        }
        return;
    }
    std::shared_ptr<Image> imagePtr = std::make_shared<Image>(image);
    effect->UpdateConfigImage(std::string(dataText), imagePtr);

    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
}

void DoParticleTest007(FuzzedDataProvider& fdp)
{
    auto effect = std::make_shared<Drawing::ParticleEffect>();

    auto dataVal = std::make_shared<Data>();
    size_t length = fdp.ConsumeIntegral<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = fdp.ConsumeIntegral<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    effect->Deserialize(dataVal);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_PARTICLE_TEST_001:
            OHOS::Rosen::Drawing::DoParticleTest001(fdp);
            break;
        case OHOS::Rosen::DO_PARTICLE_TEST_002:
            OHOS::Rosen::Drawing::DoParticleTest002(fdp);
            break;
        case OHOS::Rosen::DO_PARTICLE_TEST_003:
            OHOS::Rosen::Drawing::DoParticleTest003(fdp);
            break;
        case OHOS::Rosen::DO_PARTICLE_TEST_004:
            OHOS::Rosen::Drawing::DoParticleTest004(fdp);
            break;
        case OHOS::Rosen::DO_PARTICLE_TEST_005:
            OHOS::Rosen::Drawing::DoParticleTest005(fdp);
            break;
        case OHOS::Rosen::DO_PARTICLE_TEST_006:
            OHOS::Rosen::Drawing::DoParticleTest006(fdp);
            break;
        case OHOS::Rosen::DO_PARTICLE_TEST_007:
            OHOS::Rosen::Drawing::DoParticleTest007(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}