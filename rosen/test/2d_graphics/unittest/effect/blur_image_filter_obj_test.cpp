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

#include "effect/blur_image_filter_obj.h"
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
class BlurImageFilterObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BlurImageFilterObjTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void BlurImageFilterObjTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}

void BlurImageFilterObjTest::SetUp() {}
void BlurImageFilterObjTest::TearDown() {}

/*
 * @tc.name: CreateForUnmarshalling001
 * @tc.desc: Test BlurImageFilterObj::CreateForUnmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, CreateForUnmarshalling001, TestSize.Level1)
{
    auto obj = BlurImageFilterObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::BLUR));
}

/*
 * @tc.name: Create001
 * @tc.desc: Test BlurImageFilterObj::Create with different input types and scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, Create001, TestSize.Level1)
{
    // Test 1: Create with valid parameters
    scalar sigmaX = 5.0f;
    scalar sigmaY = 7.0f;
    TileMode tileMode = TileMode::CLAMP;
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto input = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    EXPECT_TRUE(input != nullptr);
    auto obj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, input,
        ImageBlurType::GAUSS, cropRect);
    EXPECT_TRUE(obj != nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::BLUR));

    // Test 2: Create with null input
    auto nullObj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, nullptr,
        ImageBlurType::GAUSS, cropRect);
    EXPECT_TRUE(nullObj != nullptr);
    EXPECT_EQ(nullObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(nullObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::BLUR));

    // Test 3: Create with Lazy input
    Rect lazyBlurCropRect(0.0f, 0.0f, 80.0f, 80.0f);
    auto baseInput = ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, lazyBlurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(3.0f, 4.0f, TileMode::REPEAT, baseInput);
    auto lazyObj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, lazyInput,
        ImageBlurType::GAUSS, cropRect);
    EXPECT_TRUE(lazyObj != nullptr);
    EXPECT_EQ(lazyObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(lazyObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::BLUR));
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test BlurImageFilterObj::GenerateBaseObject with null input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, GenerateBaseObject001, TestSize.Level1)
{
    scalar sigmaX = 6.0f;
    scalar sigmaY = 8.0f;
    TileMode tileMode = TileMode::REPEAT;
    Rect cropRect(5.0f, 5.0f, 95.0f, 95.0f);
    auto obj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::BLUR);

    // Compare with direct creation using null input
    auto directFilter = ImageFilter::CreateBlurImageFilter(sigmaX, sigmaY, tileMode, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test BlurImageFilterObj::GenerateBaseObject with lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, GenerateBaseObject002, TestSize.Level1)
{
    scalar sigmaX = 4.0f;
    scalar sigmaY = 6.0f;
    TileMode tileMode = TileMode::CLAMP;
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);

    auto nonLazyBlurInput = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(2.0f, 3.0f, TileMode::MIRROR, nonLazyBlurInput,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(lazyInput, nullptr);
    EXPECT_TRUE(lazyInput->IsLazy());

    auto obj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, lazyInput,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(obj, nullptr);

    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::BLUR);

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

    auto directFilter = ImageFilter::CreateBlurImageFilter(sigmaX, sigmaY, tileMode, secondBlur,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test BlurImageFilterObj::GenerateBaseObject with non-lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, GenerateBaseObject003, TestSize.Level1)
{
    scalar sigmaX = 3.0f;
    scalar sigmaY = 5.0f;
    TileMode tileMode = TileMode::DECAL;
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    Rect blurCropRect(0.0f, 0.0f, 80.0f, 80.0f);
    auto nonLazyInput = ImageFilter::CreateBlurImageFilter(1.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(nonLazyInput, nullptr);
    EXPECT_FALSE(nonLazyInput->IsLazy());

    auto obj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, nonLazyInput,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(obj, nullptr);
    auto generatedFilter = std::static_pointer_cast<ImageFilter>(obj->GenerateBaseObject());
    ASSERT_NE(generatedFilter, nullptr);
    EXPECT_EQ(generatedFilter->GetType(), ImageFilter::FilterType::BLUR);
    // Compare with direct creation using the same non-lazy input
    auto directFilter = ImageFilter::CreateBlurImageFilter(sigmaX, sigmaY, tileMode, nonLazyInput,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(directFilter, nullptr);
    EXPECT_TRUE(EffectTestUtils::CompareImageFilter(generatedFilter, directFilter));
}

/*
 * @tc.name: GenerateBaseObject004
 * @tc.desc: Test BlurImageFilterObj::GenerateBaseObject consistency
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, GenerateBaseObject004, TestSize.Level1)
{
    scalar sigmaX = 7.0f;
    scalar sigmaY = 9.0f;
    TileMode tileMode = TileMode::REPEAT;
    Rect cropRect(5.0f, 5.0f, 105.0f, 105.0f);

    auto nonLazyInput = ImageFilter::CreateBlurImageFilter(1.5f, 2.5f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    auto obj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, nonLazyInput,
        ImageBlurType::GAUSS, cropRect);
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
 * @tc.desc: Test BlurImageFilterObj Marshalling/Unmarshalling with non-Lazy input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    scalar sigmaX = 8.0f;
    scalar sigmaY = 12.0f;
    TileMode tileMode = TileMode::CLAMP;
    Rect cropRect(10.0f, 20.0f, 110.0f, 120.0f);
    auto input = ImageFilter::CreateBlurImageFilter(3.0f, 4.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    auto originalObj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, input,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = BlurImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test BlurImageFilterObj Marshalling/Unmarshalling with Lazy input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, MarshallingUnmarshallingRoundTrip002, TestSize.Level1)
{
    scalar sigmaX = 6.0f;
    scalar sigmaY = 10.0f;
    TileMode tileMode = TileMode::REPEAT;
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto baseInput = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f,
        TileMode::CLAMP, nullptr, ImageBlurType::GAUSS, blurCropRect);
    auto lazyInput = ImageFilterLazy::CreateBlur(4.0f, 5.0f, TileMode::CLAMP, baseInput,
        ImageBlurType::GAUSS, blurCropRect);
    Rect cropRect(5.0f, 15.0f, 105.0f, 115.0f);
    auto originalObj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, lazyInput,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = BlurImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test BlurImageFilterObj Marshalling/Unmarshalling with null input and memory validation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, MarshallingUnmarshallingRoundTrip003, TestSize.Level1)
{
    scalar sigmaX = 5.0f;
    scalar sigmaY = 7.0f;
    TileMode tileMode = TileMode::MIRROR;
    Rect cropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto originalObj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(originalObj, nullptr);

    // Test Marshalling
    MessageParcel parcel;
    bool marshalResult = originalObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    auto unmarshalledObj = BlurImageFilterObj::CreateForUnmarshalling();
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
 * @tc.desc: Test BlurImageFilterObj::Marshalling without DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, MarshallingFailureTest, TestSize.Level1)
{
    scalar sigmaX = 4.0f;
    scalar sigmaY = 6.0f;
    TileMode tileMode = TileMode::DECAL;
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto input = ImageFilter::CreateBlurImageFilter(2.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto obj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, input,
        ImageBlurType::GAUSS, cropRect);
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
 * @tc.desc: Test BlurImageFilterObj::Marshalling write branch failures by filling parcel to capacity
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, MarshallingWriteFailureTest, TestSize.Level1)
{
    scalar sigmaX = 3.0f;
    scalar sigmaY = 5.0f;
    TileMode tileMode = TileMode::CLAMP;
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    Rect inputCropRect(0.0f, 0.0f, 50.0f, 50.0f);
    auto input = ImageFilter::CreateBlurImageFilter(1.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, inputCropRect);
    auto obj = BlurImageFilterObj::Create(sigmaX, sigmaY, tileMode, input,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(obj, nullptr);

    // Create buffer to fill parcel capacity (200K minimum)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Test 1: Fill parcel completely, then try Marshalling (should fail on first write)
    MessageParcel parcel1;
    parcel1.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult1 = parcel1.WriteBuffer(fillBuffer.data(), BUFFER_SIZE);
    EXPECT_TRUE(fillResult1);
    // Now parcel is full, Marshalling should fail on WriteFloat(sigmaX)
    bool result1 = obj->Marshalling(parcel1);
    EXPECT_FALSE(result1);

    // Test 2: Fill parcel leaving space for sigmaX only (4 bytes)
    MessageParcel parcel2;
    parcel2.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult2 = parcel2.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 4);
    EXPECT_TRUE(fillResult2);
    // Should fail on WriteFloat(sigmaY)
    bool result2 = obj->Marshalling(parcel2);
    EXPECT_FALSE(result2);

    // Test 3: Fill parcel leaving space for sigmaX, sigmaY only (8 bytes)
    MessageParcel parcel3;
    parcel3.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult3 = parcel3.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 8);
    EXPECT_TRUE(fillResult3);
    // Should fail on WriteInt32(tileMode)
    bool result3 = obj->Marshalling(parcel3);
    EXPECT_FALSE(result3);

    // Test 4: Fill parcel leaving space for sigmaX, sigmaY, tileMode only (12 bytes)
    MessageParcel parcel4;
    parcel4.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult4 = parcel4.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 12);
    EXPECT_TRUE(fillResult4);
    // Should fail on WriteInt32(blurType)
    bool result4 = obj->Marshalling(parcel4);
    EXPECT_FALSE(result4);

    // Test 5: Fill parcel leaving space for basic parameters only (16 bytes)
    MessageParcel parcel5;
    parcel5.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult5 = parcel5.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 16);
    EXPECT_TRUE(fillResult5);
    // Should fail on WriteFloat(cropRect.left)
    bool result5 = obj->Marshalling(parcel5);
    EXPECT_FALSE(result5);

    // Test 6: Fill parcel leaving space for basic + one cropRect component (20 bytes)
    MessageParcel parcel6;
    parcel6.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult6 = parcel6.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 20);
    EXPECT_TRUE(fillResult6);
    // Should fail on WriteFloat(cropRect.top)
    bool result6 = obj->Marshalling(parcel6);
    EXPECT_FALSE(result6);

    // Test 7: Fill parcel leaving space for basic + cropRect left, top (24 bytes)
    MessageParcel parcel7;
    parcel7.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult7 = parcel7.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 24);
    EXPECT_TRUE(fillResult7);
    // Should fail on WriteFloat(cropRect.right)
    bool result7 = obj->Marshalling(parcel7);
    EXPECT_FALSE(result7);

    // Test 8: Fill parcel leaving space for basic + cropRect left, top, right (28 bytes)
    MessageParcel parcel8;
    parcel8.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult8 = parcel8.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 28);
    EXPECT_TRUE(fillResult8);
    // Should fail on WriteFloat(cropRect.bottom)
    bool result8 = obj->Marshalling(parcel8);
    EXPECT_FALSE(result8);

    // Test 9: Fill parcel leaving space for all floats only (32 bytes, no room for hasInput bool)
    MessageParcel parcel9;
    parcel9.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult9 = parcel9.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 32);
    EXPECT_TRUE(fillResult9);
    // Should fail on WriteBool(hasInput) - bool takes 4 bytes
    bool result9 = obj->Marshalling(parcel9);
    EXPECT_FALSE(result9);

    // Test 10: Fill parcel leaving space for all basic data but fail on WriteBool(isLazy)
    MessageParcel parcel10;
    parcel10.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult10 = parcel10.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 36);
    EXPECT_TRUE(fillResult10);
    // Should fail on WriteBool(isLazy) - bool takes 4 bytes
    bool result10 = obj->Marshalling(parcel10);
    EXPECT_FALSE(result10);
}

/*
 * @tc.name: UnmarshallingEdgeCaseTest
 * @tc.desc: Test BlurImageFilterObj::Unmarshalling edge cases
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, UnmarshallingEdgeCaseTest, TestSize.Level1)
{
    // Test 1: Depth protection
    auto obj1 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj1, nullptr);
    MessageParcel parcel1;
    bool isValid1 = true;
    bool result1 = obj1->Unmarshalling(parcel1, isValid1, ObjectHelper::MAX_NESTING_DEPTH);
    EXPECT_FALSE(result1);

    // Test 2: Invalid nested lazy object unmarshalling failure
    MessageParcel parcel2;
    parcel2.WriteFloat(3.0f); // sigmaX
    parcel2.WriteFloat(5.0f); // sigmaY
    parcel2.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // tileMode
    parcel2.WriteInt32(static_cast<int32_t>(ImageBlurType::GAUSS)); // blurType
    parcel2.WriteFloat(1.0f); // left
    parcel2.WriteFloat(2.0f); // top
    parcel2.WriteFloat(101.0f); // right
    parcel2.WriteFloat(102.0f); // bottom
    parcel2.WriteBool(true); // hasInput = true
    parcel2.WriteBool(true); // isLazy = true
    parcel2.WriteInt32(999); // Invalid type
    parcel2.WriteInt32(888); // Invalid subType
    auto obj2 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj2, nullptr);
    bool isValid2 = true;
    bool result2 = obj2->Unmarshalling(parcel2, isValid2);
    EXPECT_FALSE(result2);

    // Test 3: Invalid nested non-lazy object unmarshalling failure
    MessageParcel parcel3;
    parcel3.WriteFloat(4.0f); // sigmaX
    parcel3.WriteFloat(6.0f); // sigmaY
    parcel3.WriteInt32(static_cast<int32_t>(TileMode::REPEAT)); // tileMode
    parcel3.WriteInt32(static_cast<int32_t>(ImageBlurType::GAUSS)); // blurType
    parcel3.WriteFloat(1.0f); // left
    parcel3.WriteFloat(2.0f); // top
    parcel3.WriteFloat(101.0f); // right
    parcel3.WriteFloat(102.0f); // bottom
    parcel3.WriteBool(true); // hasInput = true
    parcel3.WriteBool(false); // isLazy = false
    // Missing ImageFilter data will cause ImageFilter::Unmarshalling to fail
    auto obj3 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel3, isValid3);
    EXPECT_FALSE(result3);
}

/*
 * @tc.name: UnmarshallingReadFailureTest
 * @tc.desc: Test BlurImageFilterObj::Unmarshalling read failure scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlurImageFilterObjTest, UnmarshallingReadFailureTest, TestSize.Level1)
{
    MessageParcel parcel;
    // Test 1: Failed to read sigmaX (empty parcel)
    auto obj1 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj1, nullptr);
    bool isValid1 = true;
    bool result1 = obj1->Unmarshalling(parcel, isValid1);
    EXPECT_FALSE(result1);

    // Test 2: Failed to read sigmaY (only sigmaX written)
    parcel.WriteFloat(3.0f); // sigmaX
    parcel.RewindRead(0);
    auto obj2 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj2, nullptr);
    bool isValid2 = true;
    bool result2 = obj2->Unmarshalling(parcel, isValid2);
    EXPECT_FALSE(result2);

    // Test 3: Failed to read tileMode (sigmaX, sigmaY written, but tileMode missing)
    parcel.WriteFloat(5.0f); // sigmaY
    parcel.RewindRead(0);
    auto obj3 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj3, nullptr);
    bool isValid3 = true;
    bool result3 = obj3->Unmarshalling(parcel, isValid3);
    EXPECT_FALSE(result3);

    // Test 4: Failed to read blurType (sigmaX, sigmaY, tileMode written, but blurType missing)
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // tileMode
    parcel.RewindRead(0);
    auto obj4 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj4, nullptr);
    bool isValid4 = true;
    bool result4 = obj4->Unmarshalling(parcel, isValid4);
    EXPECT_FALSE(result4);

    // Test 5: Failed to read cropRect left (basic params written, but cropRect missing)
    parcel.WriteInt32(static_cast<int32_t>(ImageBlurType::GAUSS)); // blurType
    parcel.RewindRead(0);
    auto obj5 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj5, nullptr);
    bool isValid5 = true;
    bool result5 = obj5->Unmarshalling(parcel, isValid5);
    EXPECT_FALSE(result5);

    // Test 6: Failed to read cropRect top (left written but top missing)
    parcel.WriteFloat(1.0f); // left
    parcel.RewindRead(0);
    auto obj6 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj6, nullptr);
    bool isValid6 = true;
    bool result6 = obj6->Unmarshalling(parcel, isValid6);
    EXPECT_FALSE(result6);

    // Test 7: Failed to read cropRect right (left, top written but right missing)
    parcel.WriteFloat(2.0f); // top
    parcel.RewindRead(0);
    auto obj7 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj7, nullptr);
    bool isValid7 = true;
    bool result7 = obj7->Unmarshalling(parcel, isValid7);
    EXPECT_FALSE(result7);

    // Test 8: Failed to read cropRect bottom (left, top, right written but bottom missing)
    parcel.WriteFloat(101.0f); // right
    parcel.RewindRead(0);
    auto obj8 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj8, nullptr);
    bool isValid8 = true;
    bool result8 = obj8->Unmarshalling(parcel, isValid8);
    EXPECT_FALSE(result8);

    // Test 9: Failed to read hasInput flag (complete cropRect but missing hasInput)
    parcel.WriteFloat(102.0f); // bottom
    parcel.RewindRead(0);
    auto obj9 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj9, nullptr);
    bool isValid9 = true;
    bool result9 = obj9->Unmarshalling(parcel, isValid9);
    EXPECT_FALSE(result9);

    // Test 10: Failed to read isLazy flag (hasInput=true but missing isLazy)
    parcel.WriteBool(true); // hasInput = true
    parcel.RewindRead(0);
    auto obj10 = BlurImageFilterObj::CreateForUnmarshalling();
    ASSERT_NE(obj10, nullptr);
    bool isValid10 = true;
    bool result10 = obj10->Unmarshalling(parcel, isValid10);
    EXPECT_FALSE(result10);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS