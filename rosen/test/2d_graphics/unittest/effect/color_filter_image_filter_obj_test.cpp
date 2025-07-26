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

#include "effect/color_filter_image_filter_obj.h"
#include "effect/color_filter.h"
#include "effect/image_filter.h"
#include "effect/image_filter_lazy.h"
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
class ColorFilterImageFilterObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ColorFilterImageFilterObjTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void ColorFilterImageFilterObjTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}

void ColorFilterImageFilterObjTest::SetUp() {}
void ColorFilterImageFilterObjTest::TearDown() {}

/*
 * @tc.name: CreateForUnmarshalling001
 * @tc.desc: Test ColorFilterImageFilterObj::CreateForUnmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, CreateForUnmarshalling001, TestSize.Level1)
{
    auto obj = ColorFilterImageFilterObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::COLOR_FILTER));
}

/*
 * @tc.name: Create001
 * @tc.desc: Test ColorFilterImageFilterObj::Create with different input types and scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, Create001, TestSize.Level1)
{
    // Test 1: Create with valid parameters
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF0000FF, BlendMode::SRC_OVER);
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto input = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    Rect cropRect(10.0f, 20.0f, 110.0f, 120.0f);
    EXPECT_TRUE(input != nullptr);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::COLOR_FILTER));

    // Test 2: Create with null input
    auto nullObj = ColorFilterImageFilterObj::Create(colorFilter, nullptr, cropRect);
    EXPECT_TRUE(nullObj != nullptr);
    EXPECT_EQ(nullObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(nullObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::COLOR_FILTER));

    // Test 3: Create with Lazy input
    Rect lazyBlurCropRect(0.0f, 0.0f, 80.0f, 80.0f);
    auto baseInput = ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, lazyBlurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(3.0f, 4.0f, TileMode::REPEAT, baseInput);
    auto lazyObj = ColorFilterImageFilterObj::Create(colorFilter, lazyInput, cropRect);
    EXPECT_TRUE(lazyObj != nullptr);
    EXPECT_EQ(lazyObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(lazyObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::COLOR_FILTER));
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test ColorFilterImageFilterObj::GenerateBaseObject with null input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, GenerateBaseObject001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF00FF00, BlendMode::MULTIPLY);
    Rect cropRect(5.0f, 5.0f, 95.0f, 95.0f);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, nullptr, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::COLOR_FILTER);

    // Compare with direct creation using null input
    auto directFilter = ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test ColorFilterImageFilterObj::GenerateBaseObject with lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, GenerateBaseObject002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, BlendMode::SCREEN);
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto nonLazyBlurInput = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(2.0f, 3.0f, TileMode::MIRROR, nonLazyBlurInput,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(lazyInput, nullptr);
    EXPECT_TRUE(lazyInput->IsLazy());

    auto obj = ColorFilterImageFilterObj::Create(colorFilter, lazyInput, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::COLOR_FILTER);

    // Compare with direct creation using non-lazy pipeline with double blur effects
    // First blur: same as nonLazyBlurInput above
    auto firstBlur = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(firstBlur, nullptr);
    EXPECT_FALSE(firstBlur->IsLazy());

    // Second blur: apply blur on top of first blur (matching lazy pipeline)
    auto secondBlur = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::MIRROR, firstBlur,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(secondBlur, nullptr);
    EXPECT_FALSE(secondBlur->IsLazy());

    auto directFilter = ImageFilter::CreateColorFilterImageFilter(*colorFilter, secondBlur, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test ColorFilterImageFilterObj::GenerateBaseObject with non-lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, GenerateBaseObject003, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF888888, BlendMode::OVERLAY);
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    Rect blurCropRect(0.0f, 0.0f, 80.0f, 80.0f);
    auto nonLazyInput = ImageFilter::CreateBlurImageFilter(1.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(nonLazyInput, nullptr);
    EXPECT_FALSE(nonLazyInput->IsLazy());

    auto obj = ColorFilterImageFilterObj::Create(colorFilter, nonLazyInput, cropRect);
    ASSERT_NE(obj, nullptr);
    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::COLOR_FILTER);
    // Compare with direct creation using the same non-lazy input
    auto directFilter = ImageFilter::CreateColorFilterImageFilter(*colorFilter, nonLazyInput, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject004
 * @tc.desc: Test ColorFilterImageFilterObj::GenerateBaseObject consistency
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, GenerateBaseObject004, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF654321, BlendMode::LIGHTEN);
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    auto nonLazyInput = ImageFilter::CreateBlurImageFilter(1.5f, 2.5f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, nonLazyInput, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter1 = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    auto generatedFilter2 = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter1, nullptr);
    ASSERT_NE(generatedFilter2, nullptr);

    // Multiple calls to GenerateBaseObject should produce equivalent results
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter1, generatedFilter2));
}

/*
 * @tc.name: NullColorFilterTest001
 * @tc.desc: Test ColorFilterImageFilterObj::Create with null colorFilter and Marshalling behavior
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, NullColorFilterTest001, TestSize.Level1)
{
    // Test Create with null colorFilter
    Rect cropRect(5.0f, 10.0f, 105.0f, 110.0f);
    auto obj = ColorFilterImageFilterObj::Create(nullptr, nullptr, cropRect);
    ASSERT_NE(obj, nullptr);

    // Test GenerateBaseObject with null colorFilter - should return nullptr
    auto generatedFilter = obj->GenerateBaseObject();
    EXPECT_EQ(generatedFilter, nullptr);

#ifdef ROSEN_OHOS
    // Test Marshalling with null colorFilter - should fail
    MessageParcel parcel;
    bool marshalResult = obj->Marshalling(parcel);
    EXPECT_FALSE(marshalResult);
#endif
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: MarshallingUnmarshallingRoundTrip001
 * @tc.desc: Test ColorFilterImageFilterObj Marshalling/Unmarshalling with non-Lazy input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF0000FF, BlendMode::SRC_OVER);
    Rect cropRect(10.0f, 20.0f, 110.0f, 120.0f);
    auto input = ImageFilter::CreateBlurImageFilter(3.0f, 4.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    auto originalObj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = ColorFilterImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test ColorFilterImageFilterObj Marshalling/Unmarshalling with Lazy input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingUnmarshallingRoundTrip002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF00FF00, BlendMode::MULTIPLY);
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto baseInput = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f,
        TileMode::CLAMP, nullptr, ImageBlurType::GAUSS, blurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(4.0f, 5.0f, TileMode::CLAMP, baseInput,
        ImageBlurType::GAUSS, blurCropRect);
    Rect cropRect(5.0f, 15.0f, 105.0f, 115.0f);
    auto originalObj = ColorFilterImageFilterObj::Create(colorFilter, lazyInput, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = ColorFilterImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test ColorFilterImageFilterObj Marshalling/Unmarshalling with null input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingUnmarshallingRoundTrip003, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, BlendMode::SCREEN);
    Rect cropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto originalObj = ColorFilterImageFilterObj::Create(colorFilter, nullptr, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = ColorFilterImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test ColorFilterImageFilterObj::Marshalling without DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingFailureTest, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF888888, BlendMode::OVERLAY);
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto input = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
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
 * @tc.name: MarshallingNullColorFilterTest
 * @tc.desc: Test ColorFilterImageFilterObj::Marshalling with null colorFilter
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingNullColorFilterTest, TestSize.Level1)
{
    // Create obj with null colorFilter - should fail marshalling
    auto obj = ColorFilterImageFilterObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);

    MessageParcel parcel;
    bool result = obj->Marshalling(parcel);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: MarshallingWriteFailureTest
 * @tc.desc: Test ColorFilterImageFilterObj::Marshalling write branch failures by filling parcel to capacity
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingWriteFailureTest, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF123456, BlendMode::DARKEN);
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    Rect inputCropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto input = ImageFilter::CreateBlurImageFilter(1.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, inputCropRect);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
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

    // Test 5: Fill parcel leaving space for all floats only (16 bytes, no room for ColorFilter)
    MessageParcel parcel5;
    parcel5.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult5 = parcel5.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 16);
    EXPECT_TRUE(fillResult5);
    // Should fail on ColorFilter::Marshalling
    bool result5 = obj->Marshalling(parcel5);
    EXPECT_FALSE(result5);
}

/*
 * @tc.name: MarshallingWriteFailureTest002
 * @tc.desc: Test ColorFilterImageFilterObj::Marshalling WriteBool(hasInput)
 * failure by calculating exact ColorFilter size
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingWriteFailureTest002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF654321, BlendMode::LIGHTEN);
    Rect cropRect(0.0f, 0.0f, 50.0f, 50.0f);
    Rect inputCropRect(0.0f, 0.0f, 30.0f, 30.0f);
    auto input = ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, inputCropRect);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
    ASSERT_NE(obj, nullptr);

    // First, calculate the exact size needed for cropRect + ColorFilter marshalling
    MessageParcel tempParcel;
    tempParcel.WriteFloat(cropRect.GetLeft());   // 4 bytes
    tempParcel.WriteFloat(cropRect.GetTop());    // 4 bytes
    tempParcel.WriteFloat(cropRect.GetRight());  // 4 bytes
    tempParcel.WriteFloat(cropRect.GetBottom()); // 4 bytes
    colorFilter->Marshalling(tempParcel);        // Variable size
    size_t sizeAfterColorFilter = tempParcel.GetDataSize();

    // Use 200K buffer requirement and calculate precise remaining space
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    MessageParcel parcel;
    parcel.SetMaxCapacity(BUFFER_SIZE);

    // Fill parcel leaving space for cropRect + ColorFilter but not enough for WriteBool(hasInput) (4 bytes)
    // Leave space = sizeAfterColorFilter + 2 (not enough for 4-byte bool)
    const size_t REMAINING_SPACE = sizeAfterColorFilter + 2;
    const size_t FILL_SIZE = BUFFER_SIZE - REMAINING_SPACE;
    std::vector<uint8_t> fillBuffer(FILL_SIZE, 0xBB);
    bool fillResult = parcel.WriteBuffer(fillBuffer.data(), FILL_SIZE);
    EXPECT_TRUE(fillResult);

    // Should fail on WriteBool(hasInput) due to insufficient space (needs 4 bytes but only 2 available)
    bool result = obj->Marshalling(parcel);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: MarshallingWriteFailureTest003
 * @tc.desc: Test ColorFilterImageFilterObj::Marshalling input->Marshalling failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingWriteFailureTest003, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF789ABC, BlendMode::COLOR_BURN);
    Rect cropRect(0.0f, 0.0f, 60.0f, 60.0f);
    Rect inputCropRect(0.0f, 0.0f, 40.0f, 40.0f);
    auto input = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, inputCropRect);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
    ASSERT_NE(obj, nullptr);

    // Calculate the exact size needed for cropRect + ColorFilter + WriteBool(hasInput) + WriteBool(isLazy)
    MessageParcel tempParcel;
    tempParcel.WriteFloat(cropRect.GetLeft());   // 4 bytes
    tempParcel.WriteFloat(cropRect.GetTop());    // 4 bytes
    tempParcel.WriteFloat(cropRect.GetRight());  // 4 bytes
    tempParcel.WriteFloat(cropRect.GetBottom()); // 4 bytes
    colorFilter->Marshalling(tempParcel);        // Variable size
    tempParcel.WriteBool(true);                  // hasInput - 4 bytes
    tempParcel.WriteBool(false);                 // isLazy - 4 bytes
    size_t sizeBeforeInputMarshalling = tempParcel.GetDataSize();

    // Use 200K buffer and leave insufficient space for input->Marshalling
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    MessageParcel parcel;
    parcel.SetMaxCapacity(BUFFER_SIZE);

    // Fill parcel leaving space for everything except input marshalling (leave only 10 bytes)
    const size_t REMAINING_SPACE = sizeBeforeInputMarshalling + 10; // Not enough for input marshalling
    const size_t FILL_SIZE = BUFFER_SIZE - REMAINING_SPACE;
    std::vector<uint8_t> fillBuffer(FILL_SIZE, 0xDD);
    bool fillResult = parcel.WriteBuffer(fillBuffer.data(), FILL_SIZE);
    EXPECT_TRUE(fillResult);

    // Should fail on input->Marshalling due to insufficient space
    bool result = obj->Marshalling(parcel);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: MarshallingWriteFailureTest004
 * @tc.desc: Test ColorFilterImageFilterObj::Marshalling WriteBool(isLazy) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, MarshallingWriteFailureTest004, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFFABCDEF, BlendMode::SOFT_LIGHT);
    Rect cropRect(0.0f, 0.0f, 70.0f, 70.0f);
    Rect inputCropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto input = ImageFilter::CreateBlurImageFilter(3.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, inputCropRect);
    auto obj = ColorFilterImageFilterObj::Create(colorFilter, input, cropRect);
    ASSERT_NE(obj, nullptr);

    // Calculate the exact size needed for cropRect + ColorFilter + WriteBool(hasInput)
    MessageParcel tempParcel;
    tempParcel.WriteFloat(cropRect.GetLeft());   // 4 bytes
    tempParcel.WriteFloat(cropRect.GetTop());    // 4 bytes
    tempParcel.WriteFloat(cropRect.GetRight());  // 4 bytes
    tempParcel.WriteFloat(cropRect.GetBottom()); // 4 bytes
    colorFilter->Marshalling(tempParcel);        // Variable size
    tempParcel.WriteBool(true);                  // hasInput - 4 bytes
    size_t sizeBeforeIsLazy = tempParcel.GetDataSize();

    // Use 200K buffer and leave insufficient space for WriteBool(isLazy) (4 bytes)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    MessageParcel parcel;
    parcel.SetMaxCapacity(BUFFER_SIZE);

    // Fill parcel leaving space for everything before WriteBool(isLazy) but not enough for isLazy (4 bytes)
    // Leave only 2 bytes - not enough for 4-byte bool
    const size_t REMAINING_SPACE = sizeBeforeIsLazy + 2;
    const size_t FILL_SIZE = BUFFER_SIZE - REMAINING_SPACE;
    std::vector<uint8_t> fillBuffer(FILL_SIZE, 0xEE);
    bool fillResult = parcel.WriteBuffer(fillBuffer.data(), FILL_SIZE);
    EXPECT_TRUE(fillResult);

    // Should fail on WriteBool(isLazy) due to insufficient space (needs 4 bytes but only 2 available)
    bool result = obj->Marshalling(parcel);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: UnmarshallingEdgeCaseTest
 * @tc.desc: Test ColorFilterImageFilterObj::Unmarshalling edge cases
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, UnmarshallingEdgeCaseTest, TestSize.Level1)
{
    // Test 1: Depth protection
    auto obj1 = ColorFilterImageFilterObj::CreateForUnmarshalling();
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
    // ColorFilter::Unmarshalling will fail due to invalid/incomplete data
    parcel2.WriteInt32(static_cast<int32_t>(ColorFilter::FilterType::BLEND_MODE)); // type
    parcel2.WriteBool(false); // hasColorFilterData = false, invalid ColorFilter
    parcel2.WriteBool(true); // hasInput = true
    parcel2.WriteBool(true); // isLazy = true
    parcel2.WriteInt32(999); // Invalid type
    parcel2.WriteInt32(888); // Invalid subType
    auto obj2 = ColorFilterImageFilterObj::CreateForUnmarshalling();
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
    parcel3.WriteInt32(static_cast<int32_t>(ColorFilter::FilterType::BLEND_MODE)); // type
    parcel3.WriteBool(false); // hasColorFilterData = false, invalid ColorFilter
    parcel3.WriteBool(true); // hasInput = true
    parcel3.WriteBool(false); // isLazy = false
    // Missing ImageFilter data will cause ImageFilter::Unmarshalling to fail
    auto obj3 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel3, isValid3);
    EXPECT_FALSE(result3);
}

/*
 * @tc.name: UnmarshallingReadFailureTest
 * @tc.desc: Test ColorFilterImageFilterObj::Unmarshalling read failure scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterImageFilterObjTest, UnmarshallingReadFailureTest, TestSize.Level1)
{
    MessageParcel parcel;
    // Test 1: Failed to read left (empty parcel)
    auto obj1 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj1, nullptr);
    bool isValid1 = true;
    bool result1 = obj1->Unmarshalling(parcel, isValid1);
    EXPECT_FALSE(result1);

    // Test 2: Failed to read top (only left written)
    parcel.WriteFloat(10.0f); // left
    parcel.RewindRead(0);
    auto obj2 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj2, nullptr);
    bool isValid2 = true;
    bool result2 = obj2->Unmarshalling(parcel, isValid2);
    EXPECT_FALSE(result2);

    // Test 3: Failed to read right (left, top written, but right missing)
    parcel.WriteFloat(15.0f); // top
    parcel.RewindRead(0);
    auto obj3 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel, isValid3);
    EXPECT_FALSE(result3);

    // Test 4: Failed to read bottom (left, top, right written, but bottom missing)
    parcel.WriteFloat(110.0f); // right
    parcel.RewindRead(0);
    auto obj4 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj4, nullptr);
    bool isValid4 = true;
    bool result4 = obj4->Unmarshalling(parcel, isValid4);
    EXPECT_FALSE(result4);

    // Test 5: Failed to read ColorFilter (complete cropRect but ColorFilter::Unmarshalling fails)
    parcel.WriteFloat(120.0f); // bottom
    parcel.RewindRead(0);
    auto obj5 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj5, nullptr);
    bool isValid5 = true;
    bool result5 = obj5->Unmarshalling(parcel, isValid5);
    EXPECT_FALSE(result5);

    // Test 6: Failed to read hasInput flag (cropRect + ColorFilter but missing hasInput)
    parcel.WriteInt32(static_cast<int32_t>(ColorFilter::FilterType::BLEND_MODE)); // type
    parcel.WriteBool(false); // hasColorFilterData = false, null ColorFilter
    parcel.RewindRead(0);
    auto obj6 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj6, nullptr);
    bool isValid6 = true;
    bool result6 = obj6->Unmarshalling(parcel, isValid6);
    EXPECT_FALSE(result6);

    // Test 7: Failed to read isLazy flag (hasInput=true but missing isLazy)
    parcel.WriteBool(true); // hasInput = true
    parcel.RewindRead(0);
    auto obj7 = ColorFilterImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj7, nullptr);
    bool isValid7 = true;
    bool result7 = obj7->Unmarshalling(parcel, isValid7);
    EXPECT_FALSE(result7);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS