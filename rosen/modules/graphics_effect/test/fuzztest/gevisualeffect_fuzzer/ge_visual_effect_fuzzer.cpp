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

#include <cstring>
#include "ge_visual_effect_fuzzer.h"
#include "get_object.h"
#include "ge_render.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
namespace Drawing {

namespace {
constexpr size_t STR_LEN = 10;
}

std::shared_ptr<GEVisualEffectImpl> GEVisualEffectFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    int32_t nameLen = GETest::GetPlainData<int32_t>();
    std::string name = GETest::GetStringFromData(nameLen);
    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
    geVisualEffect->GetName();
    auto impl = geVisualEffect->GetImpl();
    return impl;
}

std::shared_ptr<GEVisualEffect> GEVisualEffectFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    int32_t nameLen = GETest::GetPlainData<int32_t>();
    std::string name = GETest::GetStringFromData(nameLen);
    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
    return geVisualEffect;
}

bool GEVisualEffectFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    FuzzedDataProvider fdp(data, size);
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test");
    std::string tag = fdp.ConsumeRandomLengthString(STR_LEN);
    int32_t param = fdp.ConsumeIntegral<int32_t>();
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    int64_t param = GETest::GetPlainData<int64_t>();
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    float param = GETest::GetPlainData<float>();
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    double param = GETest::GetPlainData<double>();
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    std::string param_str = GETest::GetStringFromData(STR_LEN);
    geVisualEffect->SetParam(tag, param_str.c_str());
    return true;
}

bool GEVisualEffectFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    Matrix param;
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    float f1 = GETest::GetPlainData<float>();
    float f2 = GETest::GetPlainData<float>();
    std::vector<std::pair<float, float>> param {{f1, f2}};
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    bool param = GETest::GetPlainData<bool>();
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest011(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    uint32_t param = GETest::GetPlainData<uint32_t>();
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest012(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    std::shared_ptr<Drawing::Image> param;
    geVisualEffect->SetParam(tag, param);
    return true;
}

bool GEVisualEffectFuzzTest013(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>("test", type);
    std::string tag = GETest::GetStringFromData(STR_LEN);
    std::shared_ptr<Drawing::ColorFilter> param;
    geVisualEffect->SetParam(tag, param);
    return true;
}

std::string GEVisualEffectFuzzTest014(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    // initialize
    GETest::g_data = data;
    GETest::g_size = size;
    GETest::g_pos = 0;

    int32_t nameLen = GETest::GetPlainData<int32_t>();
    std::string name = GETest::GetStringFromData(nameLen);
    DrawingPaintType type = GETest::GetPlainData<DrawingPaintType>();
    auto geVisualEffect = std::make_shared<GEVisualEffect>(name, type);
    std::string res = geVisualEffect->GetName();
    return res;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest001(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest002(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest003(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest004(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest005(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest006(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest007(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest008(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest009(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest010(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest011(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest012(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest013(data, size);
    OHOS::Rosen::Drawing::GEVisualEffectFuzzTest014(data, size);
    return 0;
}
