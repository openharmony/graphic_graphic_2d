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

#include "gtest/gtest.h"
#define private public
#include "effect/radial_gradient_shader_obj.h"
#undef private
#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#include "utils/object_helper.h"
#endif
#include "transaction/rs_marshalling_helper.h"
#include "effect_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RadialGradientShaderObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RadialGradientShaderObjTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void RadialGradientShaderObjTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}
void RadialGradientShaderObjTest::SetUp() {}
void RadialGradientShaderObjTest::TearDown() {}

/*
 * @tc.name: Constructor001
 * @tc.desc: Test RadialGradientShaderObj constructor scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Constructor001, TestSize.Level1)
{
    // Test 1: CreateForUnmarshalling
    auto shaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT));

    // Test 2: Parameterized constructor with valid parameters
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
    Matrix matrix;

    auto validShaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode,
        &matrix);
    ASSERT_TRUE(validShaderObj != nullptr);
    EXPECT_EQ(validShaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(validShaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT));

    // Test 3: Constructor with empty colors
    std::vector<UIColor> emptyColors;
    auto emptyShaderObj = RadialGradientShaderObj::Create(centerPt, radius, emptyColors, colorSpace, pos, mode,
        &matrix);
    EXPECT_TRUE(emptyShaderObj == nullptr); // Should return nullptr when colors is empty
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test RadialGradientShaderObj::GenerateBaseObject with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, GenerateBaseObject001, TestSize.Level1)
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
    Matrix matrix;

    auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, &matrix);
    auto baseObject = shaderObj->GenerateBaseObject();
    ASSERT_TRUE(baseObject != nullptr);

    // Try to cast to ShaderEffect
    auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
    EXPECT_TRUE(generatedShader != nullptr);
    if (generatedShader) {
        EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
        EXPECT_FALSE(generatedShader->IsLazy());
    }
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test RadialGradientShaderObj::GenerateBaseObject with empty colors
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, GenerateBaseObject002, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    scalar radius = 25.0f;
    std::vector<UIColor> emptyColors;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, emptyColors, colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(shaderObj == nullptr); // Should return nullptr when colors is empty
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test RadialGradientShaderObj::GenerateBaseObject with different tile modes
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, GenerateBaseObject003, TestSize.Level1)
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

    // Test different tile modes
    std::vector<TileMode> tileModes = {
        TileMode::CLAMP,
        TileMode::REPEAT,
        TileMode::MIRROR,
        TileMode::DECAL
    };

    for (const auto& mode : tileModes) {
        auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, nullptr);
        auto baseObject = shaderObj->GenerateBaseObject();
        ASSERT_TRUE(baseObject != nullptr);

        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        ASSERT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
        }
    }
}

/*
 * @tc.name: GenerateBaseObject004
 * @tc.desc: Test RadialGradientShaderObj::GenerateBaseObject with different radius values
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, GenerateBaseObject004, TestSize.Level1)
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

    // Test different radius values
    std::vector<scalar> radii = {0.0f, 1.0f, 25.0f, 50.0f, 100.0f, -10.0f};

    for (const auto& radius : radii) {
        auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, nullptr);
        auto baseObject = shaderObj->GenerateBaseObject();
        ASSERT_TRUE(baseObject != nullptr);

        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        ASSERT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
        }
    }
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: Marshalling001
 * @tc.desc: Test RadialGradientShaderObj::Marshalling with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Marshalling001, TestSize.Level1)
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
    Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, &matrix);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result); // Should succeed with registered DataMarshallingCallback
}

/*
 * @tc.name: Marshalling002
 * @tc.desc: Test RadialGradientShaderObj::Marshalling with null matrix
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Marshalling002, TestSize.Level1)
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

    auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result); // Should succeed with null matrix
}

/*
 * @tc.name: Marshalling003
 * @tc.desc: Test RadialGradientShaderObj::Marshalling without DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Marshalling003, TestSize.Level1)
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

    // Test different tile modes
    std::vector<TileMode> tileModes = {
        TileMode::CLAMP,
        TileMode::REPEAT,
        TileMode::MIRROR,
        TileMode::DECAL
    };

    for (const auto& mode : tileModes) {
        auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, nullptr);
        ASSERT_TRUE(shaderObj != nullptr);

        MessageParcel parcel;
        bool result = shaderObj->Marshalling(parcel);
        EXPECT_TRUE(result); // Should succeed for all tile modes
    }
}

/*
 * @tc.name: Unmarshalling001
 * @tc.desc: Test RadialGradientShaderObj::Unmarshalling with valid data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Unmarshalling001, TestSize.Level1)
{
    // Create original shader object and marshal it
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

    auto originalShaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, nullptr);
    ASSERT_TRUE(originalShaderObj != nullptr);

    MessageParcel parcel;
    // Write type and subType externally
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShaderObj != nullptr);

    // Read type and subType
    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT));

    bool isValid = true;
    bool unmarshalResult = newShaderObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);

    // Verify consistency
    EXPECT_EQ(originalShaderObj->GetType(), newShaderObj->GetType());
    EXPECT_EQ(originalShaderObj->GetSubType(), newShaderObj->GetSubType());

    // Test that both can generate base objects
    auto originalBaseObject = originalShaderObj->GenerateBaseObject();
    auto newBaseObject = newShaderObj->GenerateBaseObject();
    ASSERT_TRUE(originalBaseObject != nullptr);
    ASSERT_TRUE(newBaseObject != nullptr);
}

/*
 * @tc.name: Unmarshalling002
 * @tc.desc: Test RadialGradientShaderObj::Unmarshalling depth protection
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Unmarshalling002, TestSize.Level1)
{
    auto shaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    // Test with depth at MAX_NESTING_DEPTH limit
    int32_t maxDepth = ObjectHelper::MAX_NESTING_DEPTH;
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid, maxDepth);
    EXPECT_FALSE(result); // Should fail due to depth protection
}

/*
 * @tc.name: Unmarshalling003
 * @tc.desc: Test RadialGradientShaderObj::Unmarshalling with safe depth
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Unmarshalling003, TestSize.Level1)
{
    auto shaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(shaderObj != nullptr);

    // Write valid gradient data
    Point centerPt(50.0f, 50.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));

    MessageParcel parcel;
    parcel.WriteFloat(centerPt.GetX());
    parcel.WriteFloat(centerPt.GetY());
    parcel.WriteFloat(25.0f); // radius
    parcel.WriteUint32(colors.size());
    for (const auto& color : colors) {
        parcel.WriteFloat(color.GetRed());
        parcel.WriteFloat(color.GetGreen());
        parcel.WriteFloat(color.GetBlue());
        parcel.WriteFloat(color.GetAlpha());
        parcel.WriteFloat(color.GetHeadroom());
    }
    parcel.WriteUint32(0); // Empty positions
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP));
    parcel.WriteBool(false); // No matrix

    // Test with safe depth
    int32_t safeDepth = ObjectHelper::MAX_NESTING_DEPTH / 2;
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid, safeDepth);
    EXPECT_FALSE(result);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: Unmarshalling004
 * @tc.desc: Test RadialGradientShaderObj::Unmarshalling read failures
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, Unmarshalling004, TestSize.Level1)
{
    auto shaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(shaderObj != nullptr);

    // Test 1: Empty parcel - ReadFloat(centerX) should fail
    MessageParcel emptyParcel;
    bool isValid = true;
    bool result1 = shaderObj->Unmarshalling(emptyParcel, isValid);
    EXPECT_FALSE(result1);

    // Test 2: Parcel with only centerPt X - ReadFloat(centerY) should fail
    MessageParcel partialParcel1;
    partialParcel1.WriteFloat(50.0f); // Only centerX
    result1 = shaderObj->Unmarshalling(partialParcel1, isValid);
    EXPECT_FALSE(result1);

    // Test 3: Parcel with only centerPt - ReadFloat(radius) should fail
    MessageParcel partialParcel2;
    partialParcel2.WriteFloat(50.0f); // centerX
    partialParcel2.WriteFloat(50.0f); // centerY
    bool result2 = shaderObj->Unmarshalling(partialParcel2, isValid);
    EXPECT_FALSE(result2);

    // Test 4: Parcel with centerPt and radius X - radius read should succeed but color count read should fail
    MessageParcel partialParcel3;
    partialParcel3.WriteFloat(50.0f); // centerX
    partialParcel3.WriteFloat(50.0f); // centerY
    partialParcel3.WriteFloat(25.0f); // radius
    // Missing color count
    bool result3 = shaderObj->Unmarshalling(partialParcel3, isValid);
    EXPECT_FALSE(result3);
}

/*
 * @tc.name: MarshallingUnmarshallingRoundTrip001
 * @tc.desc: Test complete marshalling/unmarshalling round trip for RadialGradientShaderObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    // Create original RadialGradientShaderObj
    Point centerPt(75.0f, 75.0f);
    scalar radius = 35.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 0.0f, 1.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(0.5f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::MIRROR;
    Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 10.0f, 0.0f, 1.0f, 20.0f, 0.0f, 0.0f, 1.0f);

    auto originalShaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, &matrix);
    ASSERT_TRUE(originalShaderObj != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShaderObj = RadialGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShaderObj != nullptr);

    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT));

    bool isValid = true;
    bool unmarshalResult = newShaderObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);

    // Verify consistency through serialization
    auto originalBaseObject = originalShaderObj->GenerateBaseObject();
    auto newBaseObject = newShaderObj->GenerateBaseObject();
    ASSERT_TRUE(originalBaseObject != nullptr);
    ASSERT_TRUE(newBaseObject != nullptr);

    auto originalShader = std::static_pointer_cast<ShaderEffect>(originalBaseObject);
    auto newShader = std::static_pointer_cast<ShaderEffect>(newBaseObject);

    auto originalData = originalShader->Serialize();
    auto newData = newShader->Serialize();
    ASSERT_TRUE(originalData != nullptr);
    ASSERT_TRUE(newData != nullptr);
    EXPECT_EQ(originalData->GetSize(), newData->GetSize());

    // Compare serialized memory content
    const void* originalMemory = originalData->GetData();
    const void* newMemory = newData->GetData();
    ASSERT_TRUE(originalMemory != nullptr);
    ASSERT_TRUE(newMemory != nullptr);
    int memResult = memcmp(originalMemory, newMemory, originalData->GetSize());
    EXPECT_EQ(memResult, 0);
}

/*
 * @tc.name: CreateWithDifferentCenters001
 * @tc.desc: Test RadialGradientShaderObj::Create with various center configurations
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, CreateWithDifferentCenters001, TestSize.Level1)
{
    scalar radius = 25.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    // Test different center configurations
    std::vector<Point> centers = {
        Point(0.0f, 0.0f),        // Origin
        Point(50.0f, 50.0f),      // Positive coordinates
        Point(-50.0f, -50.0f),    // Negative coordinates
        Point(100.0f, 0.0f),      // X-axis
        Point(0.0f, 100.0f)       // Y-axis
    };

    for (const auto& center : centers) {
        auto shaderObj = RadialGradientShaderObj::Create(center, radius, colors, colorSpace, pos, mode, nullptr);
        ASSERT_TRUE(shaderObj != nullptr);

        auto baseObject = shaderObj->GenerateBaseObject();
        ASSERT_TRUE(baseObject != nullptr);

        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        ASSERT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
        }
    }
}

/*
 * @tc.name: CreateWithMultipleColorStops001
 * @tc.desc: Test RadialGradientShaderObj::Create with multiple color stops
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RadialGradientShaderObjTest, CreateWithMultipleColorStops001, TestSize.Level1)
{
    Point centerPt(50.0f, 50.0f);
    scalar radius = 35.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));   // Red
    colors.push_back(UIColor(1.0f, 1.0f, 0.0f, 1.0f, 0.0f));   // Yellow
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));   // Green
    colors.push_back(UIColor(0.0f, 1.0f, 1.0f, 1.0f, 0.0f));   // Cyan
    colors.push_back(UIColor(0.0f, 0.0f, 1.0f, 1.0f, 0.0f));   // Blue
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(0.25f);
    pos.push_back(0.5f);
    pos.push_back(0.75f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    auto shaderObj = RadialGradientShaderObj::Create(centerPt, radius, colors, colorSpace, pos, mode, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    auto baseObject = shaderObj->GenerateBaseObject();
    ASSERT_TRUE(baseObject != nullptr);

    auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
    ASSERT_TRUE(generatedShader != nullptr);
    if (generatedShader) {
        EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
    }
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
