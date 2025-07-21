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
#include "effect/image_filter_lazy.h"
#include "effect/offset_image_filter_obj.h"
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
class OffsetImageFilterObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void OffsetImageFilterObjTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void OffsetImageFilterObjTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}

void OffsetImageFilterObjTest::SetUp() {}
void OffsetImageFilterObjTest::TearDown() {}

/*
 * @tc.name: CreateForUnmarshalling001
 * @tc.desc: Test OffsetImageFilterObj::CreateForUnmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, CreateForUnmarshalling001, TestSize.Level1)
{
    auto obj = OffsetImageFilterObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::OFFSET));
}

/*
 * @tc.name: Create001
 * @tc.desc: Test OffsetImageFilterObj::Create with different input types and scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, Create001, TestSize.Level1)
{
    // Test 1: Create with valid parameters
    scalar dx = 10.0f;
    scalar dy = 20.0f;
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto input = ImageFilter::CreateBlurImageFilter(5.0f, 5.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    EXPECT_TRUE(input != nullptr);
    auto obj = OffsetImageFilterObj::Create(dx, dy, input, cropRect);
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::OFFSET));

    // Test 2: Create with null input
    auto nullObj = OffsetImageFilterObj::Create(dx, dy, nullptr, cropRect);
    EXPECT_TRUE(nullObj != nullptr);
    EXPECT_EQ(nullObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(nullObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::OFFSET));

    // Test 3: Create with Lazy input
    Rect lazyBlurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto baseInput = ImageFilter::CreateBlurImageFilter(dx, dy, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::CLAMP, baseInput);
    auto lazyObj = OffsetImageFilterObj::Create(dx, dy, lazyInput, cropRect);
    EXPECT_TRUE(lazyObj != nullptr);
    EXPECT_EQ(lazyObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(lazyObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::OFFSET));
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test OffsetImageFilterObj::GenerateBaseObject with null input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, GenerateBaseObject001, TestSize.Level1)
{
    scalar dx = 10.0f;
    scalar dy = 15.0f;
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);
    auto obj = OffsetImageFilterObj::Create(dx, dy, nullptr, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::OFFSET);

    // Compare with direct creation using null input
    auto directFilter = ImageFilter::CreateOffsetImageFilter(dx, dy, nullptr, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test OffsetImageFilterObj::GenerateBaseObject with lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, GenerateBaseObject002, TestSize.Level1)
{
    scalar dx = 10.0f;
    scalar dy = 15.0f;
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto nonLazyBlurInput = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(2.0f, 3.0f, TileMode::DECAL, nonLazyBlurInput,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(lazyInput, nullptr);
    EXPECT_TRUE(lazyInput->IsLazy());

    auto obj = OffsetImageFilterObj::Create(dx, dy, lazyInput, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::OFFSET);

    // Compare with direct creation using non-lazy pipeline with double blur effects
    // First blur: same as nonLazyBlurInput above
    auto firstBlur = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(firstBlur, nullptr);
    EXPECT_FALSE(firstBlur->IsLazy());

    // Second blur: apply blur on top of first blur (matching lazy pipeline)
    auto secondBlur = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::DECAL, firstBlur,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(secondBlur, nullptr);
    EXPECT_FALSE(secondBlur->IsLazy());

    auto directFilter = ImageFilter::CreateOffsetImageFilter(dx, dy, secondBlur, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test OffsetImageFilterObj::GenerateBaseObject with non-lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, GenerateBaseObject003, TestSize.Level1)
{
    scalar dx = 10.0f;
    scalar dy = 15.0f;
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    Rect blurCropRect(0.0f, 0.0f, 80.0f, 80.0f);
    auto nonLazyInput = ImageFilter::CreateBlurImageFilter(4.0f, 5.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(nonLazyInput, nullptr);
    EXPECT_FALSE(nonLazyInput->IsLazy());

    auto obj = OffsetImageFilterObj::Create(dx, dy, nonLazyInput, cropRect);
    ASSERT_NE(obj, nullptr);
    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::OFFSET);
    // Compare with direct creation using the same non-lazy input
    auto directFilter = ImageFilter::CreateOffsetImageFilter(dx, dy, nonLazyInput, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject004
 * @tc.desc: Test OffsetImageFilterObj::GenerateBaseObject consistency
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, GenerateBaseObject004, TestSize.Level1)
{
    scalar dx = 10.0f;
    scalar dy = 15.0f;
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    auto nonLazyInput = ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    auto obj = OffsetImageFilterObj::Create(dx, dy, nonLazyInput, cropRect);
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
 * @tc.desc: Test OffsetImageFilterObj Marshalling/Unmarshalling with non-Lazy input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    scalar dx = 12.0f;
    scalar dy = 18.0f;
    Rect cropRect(10.0f, 20.0f, 110.0f, 120.0f);
    auto input = ImageFilter::CreateBlurImageFilter(3.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    auto originalObj = OffsetImageFilterObj::Create(dx, dy, input, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = OffsetImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test OffsetImageFilterObj Marshalling/Unmarshalling with Lazy input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, MarshallingUnmarshallingRoundTrip002, TestSize.Level1)
{
    scalar dx = 15.0f;
    scalar dy = 25.0f;
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto baseInput = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f,
        TileMode::CLAMP, nullptr, ImageBlurType::GAUSS, blurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(3.0f, 4.0f, TileMode::CLAMP, baseInput,
        ImageBlurType::GAUSS, blurCropRect);
    Rect cropRect(5.0f, 15.0f, 105.0f, 115.0f);
    auto originalObj = OffsetImageFilterObj::Create(dx, dy, lazyInput, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = OffsetImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test OffsetImageFilterObj Marshalling/Unmarshalling with null input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, MarshallingUnmarshallingRoundTrip003, TestSize.Level1)
{
    scalar dx = 8.0f;
    scalar dy = 12.0f;
    Rect cropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto originalObj = OffsetImageFilterObj::Create(dx, dy, nullptr, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = OffsetImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test OffsetImageFilterObj::Marshalling without DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, MarshallingFailureTest, TestSize.Level1)
{
    scalar dx = 10.0f;
    scalar dy = 20.0f;
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto input = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto obj = OffsetImageFilterObj::Create(dx, dy, input, cropRect);
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
 * @tc.desc: Test OffsetImageFilterObj::Marshalling write branch failures by filling parcel to capacity
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, MarshallingWriteFailureTest, TestSize.Level1)
{
    scalar dx = 10.0f;
    scalar dy = 20.0f;
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    Rect inputCropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto input = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, inputCropRect);
    auto obj = OffsetImageFilterObj::Create(dx, dy, input, cropRect);
    ASSERT_NE(obj, nullptr);

    // Create buffer to fill parcel capacity (200K minimum)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Test 1: Fill parcel completely, then try Marshalling (should fail on first write)
    MessageParcel parcel1;
    parcel1.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult1 = parcel1.WriteBuffer(fillBuffer.data(), BUFFER_SIZE);
    EXPECT_TRUE(fillResult1);
    // Now parcel is full, Marshalling should fail on WriteFloat(dx)
    bool result1 = obj->Marshalling(parcel1);
    EXPECT_FALSE(result1);

    // Test 2: Fill parcel leaving space for dx only (4 bytes)
    MessageParcel parcel2;
    parcel2.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult2 = parcel2.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 4);
    EXPECT_TRUE(fillResult2);
    // Should fail on WriteFloat(dy)
    bool result2 = obj->Marshalling(parcel2);
    EXPECT_FALSE(result2);

    // Test 3: Fill parcel leaving space for dx, dy only (8 bytes)
    MessageParcel parcel3;
    parcel3.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult3 = parcel3.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 8);
    EXPECT_TRUE(fillResult3);
    // Should fail on WriteFloat(cropRect.left)
    bool result3 = obj->Marshalling(parcel3);
    EXPECT_FALSE(result3);

    // Test 4: Fill parcel leaving space for dx, dy, left only (12 bytes)
    MessageParcel parcel4;
    parcel4.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult4 = parcel4.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 12);
    EXPECT_TRUE(fillResult4);
    // Should fail on WriteFloat(cropRect.top)
    bool result4 = obj->Marshalling(parcel4);
    EXPECT_FALSE(result4);

    // Test 5: Fill parcel leaving space for dx, dy, left, top only (16 bytes)
    MessageParcel parcel5;
    parcel5.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult5 = parcel5.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 16);
    EXPECT_TRUE(fillResult5);
    // Should fail on WriteFloat(cropRect.right)
    bool result5 = obj->Marshalling(parcel5);
    EXPECT_FALSE(result5);

    // Test 6: Fill parcel leaving space for dx, dy, left, top, right only (20 bytes)
    MessageParcel parcel6;
    parcel6.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult6 = parcel6.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 20);
    EXPECT_TRUE(fillResult6);
    // Should fail on WriteFloat(cropRect.bottom)
    bool result6 = obj->Marshalling(parcel6);
    EXPECT_FALSE(result6);

    // Test 7: Fill parcel leaving space for all floats only (24 bytes, no room for bool)
    MessageParcel parcel7;
    parcel7.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult7 = parcel7.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 24);
    EXPECT_TRUE(fillResult7);
    // Should fail on WriteBool(hasInput) - bool takes 4 bytes
    bool result7 = obj->Marshalling(parcel7);
    EXPECT_FALSE(result7);

    // Test 8: Fill parcel leaving space for all basic data but fail on WriteBool(input_->IsLazy())
    MessageParcel parcel8;
    parcel8.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult8 = parcel8.WriteBuffer(fillBuffer.data(),
        BUFFER_SIZE - 28); // 24 bytes + 4 bytes for hasInput bool
    EXPECT_TRUE(fillResult8);
    // Should fail on WriteBool(input_->IsLazy()) - bool takes 4 bytes
    bool result8 = obj->Marshalling(parcel8);
    EXPECT_FALSE(result8);
}

/*
 * @tc.name: UnmarshallingEdgeCaseTest
 * @tc.desc: Test OffsetImageFilterObj::Unmarshalling edge cases
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, UnmarshallingEdgeCaseTest, TestSize.Level1)
{
    // Test 1: Depth protection
    auto obj1 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj1, nullptr);
    MessageParcel parcel1;
    bool isValid1 = true;
    bool result1 = obj1->Unmarshalling(parcel1, isValid1, ObjectHelper::MAX_NESTING_DEPTH);
    EXPECT_FALSE(result1);

    // Test 2: Invalid nested lazy object unmarshalling failure
    MessageParcel parcel2;
    parcel2.WriteFloat(5.0f); // dx
    parcel2.WriteFloat(8.0f); // dy
    parcel2.WriteFloat(1.0f); // left
    parcel2.WriteFloat(2.0f); // top
    parcel2.WriteFloat(101.0f); // right
    parcel2.WriteFloat(102.0f); // bottom
    parcel2.WriteBool(true); // hasInput = true
    parcel2.WriteBool(true); // isLazy = true
    parcel2.WriteInt32(999); // Invalid type
    parcel2.WriteInt32(888); // Invalid subType
    auto obj2 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj2, nullptr);
    bool isValid2 = true;
    bool result2 = obj2->Unmarshalling(parcel2, isValid2);
    EXPECT_FALSE(result2);

    // Test 3: Invalid nested non-lazy object unmarshalling failure
    MessageParcel parcel3;
    parcel3.WriteFloat(5.0f); // dx
    parcel3.WriteFloat(8.0f); // dy
    parcel3.WriteFloat(1.0f); // left
    parcel3.WriteFloat(2.0f); // top
    parcel3.WriteFloat(101.0f); // right
    parcel3.WriteFloat(102.0f); // bottom
    parcel3.WriteBool(true); // hasInput = true
    parcel3.WriteBool(false); // isLazy = false
    // Missing ImageFilter data will cause ImageFilter::Unmarshalling to fail
    auto obj3 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel3, isValid3);
    EXPECT_FALSE(result3);
}

/*
 * @tc.name: UnmarshallingReadFailureTest
 * @tc.desc: Test OffsetImageFilterObj::Unmarshalling read failure scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(OffsetImageFilterObjTest, UnmarshallingReadFailureTest, TestSize.Level1)
{
    MessageParcel parcel;
    // Test 1: Failed to read dx (empty parcel)
    auto obj1 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj1, nullptr);
    bool isValid1 = true;
    bool result1 = obj1->Unmarshalling(parcel, isValid1);
    EXPECT_FALSE(result1);

    // Test 2: Failed to read dy (only dx written)
    parcel.WriteFloat(10.0f); // dx
    parcel.RewindRead(0);
    auto obj2 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj2, nullptr);
    bool isValid2 = true;
    bool result2 = obj2->Unmarshalling(parcel, isValid2);
    EXPECT_FALSE(result2);

    // Test 3: Failed to read cropRect left (dx, dy written, but left missing)
    parcel.WriteFloat(15.0f); // dy
    parcel.RewindRead(0);
    auto obj3 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel, isValid3);
    EXPECT_FALSE(result3);

    // Test 4: Failed to read cropRect top (dx, dy, left written, but top missing)
    parcel.WriteFloat(0.0f); // left
    parcel.RewindRead(0);
    auto obj4 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj4, nullptr);
    bool isValid4 = true;
    bool result4 = obj4->Unmarshalling(parcel, isValid4);
    EXPECT_FALSE(result4);

    // Test 5: Failed to read cropRect right (dx, dy, left, top written, but right missing)
    parcel.WriteFloat(0.0f); // top
    parcel.RewindRead(0);
    auto obj5 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj5, nullptr);
    bool isValid5 = true;
    bool result5 = obj5->Unmarshalling(parcel, isValid5);
    EXPECT_FALSE(result5);

    // Test 6: Failed to read cropRect bottom (dx, dy, left, top, right written, but bottom missing)
    parcel.WriteFloat(100.0f); // right
    parcel.RewindRead(0);
    auto obj6 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj6, nullptr);
    bool isValid6 = true;
    bool result6 = obj6->Unmarshalling(parcel, isValid6);
    EXPECT_FALSE(result6);

    // Test 7: Failed to read hasInput flag (complete cropRect but missing hasInput)
    parcel.WriteFloat(100.0f); // bottom
    parcel.RewindRead(0);
    auto obj7 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj7, nullptr);
    bool isValid7 = true;
    bool result7 = obj7->Unmarshalling(parcel, isValid7);
    EXPECT_FALSE(result7);

    // Test 8: Failed to read isLazy flag (hasInput=true but missing isLazy)
    parcel.WriteBool(true); // hasInput = true
    parcel.RewindRead(0);
    auto obj8 = OffsetImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj8, nullptr);
    bool isValid8 = true;
    bool result8 = obj8->Unmarshalling(parcel, isValid8);
    EXPECT_FALSE(result8);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS