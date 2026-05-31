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

#include "gtest/gtest.h"

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#endif

#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#include "draw/blend_mode.h"
#include "draw/ui_color.h"
#ifdef ROSEN_OHOS
#include "utils/object_helper.h"
#endif
#include "effect_test_utils.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShaderEffectLazyGradientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ShaderEffectLazyGradientTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void ShaderEffectLazyGradientTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}
void ShaderEffectLazyGradientTest::SetUp() {}
void ShaderEffectLazyGradientTest::TearDown() {}

/*
 * @tc.name: CreateLinearGradient001
 * @tc.desc: Test ShaderEffectLazy::CreateLinearGradient with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateLinearGradient001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    auto lazyGradient = ShaderEffectLazy::CreateLinearGradient(startPt, endPt, colors, colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());
    EXPECT_EQ(lazyGradient->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify the underlying object
    auto shaderObj = lazyGradient->GetShaderEffectObj();
    EXPECT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT));
}

/*
 * @tc.name: CreateLinearGradient002
 * @tc.desc: Test ShaderEffectLazy::CreateLinearGradient with empty colors
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateLinearGradient002, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> emptyColors;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    TileMode mode = TileMode::CLAMP;

    auto lazyGradient = ShaderEffectLazy::CreateLinearGradient(startPt, endPt, emptyColors, colorSpace, pos,
        mode, nullptr);
    EXPECT_TRUE(lazyGradient == nullptr); // Should return nullptr for empty colors
}

/*
 * @tc.name: CreateLinearGradient003
 * @tc.desc: Test ShaderEffectLazy::CreateLinearGradient materialization
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateLinearGradient003, TestSize.Level1)
{
    Point startPt(10.0f, 10.0f);
    Point endPt(90.0f, 90.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::REPEAT;

    auto lazyGradient = ShaderEffectLazy::CreateLinearGradient(startPt, endPt, colors, colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());

    // Materialize the lazy gradient
    auto materializedShader = lazyGradient->Materialize();
    EXPECT_TRUE(materializedShader != nullptr);
    EXPECT_FALSE(materializedShader->IsLazy());
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::LINEAR_GRADIENT);
}

/*
 * @tc.name: CreateRadialGradient001
 * @tc.desc: Test ShaderEffectLazy::CreateRadialGradient with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateRadialGradient001, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    scalar radius = 25.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    auto lazyGradient = ShaderEffectLazy::CreateRadialGradient(centerPt, radius, colors, colorSpace, pos, mode,
        nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());
    EXPECT_EQ(lazyGradient->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify the underlying object
    auto shaderObj = lazyGradient->GetShaderEffectObj();
    EXPECT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT));
}

/*
 * @tc.name: CreateRadialGradient002
 * @tc.desc: Test ShaderEffectLazy::CreateRadialGradient with empty colors
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateRadialGradient002, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    scalar radius = 25.0f;
    std::vector<UIColor> emptyColors;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    TileMode mode = TileMode::CLAMP;

    auto lazyGradient = ShaderEffectLazy::CreateRadialGradient(centerPt, radius, emptyColors, colorSpace, pos,
        mode, nullptr);
    EXPECT_TRUE(lazyGradient == nullptr); // Should return nullptr for empty colors
}

/*
 * @tc.name: CreateRadialGradient003
 * @tc.desc: Test ShaderEffectLazy::CreateRadialGradient materialization
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateRadialGradient003, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    scalar radius = 35.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::MIRROR;

    auto lazyGradient = ShaderEffectLazy::CreateRadialGradient(centerPt, radius, colors, colorSpace, pos, mode,
        nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());

    // Materialize the lazy gradient
    auto materializedShader = lazyGradient->Materialize();
    EXPECT_TRUE(materializedShader != nullptr);
    EXPECT_FALSE(materializedShader->IsLazy());
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
}

/*
 * @tc.name: CreateTwoPointConical001
 * @tc.desc: Test ShaderEffectLazy::CreateTwoPointConical with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateTwoPointConical001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    scalar startRadius = 0.0f;
    Point endPt(100.0f, 100.0f);
    scalar endRadius = 50.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    auto lazyGradient = ShaderEffectLazy::CreateTwoPointConical(startPt, startRadius, endPt, endRadius, colors,
        colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());
    EXPECT_EQ(lazyGradient->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify the underlying object
    auto shaderObj = lazyGradient->GetShaderEffectObj();
    EXPECT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT));
}

/*
 * @tc.name: CreateTwoPointConical002
 * @tc.desc: Test ShaderEffectLazy::CreateTwoPointConical with empty colors
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateTwoPointConical002, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    scalar startRadius = 0.0f;
    Point endPt(100.0f, 100.0f);
    scalar endRadius = 50.0f;
    std::vector<UIColor> emptyColors;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    TileMode mode = TileMode::CLAMP;

    auto lazyGradient = ShaderEffectLazy::CreateTwoPointConical(startPt, startRadius, endPt, endRadius, emptyColors,
        colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(lazyGradient == nullptr); // Should return nullptr for empty colors
}

/*
 * @tc.name: CreateTwoPointConical003
 * @tc.desc: Test ShaderEffectLazy::CreateTwoPointConical materialization
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateTwoPointConical003, TestSize.Level1)
{
    Point startPt(10.0f, 10.0f);
    scalar startRadius = 5.0f;
    Point endPt(110.0f, 110.0f);
    scalar endRadius = 55.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::REPEAT;

    auto lazyGradient = ShaderEffectLazy::CreateTwoPointConical(startPt, startRadius, endPt, endRadius, colors,
        colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());

    // Materialize the lazy gradient
    auto materializedShader = lazyGradient->Materialize();
    EXPECT_TRUE(materializedShader != nullptr);
    EXPECT_FALSE(materializedShader->IsLazy());
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
}

/*
 * @tc.name: CreateSweepGradient001
 * @tc.desc: Test ShaderEffectLazy::CreateSweepGradient with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateSweepGradient001, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;
    scalar startAngle = 0.0f;
    scalar endAngle = 360.0f;

    auto lazyGradient = ShaderEffectLazy::CreateSweepGradient(centerPt, colors, colorSpace, pos, mode, startAngle,
        endAngle, nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());
    EXPECT_EQ(lazyGradient->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify the underlying object
    auto shaderObj = lazyGradient->GetShaderEffectObj();
    EXPECT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::SWEEP_GRADIENT));
}

/*
 * @tc.name: CreateSweepGradient002
 * @tc.desc: Test ShaderEffectLazy::CreateSweepGradient with empty colors
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateSweepGradient002, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    std::vector<UIColor> emptyColors;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    TileMode mode = TileMode::CLAMP;
    scalar startAngle = 0.0f;
    scalar endAngle = 360.0f;

    auto lazyGradient = ShaderEffectLazy::CreateSweepGradient(centerPt, emptyColors, colorSpace, pos, mode,
        startAngle, endAngle, nullptr);
    EXPECT_TRUE(lazyGradient == nullptr); // Should return nullptr for empty colors
}

/*
 * @tc.name: CreateSweepGradient003
 * @tc.desc: Test ShaderEffectLazy::CreateSweepGradient materialization
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, CreateSweepGradient003, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::MIRROR;
    scalar startAngle = 45.0f;
    scalar endAngle = 315.0f;

    auto lazyGradient = ShaderEffectLazy::CreateSweepGradient(centerPt, colors, colorSpace, pos, mode, startAngle,
        endAngle, nullptr);
    EXPECT_TRUE(lazyGradient != nullptr);
    EXPECT_TRUE(lazyGradient->IsLazy());

    // Materialize the lazy gradient
    auto materializedShader = lazyGradient->Materialize();
    EXPECT_TRUE(materializedShader != nullptr);
    EXPECT_FALSE(materializedShader->IsLazy());
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::SWEEP_GRADIENT);
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: LinearGradientMarshallingRoundTrip001
 * @tc.desc: Test LinearGradient marshalling/unmarshalling round trip
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, LinearGradientMarshallingRoundTrip001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    auto originalLazyGradient = ShaderEffectLazy::CreateLinearGradient(startPt, endPt, colors, colorSpace, pos, mode,
        nullptr);
    EXPECT_TRUE(originalLazyGradient != nullptr);

    // Marshal
    MessageParcel parcel;
    bool marshalResult = originalLazyGradient->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    parcel.RewindRead(0);
    bool isValid = true;
    auto unmarshalledLazyGradient = ShaderEffectLazy::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(isValid);
    EXPECT_TRUE(unmarshalledLazyGradient != nullptr);
    EXPECT_TRUE(unmarshalledLazyGradient->IsLazy());

    // Verify types
    auto originalObj = originalLazyGradient->GetShaderEffectObj();
    auto unmarshalledObj = unmarshalledLazyGradient->GetShaderEffectObj();
    EXPECT_EQ(originalObj->GetType(), unmarshalledObj->GetType());
    EXPECT_EQ(originalObj->GetSubType(), unmarshalledObj->GetSubType());

    // Materialize both and compare
    auto originalMaterialized = originalLazyGradient->Materialize();
    auto unmarshalledMaterialized = unmarshalledLazyGradient->Materialize();
    EXPECT_TRUE(originalMaterialized != nullptr);
    EXPECT_TRUE(unmarshalledMaterialized != nullptr);

    // Compare serialized data
    auto originalData = originalMaterialized->Serialize();
    auto unmarshalledData = unmarshalledMaterialized->Serialize();
    EXPECT_TRUE(originalData != nullptr);
    EXPECT_TRUE(unmarshalledData != nullptr);
    EXPECT_EQ(originalData->GetSize(), unmarshalledData->GetSize());

    const uint8_t* originalBytes = static_cast<const uint8_t*>(originalData->GetData());
    const uint8_t* unmarshalledBytes = static_cast<const uint8_t*>(unmarshalledData->GetData());
    int memResult = memcmp(originalBytes, unmarshalledBytes, originalData->GetSize());
    EXPECT_EQ(memResult, 0);
}

/*
 * @tc.name: RadialGradientMarshallingRoundTrip001
 * @tc.desc: Test RadialGradient marshalling/unmarshalling round trip
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, RadialGradientMarshallingRoundTrip001, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    scalar radius = 25.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::REPEAT;

    auto originalLazyGradient = ShaderEffectLazy::CreateRadialGradient(centerPt, radius, colors, colorSpace, pos, mode,
        nullptr);
    EXPECT_TRUE(originalLazyGradient != nullptr);

    // Marshal
    MessageParcel parcel;
    bool marshalResult = originalLazyGradient->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    parcel.RewindRead(0);
    bool isValid = true;
    auto unmarshalledLazyGradient = ShaderEffectLazy::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(isValid);
    EXPECT_TRUE(unmarshalledLazyGradient != nullptr);
    EXPECT_TRUE(unmarshalledLazyGradient->IsLazy());

    // Verify types
    auto originalObj = originalLazyGradient->GetShaderEffectObj();
    auto unmarshalledObj = unmarshalledLazyGradient->GetShaderEffectObj();
    EXPECT_EQ(originalObj->GetType(), unmarshalledObj->GetType());
    EXPECT_EQ(originalObj->GetSubType(), unmarshalledObj->GetSubType());

    // Materialize both and compare
    auto originalMaterialized = originalLazyGradient->Materialize();
    auto unmarshalledMaterialized = unmarshalledLazyGradient->Materialize();
    EXPECT_TRUE(originalMaterialized != nullptr);
    EXPECT_TRUE(unmarshalledMaterialized != nullptr);

    // Compare serialized data
    auto originalData = originalMaterialized->Serialize();
    auto unmarshalledData = unmarshalledMaterialized->Serialize();
    EXPECT_TRUE(originalData != nullptr);
    EXPECT_TRUE(unmarshalledData != nullptr);
    EXPECT_EQ(originalData->GetSize(), unmarshalledData->GetSize());

    const uint8_t* originalBytes = static_cast<const uint8_t*>(originalData->GetData());
    const uint8_t* unmarshalledBytes = static_cast<const uint8_t*>(unmarshalledData->GetData());
    int memResult = memcmp(originalBytes, unmarshalledBytes, originalData->GetSize());
    EXPECT_EQ(memResult, 0);
}

/*
 * @tc.name: ConicalGradientMarshallingRoundTrip001
 * @tc.desc: Test ConicalGradient marshalling/unmarshalling round trip
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, ConicalGradientMarshallingRoundTrip001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    scalar startRadius = 0.0f;
    Point endPt(100.0f, 100.0f);
    scalar endRadius = 50.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::MIRROR;

    auto originalLazyGradient = ShaderEffectLazy::CreateTwoPointConical(startPt, startRadius, endPt, endRadius, colors,
        colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(originalLazyGradient != nullptr);

    // Marshal
    MessageParcel parcel;
    bool marshalResult = originalLazyGradient->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    parcel.RewindRead(0);
    bool isValid = true;
    auto unmarshalledLazyGradient = ShaderEffectLazy::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(isValid);
    EXPECT_TRUE(unmarshalledLazyGradient != nullptr);
    EXPECT_TRUE(unmarshalledLazyGradient->IsLazy());

    // Verify types
    auto originalObj = originalLazyGradient->GetShaderEffectObj();
    auto unmarshalledObj = unmarshalledLazyGradient->GetShaderEffectObj();
    EXPECT_EQ(originalObj->GetType(), unmarshalledObj->GetType());
    EXPECT_EQ(originalObj->GetSubType(), unmarshalledObj->GetSubType());

    // Materialize both and compare
    auto originalMaterialized = originalLazyGradient->Materialize();
    auto unmarshalledMaterialized = unmarshalledLazyGradient->Materialize();
    EXPECT_TRUE(originalMaterialized != nullptr);
    EXPECT_TRUE(unmarshalledMaterialized != nullptr);

    // Compare serialized data
    auto originalData = originalMaterialized->Serialize();
    auto unmarshalledData = unmarshalledMaterialized->Serialize();
    EXPECT_TRUE(originalData != nullptr);
    EXPECT_TRUE(unmarshalledData != nullptr);
    EXPECT_EQ(originalData->GetSize(), unmarshalledData->GetSize());

    const uint8_t* originalBytes = static_cast<const uint8_t*>(originalData->GetData());
    const uint8_t* unmarshalledBytes = static_cast<const uint8_t*>(unmarshalledData->GetData());
    int memResult = memcmp(originalBytes, unmarshalledBytes, originalData->GetSize());
    EXPECT_EQ(memResult, 0);
}

/*
 * @tc.name: SweepGradientMarshallingRoundTrip001
 * @tc.desc: Test SweepGradient marshalling/unmarshalling round trip
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyGradientTest, SweepGradientMarshallingRoundTrip001, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;
    scalar startAngle = 45.0f;
    scalar endAngle = 315.0f;

    auto originalLazyGradient = ShaderEffectLazy::CreateSweepGradient(centerPt, colors, colorSpace, pos, mode,
        startAngle, endAngle, nullptr);
    EXPECT_TRUE(originalLazyGradient != nullptr);

    // Marshal
    MessageParcel parcel;
    bool marshalResult = originalLazyGradient->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    parcel.RewindRead(0);
    bool isValid = true;
    auto unmarshalledLazyGradient = ShaderEffectLazy::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(isValid);
    EXPECT_TRUE(unmarshalledLazyGradient != nullptr);
    EXPECT_TRUE(unmarshalledLazyGradient->IsLazy());

    // Verify types
    auto originalObj = originalLazyGradient->GetShaderEffectObj();
    auto unmarshalledObj = unmarshalledLazyGradient->GetShaderEffectObj();
    EXPECT_EQ(originalObj->GetType(), unmarshalledObj->GetType());
    EXPECT_EQ(originalObj->GetSubType(), unmarshalledObj->GetSubType());

    // Materialize both and compare
    auto originalMaterialized = originalLazyGradient->Materialize();
    auto unmarshalledMaterialized = unmarshalledLazyGradient->Materialize();
    EXPECT_TRUE(originalMaterialized != nullptr);
    EXPECT_TRUE(unmarshalledMaterialized != nullptr);

    // Compare serialized data
    auto originalData = originalMaterialized->Serialize();
    auto unmarshalledData = unmarshalledMaterialized->Serialize();
    EXPECT_TRUE(originalData != nullptr);
    EXPECT_TRUE(unmarshalledData != nullptr);
    EXPECT_EQ(originalData->GetSize(), unmarshalledData->GetSize());

    const uint8_t* originalBytes = static_cast<const uint8_t*>(originalData->GetData());
    const uint8_t* unmarshalledBytes = static_cast<const uint8_t*>(unmarshalledData->GetData());
    int memResult = memcmp(originalBytes, unmarshalledBytes, originalData->GetSize());
    EXPECT_EQ(memResult, 0);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
