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
#include "effect/conical_gradient_shader_obj.h"
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
class ConicalGradientShaderObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ConicalGradientShaderObjTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void ConicalGradientShaderObjTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}
void ConicalGradientShaderObjTest::SetUp() {}
void ConicalGradientShaderObjTest::TearDown() {}

/*
 * @tc.name: Constructor001
 * @tc.desc: Test ConicalGradientShaderObj constructor scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Constructor001, TestSize.Level1)
{
    // Test 1: CreateForUnmarshalling
    auto shaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT));

    // Test 2: Parameterized constructor with valid parameters
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
    Matrix matrix;

    auto validShaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors,
        colorSpace, pos, mode, &matrix);
    ASSERT_TRUE(validShaderObj != nullptr);
    EXPECT_EQ(validShaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(validShaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT));

    // Test 3: Constructor with empty colors
    std::vector<UIColor> emptyColors;
    auto emptyShaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, emptyColors,
        colorSpace, pos, mode, &matrix);
    EXPECT_TRUE(emptyShaderObj == nullptr); // Should return nullptr when colors is empty
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test ConicalGradientShaderObj::GenerateBaseObject with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, GenerateBaseObject001, TestSize.Level1)
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
    Matrix matrix;

    auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors, colorSpace,
        pos, mode, &matrix);
    auto baseObject = shaderObj->GenerateBaseObject();
    ASSERT_TRUE(baseObject != nullptr);

    // Try to cast to ShaderEffect
    auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
    EXPECT_TRUE(generatedShader != nullptr);
    if (generatedShader) {
        EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
        EXPECT_FALSE(generatedShader->IsLazy());
    }
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test ConicalGradientShaderObj::GenerateBaseObject with empty colors
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, GenerateBaseObject002, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    scalar startRadius = 0.0f;
    Point endPt(100.0f, 100.0f);
    scalar endRadius = 50.0f;
    std::vector<UIColor> emptyColors;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, emptyColors,
        colorSpace, pos, mode, nullptr);
    EXPECT_TRUE(shaderObj == nullptr); // Should return nullptr when colors is empty
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test ConicalGradientShaderObj::GenerateBaseObject with different tile modes
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, GenerateBaseObject003, TestSize.Level1)
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

    // Test different tile modes
    std::vector<TileMode> tileModes = {
        TileMode::CLAMP,
        TileMode::REPEAT,
        TileMode::MIRROR,
        TileMode::DECAL
    };

    for (const auto& mode : tileModes) {
        auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors, colorSpace,
        pos, mode, nullptr);
        auto baseObject = shaderObj->GenerateBaseObject();
        ASSERT_TRUE(baseObject != nullptr);

        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        ASSERT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
        }
    }
}

/*
 * @tc.name: GenerateBaseObject004
 * @tc.desc: Test ConicalGradientShaderObj::GenerateBaseObject with different radius values
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, GenerateBaseObject004, TestSize.Level1)
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

    // Test different radius configurations
    std::vector<std::pair<scalar, scalar>> radiusConfigs = {
        {0.0f, 50.0f},    // Normal: start at 0, end at 50
        {0.0f, 0.0f},     // Both zero
        {10.0f, 50.0f},   // Non-zero start
        {25.0f, 25.0f},   // Same start and end
        {50.0f, 25.0f},   // Start larger than end
        {-10.0f, 50.0f},  // Negative start
        {0.0f, -25.0f}    // Negative end
    };

    for (const auto& [startRadius, endRadius] : radiusConfigs) {
        auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors, colorSpace,
        pos, mode, nullptr);
        auto baseObject = shaderObj->GenerateBaseObject();
        ASSERT_TRUE(baseObject != nullptr);

        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        ASSERT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
        }
    }
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: Marshalling001
 * @tc.desc: Test ConicalGradientShaderObj::Marshalling with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Marshalling001, TestSize.Level1)
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
    Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors, colorSpace,
        pos, mode, &matrix);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result); // Should succeed with registered DataMarshallingCallback
}

/*
 * @tc.name: Marshalling002
 * @tc.desc: Test ConicalGradientShaderObj::Marshalling with null matrix
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Marshalling002, TestSize.Level1)
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

    auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors, colorSpace,
        pos, mode, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result); // Should succeed with null matrix
}

/*
 * @tc.name: Marshalling003
 * @tc.desc: Test ConicalGradientShaderObj::Marshalling with different tile modes
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Marshalling003, TestSize.Level1)
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

    // Test different tile modes
    std::vector<TileMode> tileModes = {
        TileMode::CLAMP,
        TileMode::REPEAT,
        TileMode::MIRROR,
        TileMode::DECAL
    };

    for (const auto& mode : tileModes) {
        auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors,
            colorSpace, pos, mode, nullptr);
        ASSERT_TRUE(shaderObj != nullptr);

        MessageParcel parcel;
        bool result = shaderObj->Marshalling(parcel);
        EXPECT_TRUE(result); // Should succeed for all tile modes
    }
}

/*
 * @tc.name: Unmarshalling001
 * @tc.desc: Test ConicalGradientShaderObj::Unmarshalling with valid data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Unmarshalling001, TestSize.Level1)
{
    // Create original shader object and marshal it
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

    auto originalShaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors,
        colorSpace, pos, mode, nullptr);
    ASSERT_TRUE(originalShaderObj != nullptr);

    MessageParcel parcel;
    // Write type and subType externally
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShaderObj != nullptr);

    // Read type and subType
    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT));

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
 * @tc.desc: Test ConicalGradientShaderObj::Unmarshalling depth protection
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Unmarshalling002, TestSize.Level1)
{
    auto shaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
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
 * @tc.desc: Test ConicalGradientShaderObj::Unmarshalling with safe depth
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Unmarshalling003, TestSize.Level1)
{
    auto shaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(shaderObj != nullptr);

    // Write valid gradient data
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));

    MessageParcel parcel;
    parcel.WriteFloat(startPt.GetX());
    parcel.WriteFloat(startPt.GetY());
    parcel.WriteFloat(0.0f); // startRadius
    parcel.WriteFloat(endPt.GetX());
    parcel.WriteFloat(endPt.GetY());
    parcel.WriteFloat(50.0f); // endRadius
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
 * @tc.desc: Test ConicalGradientShaderObj::Unmarshalling read failures
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, Unmarshalling004, TestSize.Level1)
{
    auto shaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(shaderObj != nullptr);

    // Test 1: Empty parcel - ReadFloat(startX) should fail
    MessageParcel emptyParcel;
    bool isValid = true;
    bool result1 = shaderObj->Unmarshalling(emptyParcel, isValid);
    EXPECT_FALSE(result1);

    // Test 2: Parcel with only startPt X - ReadFloat(startY) should fail
    MessageParcel partialParcel1;
    partialParcel1.WriteFloat(10.0f); // Only startX
    result1 = shaderObj->Unmarshalling(partialParcel1, isValid);
    EXPECT_FALSE(result1);

    // Test 3: Parcel with startPt - ReadFloat(startRadius) should fail
    MessageParcel partialParcel2;
    partialParcel2.WriteFloat(10.0f); // startX
    partialParcel2.WriteFloat(10.0f); // startY
    bool result2 = shaderObj->Unmarshalling(partialParcel2, isValid);
    EXPECT_FALSE(result2);

    // Test 4: Parcel with startPt and startRadius - ReadFloat(endX) should fail
    MessageParcel partialParcel3;
    partialParcel3.WriteFloat(10.0f); // startX
    partialParcel3.WriteFloat(10.0f); // startY
    partialParcel3.WriteFloat(5.0f); // startRadius
    bool result3 = shaderObj->Unmarshalling(partialParcel3, isValid);
    EXPECT_FALSE(result3);

    // Test 5: Parcel with start data and endPt X - ReadFloat(endY) should fail
    MessageParcel partialParcel4;
    partialParcel4.WriteFloat(10.0f); // startX
    partialParcel4.WriteFloat(10.0f); // startY
    partialParcel4.WriteFloat(5.0f); // startRadius
    partialParcel4.WriteFloat(100.0f); // endX
    bool result4 = shaderObj->Unmarshalling(partialParcel4, isValid);
    EXPECT_FALSE(result4);

    // Test 6: Parcel with start data and endPt - ReadFloat(endRadius) should fail
    MessageParcel partialParcel5;
    partialParcel5.WriteFloat(10.0f); // startX
    partialParcel5.WriteFloat(10.0f); // startY
    partialParcel5.WriteFloat(5.0f); // startRadius
    partialParcel5.WriteFloat(100.0f); // endX
    partialParcel5.WriteFloat(100.0f); // endY
    bool result5 = shaderObj->Unmarshalling(partialParcel5, isValid);
    EXPECT_FALSE(result5);
}

/*
 * @tc.name: MarshallingUnmarshallingRoundTrip001
 * @tc.desc: Test complete marshalling/unmarshalling round trip for ConicalGradientShaderObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    // Create original ConicalGradientShaderObj
    Point startPt(25.0f, 25.0f);
    scalar startRadius = 10.0f;
    Point endPt(125.0f, 125.0f);
    scalar endRadius = 60.0f;
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

    auto originalShaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors,
        colorSpace, pos, mode, &matrix);
    ASSERT_TRUE(originalShaderObj != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShaderObj = ConicalGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShaderObj != nullptr);

    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT));

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
 * @tc.name: CreateWithDifferentPoints001
 * @tc.desc: Test ConicalGradientShaderObj::Create with various point configurations
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, CreateWithDifferentPoints001, TestSize.Level1)
{
    scalar startRadius = 0.0f;
    scalar endRadius = 50.0f;
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    TileMode mode = TileMode::CLAMP;

    // Test different point configurations
    std::vector<std::pair<Point, Point>> pointConfigs = {
        {Point(0.0f, 0.0f), Point(100.0f, 100.0f)},  // Diagonal
        {Point(0.0f, 0.0f), Point(100.0f, 0.0f)},    // Horizontal
        {Point(0.0f, 0.0f), Point(0.0f, 100.0f)},    // Vertical
        {Point(-50.0f, -50.0f), Point(50.0f, 50.0f)}, // Negative to positive
        {Point(50.0f, 50.0f), Point(50.0f, 50.0f)}   // Same points
    };

    for (const auto& [start, end] : pointConfigs) {
        auto shaderObj = ConicalGradientShaderObj::Create(start, startRadius, end, endRadius, colors, colorSpace,
            pos, mode, nullptr);
        ASSERT_TRUE(shaderObj != nullptr);

        auto baseObject = shaderObj->GenerateBaseObject();
        ASSERT_TRUE(baseObject != nullptr);

        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        ASSERT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
        }
    }
}

/*
 * @tc.name: CreateWithMultipleColorStops001
 * @tc.desc: Test ConicalGradientShaderObj::Create with multiple color stops
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ConicalGradientShaderObjTest, CreateWithMultipleColorStops001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    scalar startRadius = 0.0f;
    Point endPt(100.0f, 100.0f);
    scalar endRadius = 50.0f;
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

    auto shaderObj = ConicalGradientShaderObj::Create(startPt, startRadius, endPt, endRadius, colors, colorSpace,
        pos, mode, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    auto baseObject = shaderObj->GenerateBaseObject();
    ASSERT_TRUE(baseObject != nullptr);

    auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
    ASSERT_TRUE(generatedShader != nullptr);
    if (generatedShader) {
        EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
    }
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
