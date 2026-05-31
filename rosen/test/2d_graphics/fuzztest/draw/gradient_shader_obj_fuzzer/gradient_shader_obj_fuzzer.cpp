/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gradient_shader_obj_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/linear_gradient_shader_obj.h"
#include "effect/radial_gradient_shader_obj.h"
#include "effect/conical_gradient_shader_obj.h"
#include "effect/sweep_gradient_shader_obj.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#include "utils/matrix.h"
#include "utils/scalar.h"
#include "draw/ui_color.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t MAX_ARRAY_SIZE = 5000;
    constexpr size_t TILEMODE_SIZE = 4;
} // namespace

/*
 * 测试以下 LinearGradientShaderObj 接口：
 * 1. CreateForUnmarshalling()
 * 2. Create(...) with various parameters
 * 3. Marshalling()
 * 4. Unmarshalling()
 * 5. GenerateBaseObject()
 */
bool LinearGradientShaderObjFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // Test CreateForUnmarshalling
    auto shaderObj = LinearGradientShaderObj::CreateForUnmarshalling();
    if (shaderObj == nullptr) {
        return false;
    }

#ifdef ROSEN_OHOS
    // Test Create with fuzzed parameters
    Point startPt(GetObject<float>(), GetObject<float>());
    Point endPt(GetObject<float>(), GetObject<float>());

    // Create color array with fuzzed size
    uint32_t colorCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<UIColor> colors;
    colors.reserve(colorCount);
    for (uint32_t i = 0; i < colorCount && i < 10; i++) { // Limit to 10 colors for safety
        float r = GetObject<float>();
        float g = GetObject<float>();
        float b = GetObject<float>();
        float a = GetObject<float>();
        float h = GetObject<float>();
        colors.emplace_back(r, g, b, a, h);
    }

    // Empty colors test
    if (colors.empty()) {
        auto emptyShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr,
            std::vector<scalar>(), TileMode::CLAMP, nullptr);
        if (emptyShader != nullptr) {
            return false; // Should return nullptr for empty colors
        }
    }

    // Create position array
    uint32_t posCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<scalar> pos;
    pos.reserve(posCount);
    for (uint32_t i = 0; i < posCount && i < 10; i++) {
        pos.push_back(GetObject<scalar>());
    }

    TileMode mode = static_cast<TileMode>(GetObject<uint32_t>() % TILEMODE_SIZE);
    Matrix matrix;
    matrix.SetMatrix(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());

    auto validShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, mode, &matrix);
    if (validShader == nullptr) {
        return false;
    }

    // Test GenerateBaseObject
    auto baseObject = validShader->GenerateBaseObject();
    if (baseObject == nullptr) {
        return false;
    }

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = validShader->Marshalling(parcel);
    if (!marshalResult) {
        return false; // May fail without proper callback setup
    }

    // Test Unmarshalling
    parcel.RewindRead(0);
    parcel.WriteInt32(validShader->GetType());
    parcel.WriteInt32(validShader->GetSubType());
    bool isValid = true;
    auto newShaderObj = LinearGradientShaderObj::CreateForUnmarshalling();
    if (newShaderObj == nullptr) {
        return false;
    }

    bool unmarshalResult = newShaderObj->Unmarshalling(parcel, isValid, 0);
    // Unmarshalling may fail without proper callbacks, but should not crash

    return true;
#else
    return true;
#endif
}

/*
 * 测试以下 RadialGradientShaderObj 接口：
 * 1. CreateForUnmarshalling()
 * 2. Create(...) with various parameters
 * 3. Marshalling()
 * 4. Unmarshalling()
 * 5. GenerateBaseObject()
 */
bool RadialGradientShaderObjFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // Test CreateForUnmarshalling
    auto shaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
    if (shaderObj == nullptr) {
        return false;
    }

#ifdef ROSEN_OHOS
    // Test Create with fuzzed parameters
    Point centerPt(GetObject<float>(), GetObject<float>());
    scalar radius = GetObject<scalar>();

    // Create color array
    uint32_t colorCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<UIColor> colors;
    colors.reserve(colorCount);
    for (uint32_t i = 0; i < colorCount && i < 10; i++) {
        float r = GetObject<float>();
        float g = GetObject<float>();
        float b = GetObject<float>();
        float a = GetObject<float>();
        float h = GetObject<float>();
        colors.emplace_back(r, g, b, a, h);
    }

    // Create position array
    uint32_t posCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<scalar> pos;
    pos.reserve(posCount);
    for (uint32_t i = 0; i < posCount && i < 10; i++) {
        pos.push_back(GetObject<scalar>());
    }

    TileMode mode = static_cast<TileMode>(GetObject<uint32_t>() % TILEMODE_SIZE);
    Matrix matrix;
    matrix.SetMatrix(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());

    auto validShader = RadialGradientShaderObj::Create(centerPt, radius, colors, nullptr, pos, mode, &matrix);
    if (validShader == nullptr && !colors.empty()) {
        return false;
    }

    if (validShader != nullptr) {
        // Test GenerateBaseObject
        auto baseObject = validShader->GenerateBaseObject();
        if (baseObject == nullptr) {
            return false;
        }

        // Test Marshalling
        MessageParcel parcel;
        bool marshalResult = validShader->Marshalling(parcel);
        if (!marshalResult) {
            return false;
        }

        // Test Unmarshalling
        parcel.RewindRead(0);
        parcel.WriteInt32(validShader->GetType());
        parcel.WriteInt32(validShader->GetSubType());
        bool isValid = true;
        auto newShaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
        if (newShaderObj == nullptr) {
            return false;
        }

        newShaderObj->Unmarshalling(parcel, isValid, 0);
    }

    return true;
#else
    return true;
#endif
}

/*
 * 测试以下 ConicalGradientShaderObj 接口：
 * 1. CreateForUnmarshalling()
 * 2. Create(...) with various parameters
 * 3. Marshalling()
 * 4. Unmarshalling()
 * 5. GenerateBaseObject()
 */
bool ConicalGradientShaderObjFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // Test CreateForUnmarshalling
    auto shaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
    if (shaderObj == nullptr) {
        return false;
    }

#ifdef ROSEN_OHOS
    // Test Create with fuzzed parameters
    Point startPt(GetObject<float>(), GetObject<float>());
    scalar startRadius = GetObject<scalar>();
    Point endPt(GetObject<float>(), GetObject<float>());
    scalar endRadius = GetObject<scalar>();

    // Create color array
    uint32_t colorCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<UIColor> colors;
    colors.reserve(colorCount);
    for (uint32_t i = 0; i < colorCount && i < 10; i++) {
        float r = GetObject<float>();
        float g = GetObject<float>();
        float b = GetObject<float>();
        float a = GetObject<float>();
        float h = GetObject<float>();
        colors.emplace_back(r, g, b, a, h);
    }

    // Create position array
    uint32_t posCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<scalar> pos;
    pos.reserve(posCount);
    for (uint32_t i = 0; i < posCount && i < 10; i++) {
        pos.push_back(GetObject<scalar>());
    }

    TileMode mode = static_cast<TileMode>(GetObject<uint32_t>() % TILEMODE_SIZE);
    Matrix matrix;
    matrix.SetMatrix(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());

    auto validShader = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius,
        colors, nullptr, pos, mode, &matrix);
    if (validShader == nullptr && !colors.empty()) {
        return false;
    }

    if (validShader != nullptr) {
        // Test GenerateBaseObject
        auto baseObject = validShader->GenerateBaseObject();
        if (baseObject == nullptr) {
            return false;
        }

        // Test Marshalling
        MessageParcel parcel;
        bool marshalResult = validShader->Marshalling(parcel);
        if (!marshalResult) {
            return false;
        }

        // Test Unmarshalling
        parcel.RewindRead(0);
        parcel.WriteInt32(validShader->GetType());
        parcel.WriteInt32(validShader->GetSubType());
        bool isValid = true;
        auto newShaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
        if (newShaderObj == nullptr) {
            return false;
        }

        newShaderObj->Unmarshalling(parcel, isValid, 0);
    }

    return true;
#else
    return true;
#endif
}

/*
 * 测试以下 SweepGradientShaderObj 接口：
 * 1. CreateForUnmarshalling()
 * 2. Create(...) with various parameters
 * 3. Marshalling()
 * 4. Unmarshalling()
 * 5. GenerateBaseObject()
 */
bool SweepGradientShaderObjFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // Test CreateForUnmarshalling
    auto shaderObj = SweepGradientShaderObj::CreateForUnmarshalling();
    if (shaderObj == nullptr) {
        return false;
    }

#ifdef ROSEN_OHOS
    // Test Create with fuzzed parameters
    Point centerPt(GetObject<float>(), GetObject<float>());
    scalar startAngle = GetObject<scalar>();
    scalar endAngle = GetObject<scalar>();

    // Create color array
    uint32_t colorCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<UIColor> colors;
    colors.reserve(colorCount);
    for (uint32_t i = 0; i < colorCount && i < 10; i++) {
        float r = GetObject<float>();
        float g = GetObject<float>();
        float b = GetObject<float>();
        float a = GetObject<float>();
        float h = GetObject<float>();
        colors.emplace_back(r, g, b, a, h);
    }

    // Create position array
    uint32_t posCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<scalar> pos;
    pos.reserve(posCount);
    for (uint32_t i = 0; i < posCount && i < 10; i++) {
        pos.push_back(GetObject<scalar>());
    }

    TileMode mode = static_cast<TileMode>(GetObject<uint32_t>() % TILEMODE_SIZE);
    Matrix matrix;
    matrix.SetMatrix(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());

    auto validShader = SweepGradientShaderObj::Create(centerPt, colors, nullptr, pos, mode,
        startAngle, endAngle, &matrix);
    if (validShader == nullptr && !colors.empty()) {
        return false;
    }

    if (validShader != nullptr) {
        // Test GenerateBaseObject
        auto baseObject = validShader->GenerateBaseObject();
        if (baseObject == nullptr) {
            return false;
        }

        // Test Marshalling
        MessageParcel parcel;
        bool marshalResult = validShader->Marshalling(parcel);
        if (!marshalResult) {
            return false;
        }

        // Test Unmarshalling
        parcel.RewindRead(0);
        parcel.WriteInt32(validShader->GetType());
        parcel.WriteInt32(validShader->GetSubType());
        bool isValid = true;
        auto newShaderObj = SweepGradientShaderObj::CreateForUnmarshalling();
        if (newShaderObj == nullptr) {
            return false;
        }

        newShaderObj->Unmarshalling(parcel, isValid, 0);
    }

    return true;
#else
    return true;
#endif
}

/*
 * 测试以下 ShaderEffectLazy gradient 接口：
 * 1. CreateLinearGradient()
 * 2. CreateRadialGradient()
 * 3. CreateTwoPointConical()
 * 4. CreateSweepGradient()
 * 5. Materialize()
 * 6. Marshalling()
 * 7. Unmarshalling()
 */
bool ShaderEffectLazyGradientFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

#ifdef ROSEN_OHOS
    // Prepare common fuzzed parameters
    Point startPt(GetObject<float>(), GetObject<float>());
    Point endPt(GetObject<float>(), GetObject<float>());
    Point centerPt(GetObject<float>(), GetObject<float>());
    scalar radius = GetObject<scalar>();
    scalar startRadius = GetObject<scalar>();
    scalar endRadius = GetObject<scalar>();
    scalar startAngle = GetObject<scalar>();
    scalar endAngle = GetObject<scalar>();

    // Create color array
    uint32_t colorCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<UIColor> colors;
    colors.reserve(colorCount);
    for (uint32_t i = 0; i < colorCount && i < 5; i++) {
        float r = GetObject<float>();
        float g = GetObject<float>();
        float b = GetObject<float>();
        float a = GetObject<float>();
        float h = GetObject<float>();
        colors.emplace_back(r, g, b, a, h);
    }

    // Create position array
    uint32_t posCount = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    std::vector<scalar> pos;
    pos.reserve(posCount);
    for (uint32_t i = 0; i < posCount && i < 5; i++) {
        pos.push_back(GetObject<scalar>());
    }

    TileMode mode = static_cast<TileMode>(GetObject<uint32_t>() % TILEMODE_SIZE);

    // Test CreateLinearGradient
    if (!colors.empty()) {
        auto linearLazy = ShaderEffectLazy::CreateLinearGradient(startPt, endPt, colors, nullptr,
            pos, mode, nullptr);
        if (linearLazy != nullptr) {
            EXPECT_TRUE(linearLazy->IsLazy());
            auto materialized = linearLazy->Materialize();
            if (materialized == nullptr) {
                return false;
            }
            EXPECT_FALSE(materialized->IsLazy());
        }
    }

    // Test CreateRadialGradient
    if (!colors.empty()) {
        auto radialLazy = ShaderEffectLazy::CreateRadialGradient(centerPt, radius, colors, nullptr,
            pos, mode, nullptr);
        if (radialLazy != nullptr) {
            EXPECT_TRUE(radialLazy->IsLazy());
            auto materialized = radialLazy->Materialize();
            if (materialized == nullptr) {
                return false;
            }
            EXPECT_FALSE(materialized->IsLazy());
        }
    }

    // Test CreateTwoPointConical
    if (!colors.empty()) {
        auto conicalLazy = ShaderEffectLazy::CreateTwoPointConical(startPt, startRadius, endPt,
            endRadius, colors, nullptr, pos, mode, nullptr);
        if (conicalLazy != nullptr) {
            EXPECT_TRUE(conicalLazy->IsLazy());
            auto materialized = conicalLazy->Materialize();
            if (materialized == nullptr) {
                return false;
            }
            EXPECT_FALSE(materialized->IsLazy());
        }
    }

    // Test CreateSweepGradient
    if (!colors.empty()) {
        auto sweepLazy = ShaderEffectLazy::CreateSweepGradient(centerPt, colors, nullptr, pos,
            mode, startAngle, endAngle, nullptr);
        if (sweepLazy != nullptr) {
            EXPECT_TRUE(sweepLazy->IsLazy());
            auto materialized = sweepLazy->Materialize();
            if (materialized == nullptr) {
                return false;
            }
            EXPECT_FALSE(materialized->IsLazy());
        }
    }

    return true;
#else
    return true;
#endif
}

/*
 * 测试深度保护和边界条件
 * 1. Depth protection in Unmarshalling
 * 2. Empty colors handling
 * 3. Large arrays handling
 */
bool GradientShaderObjBoundaryTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

#ifdef ROSEN_OHOS
    // Test depth protection
    auto linearShader = LinearGradientShaderObj::CreateForUnmarshalling();
    auto radialShader = RadialGradientShaderObj::CreateForUnmarshalling();
    auto conicalShader = ConicalGradientShaderObj::CreateForUnmarshalling();
    auto sweepShader = SweepGradientShaderObj::CreateForUnmarshalling();

    if (linearShader == nullptr || radialShader == nullptr ||
        conicalShader == nullptr || sweepShader == nullptr) {
        return false;
    }

    MessageParcel parcel;
    bool isValid = true;

    // Test depth at limit
    int32_t maxDepth = ObjectHelper::MAX_NESTING_DEPTH;
    linearShader->Unmarshalling(parcel, isValid, maxDepth);
    radialShader->Unmarshalling(parcel, isValid, maxDepth);
    conicalShader->Unmarshalling(parcel, isValid, maxDepth);
    sweepShader->Unmarshalling(parcel, isValid, maxDepth);

    // Test with safe depth
    int32_t safeDepth = 0;
    linearShader->Unmarshalling(parcel, isValid, safeDepth);
    radialShader->Unmarshalling(parcel, isValid, safeDepth);
    conicalShader->Unmarshalling(parcel, isValid, safeDepth);
    sweepShader->Unmarshalling(parcel, isValid, safeDepth);

    return true;
#else
    return true;
#endif
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
    OHOS::Rosen::Drawing::LinearGradientShaderObjFuzzTest001(data, size);
    OHOS::Rosen::Drawing::RadialGradientShaderObjFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ConicalGradientShaderObjFuzzTest001(data, size);
    OHOS::Rosen::Drawing::SweepGradientShaderObjFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ShaderEffectLazyGradientFuzzTest001(data, size);
    OHOS::Rosen::Drawing::GradientShaderObjBoundaryTest001(data, size);
    return 0;
}
