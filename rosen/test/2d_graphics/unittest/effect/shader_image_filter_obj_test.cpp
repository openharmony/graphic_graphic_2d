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
#include <message_parcel.h>
#endif

#include "effect/image_filter.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#include "effect/shader_image_filter_obj.h"
#include "effect_test_utils.h"
#include "transaction/rs_marshalling_helper.h"
#include "utils/object_helper.h"
#include "utils/rect.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShaderImageFilterObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ShaderImageFilterObjTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void ShaderImageFilterObjTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}

void ShaderImageFilterObjTest::SetUp() {}
void ShaderImageFilterObjTest::TearDown() {}

/*
 * @tc.name: CreateForUnmarshalling001
 * @tc.desc: Test ShaderImageFilterObj::CreateForUnmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, CreateForUnmarshalling001, TestSize.Level1)
{
    auto obj = ShaderImageFilterObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::SHADER));
}

/*
 * @tc.name: Create001
 * @tc.desc: Test ShaderImageFilterObj::Create with different shader types and scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, Create001, TestSize.Level1)
{
    // Test 1: Create with valid shader and crop rect
    auto shader = ShaderEffect::CreateColorShader(0xFF0000FF);
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto obj = ShaderImageFilterObj::Create(shader, cropRect);
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::SHADER));

    // Test 2: Create with null shader
    std::shared_ptr<ShaderEffect> nullShader = nullptr;
    Rect nullCropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto nullObj = ShaderImageFilterObj::Create(nullShader, nullCropRect);
    EXPECT_TRUE(nullObj != nullptr);
    EXPECT_EQ(nullObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(nullObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::SHADER));

    // Test 3: Create with Lazy shader
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    Rect lazyCropRect(10.0f, 20.0f, 110.0f, 120.0f);
    auto lazyObj = ShaderImageFilterObj::Create(lazyShader, lazyCropRect);
    EXPECT_TRUE(lazyObj != nullptr);
    EXPECT_EQ(lazyObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(lazyObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::SHADER));
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test ShaderImageFilterObj::GenerateBaseObject with null shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, GenerateBaseObject001, TestSize.Level1)
{
    Rect cropRect(5.0f, 5.0f, 95.0f, 95.0f);
    auto obj = ShaderImageFilterObj::Create(nullptr, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::SHADER);

    // Compare with direct creation using null shader
    auto directFilter = ImageFilter::CreateShaderImageFilter(nullptr, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test ShaderImageFilterObj::GenerateBaseObject with lazy shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, GenerateBaseObject002, TestSize.Level1)
{
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    auto nonLazyDstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto nonLazySrcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(nonLazyDstShader, nonLazySrcShader, BlendMode::SRC_OVER);
    ASSERT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());

    auto obj = ShaderImageFilterObj::Create(lazyShader, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::SHADER);

    // Compare with direct creation using non-lazy pipeline with blend shader
    // Create the same blend shader using non-lazy components
    auto firstDstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    ASSERT_NE(firstDstShader, nullptr);
    EXPECT_FALSE(firstDstShader->IsLazy());

    auto firstSrcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    ASSERT_NE(firstSrcShader, nullptr);
    EXPECT_FALSE(firstSrcShader->IsLazy());

    auto directBlendShader = ShaderEffect::CreateBlendShader(*firstDstShader, *firstSrcShader, BlendMode::SRC_OVER);
    ASSERT_NE(directBlendShader, nullptr);
    EXPECT_FALSE(directBlendShader->IsLazy());

    auto directFilter = ImageFilter::CreateShaderImageFilter(directBlendShader, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test ShaderImageFilterObj::GenerateBaseObject with non-lazy shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, GenerateBaseObject003, TestSize.Level1)
{
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    auto nonLazyShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    ASSERT_NE(nonLazyShader, nullptr);
    EXPECT_FALSE(nonLazyShader->IsLazy());

    auto obj = ShaderImageFilterObj::Create(nonLazyShader, cropRect);
    ASSERT_NE(obj, nullptr);
    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::SHADER);
    // Compare with direct creation using the same non-lazy shader
    auto directFilter = ImageFilter::CreateShaderImageFilter(nonLazyShader, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject004
 * @tc.desc: Test ShaderImageFilterObj::GenerateBaseObject consistency
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, GenerateBaseObject004, TestSize.Level1)
{
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    auto nonLazyShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto obj = ShaderImageFilterObj::Create(nonLazyShader, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter1 = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    auto generatedFilter2 = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter1, nullptr);
    ASSERT_NE(generatedFilter2, nullptr);

    // Multiple calls to GenerateBaseObject should produce equivalent results
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter1, generatedFilter2));
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: MarshallingUnmarshallingRoundTrip001
 * @tc.desc: Test ShaderImageFilterObj Marshalling/Unmarshalling with non-Lazy shader and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    auto originalShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    Rect cropRect(10.0f, 20.0f, 110.0f, 120.0f);
    auto originalObj = ShaderImageFilterObj::Create(originalShader, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(unmarshalledObj, nullptr);
    bool isValid = true;
    bool unmarshalResult = unmarshalledObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);

    // Memory validation, verify object equivalence
    auto originalFilter = std::static_pointer_cast<ImageFilter>(originalObj->GenerateBaseObject());
    auto unmarshalledFilter = std::static_pointer_cast<ImageFilter>(unmarshalledObj->GenerateBaseObject());
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(originalFilter, unmarshalledFilter));
}

/*
 * @tc.name: MarshallingUnmarshallingRoundTrip002
 * @tc.desc: Test ShaderImageFilterObj Marshalling/Unmarshalling with Lazy shader and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, MarshallingUnmarshallingRoundTrip002, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    Rect cropRect(5.0f, 15.0f, 105.0f, 115.0f);
    auto originalObj = ShaderImageFilterObj::Create(lazyShader, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(unmarshalledObj, nullptr);
    bool isValid = true;
    bool unmarshalResult = unmarshalledObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);

    // Memory validation, verify object equivalence
    auto originalFilter = std::static_pointer_cast<ImageFilter>(originalObj->GenerateBaseObject());
    auto unmarshalledFilter = std::static_pointer_cast<ImageFilter>(unmarshalledObj->GenerateBaseObject());
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(originalFilter, unmarshalledFilter));
}

/*
 * @tc.name: MarshallingUnmarshallingRoundTrip003
 * @tc.desc: Test ShaderImageFilterObj Marshalling/Unmarshalling with null shader and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, MarshallingUnmarshallingRoundTrip003, TestSize.Level1)
{
    Rect cropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto originalObj = ShaderImageFilterObj::Create(nullptr, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(unmarshalledObj, nullptr);
    bool isValid = true;
    bool unmarshalResult = unmarshalledObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);

    // Memory validation, verify object equivalence
    auto originalFilter = std::static_pointer_cast<ImageFilter>(originalObj->GenerateBaseObject());
    auto unmarshalledFilter = std::static_pointer_cast<ImageFilter>(unmarshalledObj->GenerateBaseObject());
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(originalFilter, unmarshalledFilter));
}

/*
 * @tc.name: MarshallingFailureTest
 * @tc.desc: Test ShaderImageFilterObj::Marshalling without DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, MarshallingFailureTest, TestSize.Level1)
{
    auto shader = ShaderEffect::CreateColorShader(0xFF0000FF);
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto obj = ShaderImageFilterObj::Create(shader, cropRect);
    EXPECT_TRUE(obj != nullptr);
    // Temporarily clear callback to test failure
    auto savedCallback = EffectTestUtils::ClearMarshallingCallback();

    MessageParcel parcel;
    bool result = obj->Marshalling(parcel);
    EXPECT_FALSE(result);

    // Restore callback
    EffectTestUtils::RestoreMarshallingCallback(savedCallback);
}

/*
 * @tc.name: MarshallingWriteFailureTest
 * @tc.desc: Test ShaderImageFilterObj::Marshalling write branch failures by filling parcel to capacity
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, MarshallingWriteFailureTest, TestSize.Level1)
{
    auto shader = ShaderEffect::CreateColorShader(0xFF0000FF);
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto obj = ShaderImageFilterObj::Create(shader, cropRect);
    ASSERT_NE(obj, nullptr);

    // Create buffer to fill parcel capacity (200K minimum)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Test 1: Fill parcel completely, then try Marshalling (should fail on first write)
    MessageParcel parcel1;
    parcel1.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult1 = parcel1.WriteBuffer(fillBuffer.data(), BUFFER_SIZE);
    EXPECT_TRUE(fillResult1);
    // Now parcel is full, Marshalling should fail on WriteFloat(left)
    bool result1 = obj->Marshalling(parcel1);
    EXPECT_FALSE(result1);

    // Test 2: Fill parcel leaving space for left only (4 bytes)
    MessageParcel parcel2;
    parcel2.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult2 = parcel2.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 4);
    EXPECT_TRUE(fillResult2);
    // Should fail on WriteFloat(top)
    bool result2 = obj->Marshalling(parcel2);
    EXPECT_FALSE(result2);

    // Test 3: Fill parcel leaving space for left, top only (8 bytes)
    MessageParcel parcel3;
    parcel3.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult3 = parcel3.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 8);
    EXPECT_TRUE(fillResult3);
    // Should fail on WriteFloat(right)
    bool result3 = obj->Marshalling(parcel3);
    EXPECT_FALSE(result3);

    // Test 4: Fill parcel leaving space for left, top, right only (12 bytes)
    MessageParcel parcel4;
    parcel4.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult4 = parcel4.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 12);
    EXPECT_TRUE(fillResult4);
    // Should fail on WriteFloat(bottom)
    bool result4 = obj->Marshalling(parcel4);
    EXPECT_FALSE(result4);

    // Test 5: Fill parcel leaving space for all floats only (16 bytes, no room for hasShader bool)
    MessageParcel parcel5;
    parcel5.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult5 = parcel5.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 16);
    EXPECT_TRUE(fillResult5);
    // Should fail on WriteBool(hasShader) - bool takes 4 bytes
    bool result5 = obj->Marshalling(parcel5);
    EXPECT_FALSE(result5);

    // Test 6: Fill parcel leaving space for all basic data but fail on WriteBool(isLazy)
    MessageParcel parcel6;
    parcel6.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult6 = parcel6.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 20);
    EXPECT_TRUE(fillResult6);
    // Should fail on WriteBool(isLazy) - bool takes 4 bytes
    bool result6 = obj->Marshalling(parcel6);
    EXPECT_FALSE(result6);
}

/*
 * @tc.name: UnmarshallingEdgeCaseTest
 * @tc.desc: Test ShaderImageFilterObj::Unmarshalling edge cases
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, UnmarshallingEdgeCaseTest, TestSize.Level1)
{
    // Test 1: Depth protection
    auto obj1 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj1, nullptr);
    MessageParcel parcel1;
    bool isValid1 = true;
    bool result1 = obj1->Unmarshalling(parcel1, isValid1, ObjectHelper::MAX_NESTING_DEPTH);
    EXPECT_FALSE(result1);

    // Test 2: Invalid nested lazy object unmarshalling failure
    MessageParcel parcel2;
    parcel2.WriteFloat(1.0f); // left
    parcel2.WriteFloat(2.0f); // top
    parcel2.WriteFloat(101.0f); // right
    parcel2.WriteFloat(102.0f); // bottom
    parcel2.WriteBool(true); // hasShader = true
    parcel2.WriteBool(true); // isLazy = true
    parcel2.WriteInt32(999); // Invalid type
    parcel2.WriteInt32(888); // Invalid subType
    auto obj2 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj2, nullptr);
    bool isValid2 = true;
    bool result2 = obj2->Unmarshalling(parcel2, isValid2);
    EXPECT_FALSE(result2);

    // Test 3: Invalid nested non-lazy object unmarshalling failure
    MessageParcel parcel3;
    parcel3.WriteFloat(1.0f); // left
    parcel3.WriteFloat(2.0f); // top
    parcel3.WriteFloat(101.0f); // right
    parcel3.WriteFloat(102.0f); // bottom
    parcel3.WriteBool(true); // hasShader = true
    parcel3.WriteBool(false); // isLazy = false
    // Missing ShaderEffect data will cause ShaderEffect::Unmarshalling to fail
    auto obj3 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel3, isValid3);
    EXPECT_FALSE(result3);
}

/*
 * @tc.name: UnmarshallingReadFailureTest
 * @tc.desc: Test ShaderImageFilterObj::Unmarshalling read failure scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderImageFilterObjTest, UnmarshallingReadFailureTest, TestSize.Level1)
{
    MessageParcel parcel;
    // Test 1: Failed to read left (empty parcel)
    auto obj1 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj1, nullptr);
    bool isValid1 = true;
    bool result1 = obj1->Unmarshalling(parcel, isValid1);
    EXPECT_FALSE(result1);

    // Test 2: Failed to read top (only left written)
    parcel.WriteFloat(10.0f); // left
    parcel.RewindRead(0);
    auto obj2 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj2, nullptr);
    bool isValid2 = true;
    bool result2 = obj2->Unmarshalling(parcel, isValid2);
    EXPECT_FALSE(result2);

    // Test 3: Failed to read right (left, top written, but right missing)
    parcel.WriteFloat(15.0f); // top
    parcel.RewindRead(0);
    auto obj3 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel, isValid3);
    EXPECT_FALSE(result3);

    // Test 4: Failed to read bottom (left, top, right written, but bottom missing)
    parcel.WriteFloat(110.0f); // right
    parcel.RewindRead(0);
    auto obj4 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj4, nullptr);
    bool isValid4 = true;
    bool result4 = obj4->Unmarshalling(parcel, isValid4);
    EXPECT_FALSE(result4);

    // Test 5: Failed to read hasShader flag (complete cropRect but missing hasShader)
    parcel.WriteFloat(120.0f); // bottom
    parcel.RewindRead(0);
    auto obj5 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj5, nullptr);
    bool isValid5 = true;
    bool result5 = obj5->Unmarshalling(parcel, isValid5);
    EXPECT_FALSE(result5);

    // Test 6: Failed to read isLazy flag (hasShader=true but missing isLazy)
    parcel.WriteBool(true); // hasShader = true
    parcel.RewindRead(0);
    auto obj6 = ShaderImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj6, nullptr);
    bool isValid6 = true;
    bool result6 = obj6->Unmarshalling(parcel, isValid6);
    EXPECT_FALSE(result6);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS