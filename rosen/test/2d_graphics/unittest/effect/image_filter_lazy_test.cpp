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

#include <limits>
#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#endif

#include "draw/blend_mode.h"
#include "effect/blur_image_filter_obj.h"
#include "effect/color_filter.h"
#include "effect/image_filter.h"
#include "effect/image_filter_lazy.h"
#include "effect/shader_effect.h"
#include "transaction/rs_marshalling_helper.h"
#include "utils/object_helper.h"
#include "utils/rect.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageFilterLazyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ImageFilterLazyTest::SetUpTestCase() {}
void ImageFilterLazyTest::TearDownTestCase() {}
void ImageFilterLazyTest::SetUp() {}
void ImageFilterLazyTest::TearDown() {}

/*
 * @tc.name: CreateBlur001
 * @tc.desc: Test ImageFilterLazy::CreateBlur with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, CreateBlur001, TestSize.Level1)
{
    auto lazyBlur = ImageFilterLazy::CreateBlur(2.0f, 3.0f, TileMode::CLAMP, nullptr);
    EXPECT_NE(lazyBlur, nullptr);
    EXPECT_TRUE(lazyBlur->IsLazy());
    EXPECT_EQ(lazyBlur->GetType(), ImageFilter::FilterType::LAZY_IMAGE_FILTER);

    // Verify underlying object type
    auto imageFilterObj = lazyBlur->GetImageFilterObj();
    EXPECT_NE(imageFilterObj, nullptr);
    EXPECT_EQ(imageFilterObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(imageFilterObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::BLUR));

    // Test materialization
    auto materializedFilter = lazyBlur->Materialize();
    EXPECT_NE(materializedFilter, nullptr);
    EXPECT_FALSE(materializedFilter->IsLazy());
    EXPECT_EQ(materializedFilter->GetType(), ImageFilter::FilterType::BLUR);
}

/*
 * @tc.name: CreateColorFilter001
 * @tc.desc: Test ImageFilterLazy::CreateColorFilter scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, CreateColorFilter001, TestSize.Level1)
{
    // Test 1: CreateColorFilter with valid parameters
    auto colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    ASSERT_NE(colorFilter, nullptr);

    Rect cropRect(10.0f, 10.0f, 110.0f, 110.0f);
    auto lazyColorFilter = ImageFilterLazy::CreateColorFilter(colorFilter, nullptr, cropRect);
    EXPECT_NE(lazyColorFilter, nullptr);
    EXPECT_TRUE(lazyColorFilter->IsLazy());

    // Test materialization
    auto materializedFilter = lazyColorFilter->Materialize();
    EXPECT_NE(materializedFilter, nullptr);
    EXPECT_FALSE(materializedFilter->IsLazy());

    // Test 2: CreateColorFilter with null color filter
    auto nullLazyColorFilter = ImageFilterLazy::CreateColorFilter(nullptr, nullptr, cropRect);
    EXPECT_EQ(nullLazyColorFilter, nullptr);
}

/*
 * @tc.name: CreateOffset001
 * @tc.desc: Test ImageFilterLazy::CreateOffset with valid parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, CreateOffset001, TestSize.Level1)
{
    scalar dx = 10.0f;
    scalar dy = 20.0f;
    Rect cropRect(0, 0, 100, 100);

    auto lazyOffset = ImageFilterLazy::CreateOffset(dx, dy, nullptr, cropRect);
    EXPECT_NE(lazyOffset, nullptr);
    EXPECT_TRUE(lazyOffset->IsLazy());

    // Test materialization
    auto materializedFilter = lazyOffset->Materialize();
    EXPECT_NE(materializedFilter, nullptr);
    EXPECT_FALSE(materializedFilter->IsLazy());
}

/*
 * @tc.name: CreateShader001
 * @tc.desc: Test ImageFilterLazy::CreateShader scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, CreateShader001, TestSize.Level1)
{
    Rect cropRect(0, 0, 50, 50);

    // Test 1: CreateShader with valid parameters
    ColorQuad color = 0xFF0000FF; // Red color
    auto shader = ShaderEffect::CreateColorShader(color);
    ASSERT_NE(shader, nullptr);

    auto lazyShader = ImageFilterLazy::CreateShader(shader, cropRect);
    EXPECT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());

    // Test materialization
    auto materializedFilter = lazyShader->Materialize();
    EXPECT_NE(materializedFilter, nullptr);
    EXPECT_FALSE(materializedFilter->IsLazy());

    // Test 2: CreateShader with null shader
    auto nullLazyShader = ImageFilterLazy::CreateShader(nullptr, cropRect);
    EXPECT_EQ(nullLazyShader, nullptr);
}

/*
 * @tc.name: SerializeDeserialize001
 * @tc.desc: Test ImageFilterLazy Serialize/Deserialize should fail with proper error logging
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, SerializeDeserialize001, TestSize.Level1)
{
    scalar sigmaX = 2.0f;
    scalar sigmaY = 3.0f;
    auto lazyBlur = ImageFilterLazy::CreateBlur(sigmaX, sigmaY, TileMode::CLAMP);
    ASSERT_NE(lazyBlur, nullptr);

    // Serialize should return nullptr and log error
    auto data = lazyBlur->Serialize();
    EXPECT_EQ(data, nullptr);

    // Deserialize should return false and log error
    auto testData = std::make_shared<Data>();
    bool result = lazyBlur->Deserialize(testData);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: NestedLazyFilter001
 * @tc.desc: Test nested lazy filters work correctly
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, NestedLazyFilter001, TestSize.Level1)
{
    // Create a lazy blur filter
    auto lazyBlur = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::CLAMP);
    ASSERT_NE(lazyBlur, nullptr);

    // Use lazy blur as input to offset filter
    auto lazyOffset = ImageFilterLazy::CreateOffset(10.0f, 10.0f, lazyBlur);
    EXPECT_NE(lazyOffset, nullptr);
    EXPECT_TRUE(lazyOffset->IsLazy());

    // Test materialization of nested filters
    auto materializedFilter = lazyOffset->Materialize();
    EXPECT_NE(materializedFilter, nullptr);
    EXPECT_FALSE(materializedFilter->IsLazy());
}

/*
 * @tc.name: GetImageFilterObj001
 * @tc.desc: Test GetImageFilterObj returns valid object
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, GetImageFilterObj001, TestSize.Level1)
{
    auto lazyBlur = ImageFilterLazy::CreateBlur(1.0f, 1.0f, TileMode::CLAMP);
    ASSERT_NE(lazyBlur, nullptr);

    auto imageFilterObj = lazyBlur->GetImageFilterObj();
    EXPECT_NE(imageFilterObj, nullptr);
    EXPECT_EQ(imageFilterObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    EXPECT_EQ(imageFilterObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::BLUR));
}

/*
 * @tc.name: MaterializeCaching001
 * @tc.desc: Test materialization caching functionality
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, MaterializeCaching001, TestSize.Level1)
{
    auto lazyBlur = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::CLAMP);
    ASSERT_NE(lazyBlur, nullptr);

    // First materialization
    auto filter1 = lazyBlur->Materialize();
    EXPECT_NE(filter1, nullptr);

    // Second materialization should return cached result (same pointer)
    auto filter2 = lazyBlur->Materialize();
    EXPECT_EQ(filter1, filter2);

    // Third materialization should return same cached object
    auto filter3 = lazyBlur->Materialize();
    EXPECT_NE(filter3, nullptr);
    EXPECT_EQ(filter1, filter3);
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: Marshalling001
 * @tc.desc: Test ImageFilterLazy::Marshalling with blur filter
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, Marshalling001, TestSize.Level1)
{
    auto lazyBlur = ImageFilterLazy::CreateBlur(3.0f, 4.0f, TileMode::CLAMP);
    ASSERT_NE(lazyBlur, nullptr);

    Parcel parcel;
    bool result = lazyBlur->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: Unmarshalling001
 * @tc.desc: Test ImageFilterLazy::Unmarshalling with valid blur filter data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, Unmarshalling001, TestSize.Level1)
{
    // Prepare parcel data for blur filter
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::BLUR));
    parcel.WriteFloat(2.0f); // sigmaX
    parcel.WriteFloat(3.0f); // sigmaY
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // tileMode
    parcel.WriteInt32(static_cast<int32_t>(ImageBlurType::GAUSS)); // blurType
    parcel.WriteFloat(0.0f); // cropRect left
    parcel.WriteFloat(0.0f); // cropRect top
    parcel.WriteFloat(100.0f); // cropRect right
    parcel.WriteFloat(100.0f); // cropRect bottom
    parcel.WriteBool(false); // hasInput = false

    bool isValid = true;
    auto lazyFilter = ImageFilterLazy::Unmarshalling(parcel, isValid);
    EXPECT_NE(lazyFilter, nullptr); // Should succeed with ObjectHelper setup
    EXPECT_TRUE(isValid);

    if (lazyFilter) {
        EXPECT_TRUE(lazyFilter->IsLazy());
        EXPECT_EQ(lazyFilter->GetType(), ImageFilter::FilterType::LAZY_IMAGE_FILTER);

        // Verify the underlying object
        auto imageFilterObj = lazyFilter->GetImageFilterObj();
        EXPECT_NE(imageFilterObj, nullptr);
        EXPECT_EQ(imageFilterObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
        EXPECT_EQ(imageFilterObj->GetSubType(), static_cast<int32_t>(ImageFilter::FilterType::BLUR));
    }
}

/*
 * @tc.name: Unmarshalling002
 * @tc.desc: Test ImageFilterLazy::Unmarshalling with invalid data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, Unmarshalling002, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(999); // Invalid type
    parcel.WriteInt32(888); // Invalid subType

    bool isValid = true;
    auto lazyFilter = ImageFilterLazy::Unmarshalling(parcel, isValid);
    EXPECT_EQ(lazyFilter, nullptr);
}
#endif

/*
 * @tc.name: MaterializeNullObj001
 * @tc.desc: Test Materialize with null imageFilterObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, MaterializeNullObj001, TestSize.Level1)
{
    // Test defensive programming for null imageFilterObj_
    auto lazyFilter = ImageFilterLazy::CreateFromImageFilterObj(nullptr);
    EXPECT_EQ(lazyFilter, nullptr);
}

/*
 * @tc.name: CreateBlurEdgeCases001
 * @tc.desc: Test CreateBlur with edge case parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, CreateBlurEdgeCases001, TestSize.Level1)
{
    // Test with zero sigma values
    auto lazyBlur1 = ImageFilterLazy::CreateBlur(0.0f, 0.0f, TileMode::CLAMP);
    EXPECT_NE(lazyBlur1, nullptr);

    // Test with very small positive values
    auto lazyBlur2 = ImageFilterLazy::CreateBlur(0.001f, 0.001f, TileMode::CLAMP);
    EXPECT_NE(lazyBlur2, nullptr);

    // Test with very large values
    auto lazyBlur3 = ImageFilterLazy::CreateBlur(1000.0f, 1000.0f, TileMode::CLAMP);
    EXPECT_NE(lazyBlur3, nullptr);

    // Test with different TileModes
    auto lazyBlur4 = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::REPEAT);
    EXPECT_NE(lazyBlur4, nullptr);

    auto lazyBlur5 = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::MIRROR);
    EXPECT_NE(lazyBlur5, nullptr);

    auto lazyBlur6 = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::DECAL);
    EXPECT_NE(lazyBlur6, nullptr);
}

/*
 * @tc.name: CreateOffsetEdgeCases001
 * @tc.desc: Test CreateOffset with edge case parameters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, CreateOffsetEdgeCases001, TestSize.Level1)
{
    // Test with zero offset
    auto lazyOffset1 = ImageFilterLazy::CreateOffset(0.0f, 0.0f);
    EXPECT_NE(lazyOffset1, nullptr);

    // Test with negative offset
    auto lazyOffset2 = ImageFilterLazy::CreateOffset(-10.0f, -20.0f);
    EXPECT_NE(lazyOffset2, nullptr);

    // Test with very large offset
    auto lazyOffset3 = ImageFilterLazy::CreateOffset(10000.0f, -10000.0f);
    EXPECT_NE(lazyOffset3, nullptr);

    // Test with empty crop rect
    Rect emptyCropRect(0, 0, 0, 0);
    auto lazyOffset4 = ImageFilterLazy::CreateOffset(5.0f, 5.0f, nullptr, emptyCropRect);
    EXPECT_NE(lazyOffset4, nullptr);

    // Test with invalid crop rect (negative dimensions)
    Rect invalidCropRect(100, 100, 50, 50);
    auto lazyOffset5 = ImageFilterLazy::CreateOffset(5.0f, 5.0f, nullptr, invalidCropRect);
    EXPECT_NE(lazyOffset5, nullptr);
}

/*
 * @tc.name: ChainedLazyFilters001
 * @tc.desc: Test chaining multiple lazy filters and compare with direct ImageFilter creation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, ChainedLazyFilters001, TestSize.Level1)
{
    // Define common parameters - use same parameters for both methods
    scalar sigmaX = 2.0f;
    scalar sigmaY = 2.0f;
    scalar dx = 10.0f;
    scalar dy = 15.0f;
    Rect cropRect(30.0f, 30.0f, 130.0f, 130.0f); // Use same cropRect for final comparison

    auto colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    ASSERT_NE(colorFilter, nullptr);

    // Method 1: Create chain using ImageFilterLazy (Blur -> Offset -> ColorFilter)
    auto lazyBlur = ImageFilterLazy::CreateBlur(sigmaX, sigmaY, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(lazyBlur, nullptr);

    auto lazyOffset = ImageFilterLazy::CreateOffset(dx, dy, lazyBlur, cropRect);
    ASSERT_NE(lazyOffset, nullptr);

    auto lazyColorFilter = ImageFilterLazy::CreateColorFilter(colorFilter, lazyOffset, cropRect);
    ASSERT_NE(lazyColorFilter, nullptr);

    // Materialize the lazy chain
    auto materializedLazy = lazyColorFilter->Materialize();
    ASSERT_NE(materializedLazy, nullptr);
    EXPECT_FALSE(materializedLazy->IsLazy());

    // Method 2: Create equivalent chain using direct ImageFilter calls with SAME parameters
    auto directBlur = ImageFilter::CreateBlurImageFilter(sigmaX, sigmaY, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(directBlur, nullptr);

    auto directOffset = ImageFilter::CreateOffsetImageFilter(dx, dy, directBlur, cropRect);
    ASSERT_NE(directOffset, nullptr);

    auto directColorFilter = ImageFilter::CreateColorFilterImageFilter(*colorFilter, directOffset, cropRect);
    ASSERT_NE(directColorFilter, nullptr);

    // Compare serialized data to verify equivalence
    auto lazyData = materializedLazy->Serialize();
    auto directData = directColorFilter->Serialize();
    ASSERT_NE(lazyData, nullptr);
    ASSERT_NE(directData, nullptr);
    EXPECT_GT(lazyData->GetSize(), 0);
    EXPECT_GT(directData->GetSize(), 0);

    // Compare data sizes
    EXPECT_EQ(lazyData->GetSize(), directData->GetSize());

    // Compare serialized content byte by byte
    const uint8_t* lazyBytes = static_cast<const uint8_t*>(lazyData->GetData());
    const uint8_t* directBytes = static_cast<const uint8_t*>(directData->GetData());
    int memResult = memcmp(lazyBytes, directBytes, lazyData->GetSize());
    EXPECT_EQ(memResult, 0);
}

/*
 * @tc.name: MixedLazyAndNormalFilters001
 * @tc.desc: Test mixing lazy and normal filters
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, MixedLazyAndNormalFilters001, TestSize.Level1)
{
    // Create a normal blur filter
    Rect blurCropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto normalBlur = ImageFilter::CreateBlurImageFilter(3.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, blurCropRect);
    ASSERT_NE(normalBlur, nullptr);
    EXPECT_FALSE(normalBlur->IsLazy());

    // Use normal filter as input to lazy offset
    auto lazyOffset = ImageFilterLazy::CreateOffset(5.0f, 10.0f, normalBlur);
    ASSERT_NE(lazyOffset, nullptr);
    EXPECT_TRUE(lazyOffset->IsLazy());

    // Materialize the lazy offset
    auto materializedFilter = lazyOffset->Materialize();
    EXPECT_NE(materializedFilter, nullptr);
    EXPECT_FALSE(materializedFilter->IsLazy());
}

/*
 * @tc.name: MultipleInstancesIndependence001
 * @tc.desc: Test that multiple lazy filter instances are independent
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, MultipleInstancesIndependence001, TestSize.Level1)
{
    // Create two identical lazy blur filters
    auto lazyBlur1 = ImageFilterLazy::CreateBlur(4.0f, 4.0f, TileMode::CLAMP);
    auto lazyBlur2 = ImageFilterLazy::CreateBlur(4.0f, 4.0f, TileMode::CLAMP);
    ASSERT_NE(lazyBlur1, nullptr);
    ASSERT_NE(lazyBlur2, nullptr);
    EXPECT_NE(lazyBlur1, lazyBlur2); // Different instances

    // Materialize one filter
    auto filter1 = lazyBlur1->Materialize();
    EXPECT_NE(filter1, nullptr);

    // Second filter should be able to materialize independently
    auto filter2 = lazyBlur2->Materialize();
    EXPECT_NE(filter2, nullptr);

    // Both materialized filters should be different instances
    EXPECT_NE(filter1, filter2);
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: UnmarshallingReadTypeFailure001
 * @tc.desc: Test ImageFilterLazy::Unmarshalling with parcel.ReadInt32(type) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterLazyTest, UnmarshallingReadTypeFailure001, TestSize.Level1)
{
    // Create an empty Parcel - ReadInt32 will fail due to no data
    Parcel emptyParcel;
    bool isValid = true;
    auto result = ImageFilterLazy::Unmarshalling(emptyParcel, isValid, 0);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: UnmarshallingReadSubTypeFailure001
 * @tc.desc: Test ImageFilterLazy::Unmarshalling with parcel.ReadInt32(subType) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterLazyTest, UnmarshallingReadSubTypeFailure001, TestSize.Level1)
{
    // Create a Parcel with only one int32 - first ReadInt32 succeeds, second fails
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    // No second int32 written, so second ReadInt32 will fail
    bool isValid = true;
    auto result = ImageFilterLazy::Unmarshalling(parcel, isValid, 0);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: MarshallingUnmarshallingRoundTrip001
 * @tc.desc: Test complete Marshalling/Unmarshalling round trip for ImageFilterLazy
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    // Create a chained lazy filter similar to ChainedLazyFilters001
    scalar sigmaX = 3.0f;
    scalar sigmaY = 4.0f;
    scalar dx = 15.0f;
    scalar dy = 25.0f;
    Rect cropRect(20.0f, 20.0f, 120.0f, 120.0f);

    auto colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    ASSERT_NE(colorFilter, nullptr);

    // Create the chained lazy filter
    auto lazyBlur = ImageFilterLazy::CreateBlur(sigmaX, sigmaY, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(lazyBlur, nullptr);

    auto lazyOffset = ImageFilterLazy::CreateOffset(dx, dy, lazyBlur, cropRect);
    ASSERT_NE(lazyOffset, nullptr);

    auto lazyColorFilter = ImageFilterLazy::CreateColorFilter(colorFilter, lazyOffset, cropRect);
    ASSERT_NE(lazyColorFilter, nullptr);

    // Verify it's a lazy filter
    EXPECT_TRUE(lazyColorFilter->IsLazy());
    EXPECT_EQ(lazyColorFilter->GetType(), ImageFilter::FilterType::LAZY_IMAGE_FILTER);

    // Test Marshalling
    Parcel parcel;
    bool marshalResult = lazyColorFilter->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    parcel.RewindRead(0);
    bool isValid = true;
    auto unmarshalledFilter = ImageFilterLazy::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(isValid);
    ASSERT_NE(unmarshalledFilter, nullptr);
    EXPECT_TRUE(unmarshalledFilter->IsLazy());
    EXPECT_EQ(unmarshalledFilter->GetType(), ImageFilter::FilterType::LAZY_IMAGE_FILTER);

    // Verify the underlying object structure
    auto originalObj = lazyColorFilter->GetImageFilterObj();
    auto unmarshalledObj = unmarshalledFilter->GetImageFilterObj();
    ASSERT_NE(originalObj, nullptr);
    ASSERT_NE(unmarshalledObj, nullptr);
    EXPECT_EQ(originalObj->GetType(), unmarshalledObj->GetType());
    EXPECT_EQ(originalObj->GetSubType(), unmarshalledObj->GetSubType());

    // Test Materialize for both original and unmarshalled filters
    auto originalMaterialized = lazyColorFilter->Materialize();
    auto unmarshalledMaterialized = unmarshalledFilter->Materialize();
    ASSERT_NE(originalMaterialized, nullptr);
    ASSERT_NE(unmarshalledMaterialized, nullptr);
    EXPECT_FALSE(originalMaterialized->IsLazy());
    EXPECT_FALSE(unmarshalledMaterialized->IsLazy());

    // Test serialization data comparison
    auto originalSerializedData = originalMaterialized->Serialize();
    auto unmarshalledSerializedData = unmarshalledMaterialized->Serialize();
    ASSERT_NE(originalSerializedData, nullptr);
    ASSERT_NE(unmarshalledSerializedData, nullptr);
    EXPECT_GT(originalSerializedData->GetSize(), 0);
    EXPECT_GT(unmarshalledSerializedData->GetSize(), 0);

    // Compare serialized data sizes
    EXPECT_EQ(originalSerializedData->GetSize(), unmarshalledSerializedData->GetSize());

    // Compare serialized data content byte by byte
    const uint8_t* originalBytes = static_cast<const uint8_t*>(originalSerializedData->GetData());
    const uint8_t* unmarshalledBytes = static_cast<const uint8_t*>(unmarshalledSerializedData->GetData());
    int memResult = memcmp(originalBytes, unmarshalledBytes, originalSerializedData->GetSize());
    EXPECT_EQ(memResult, 0);

    // Test round-trip marshalling consistency
    Parcel parcel2;
    bool marshalResult2 = unmarshalledFilter->Marshalling(parcel2);
    EXPECT_TRUE(marshalResult2);

    // Compare marshalled data sizes
    size_t originalMarshalledSize = parcel.GetDataSize();
    size_t unmarshalledMarshalledSize = parcel2.GetDataSize();
    EXPECT_EQ(originalMarshalledSize, unmarshalledMarshalledSize);
    EXPECT_GT(originalMarshalledSize, 0);

    // Compare marshalled data content
    const uint8_t* originalMarshalledData = reinterpret_cast<const uint8_t*>(parcel.GetData());
    const uint8_t* unmarshalledMarshalledData = reinterpret_cast<const uint8_t*>(parcel2.GetData());
    int marshalledMemResult = memcmp(originalMarshalledData, unmarshalledMarshalledData, originalMarshalledSize);
    EXPECT_EQ(marshalledMemResult, 0);
}

/*
 * @tc.name: MarshallingUnmarshallingMultipleTypes001
 * @tc.desc: Test Marshalling/Unmarshalling for different ImageFilterLazy types
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterLazyTest, MarshallingUnmarshallingMultipleTypes001, TestSize.Level1)
{
    // Test different types of lazy filters
    std::vector<std::shared_ptr<ImageFilterLazy>> testFilters;

    // 1. Blur filter
    auto lazyBlur = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::CLAMP);
    testFilters.push_back(lazyBlur);

    // 2. Offset filter
    auto lazyOffset = ImageFilterLazy::CreateOffset(10.0f, 15.0f);
    testFilters.push_back(lazyOffset);

    // 3. ColorFilter
    auto colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    ASSERT_NE(colorFilter, nullptr);
    auto lazyColorFilter = ImageFilterLazy::CreateColorFilter(colorFilter, nullptr);
    testFilters.push_back(lazyColorFilter);

    // 4. Shader filter
    ColorQuad color = 0xFF0000FF;
    auto shader = ShaderEffect::CreateColorShader(color);
    ASSERT_NE(shader, nullptr);
    auto lazyShader = ImageFilterLazy::CreateShader(shader);
    testFilters.push_back(lazyShader);

    // Test each filter type
    for (size_t i = 0; i < testFilters.size(); i++) {
        auto& originalFilter = testFilters[i];
        ASSERT_NE(originalFilter, nullptr);
        EXPECT_TRUE(originalFilter->IsLazy());

        // Store original type information
        auto originalType = originalFilter->GetType();
        auto originalObj = originalFilter->GetImageFilterObj();
        ASSERT_NE(originalObj, nullptr);
        auto originalSubType = originalObj->GetSubType();

        // Test Marshalling
        Parcel parcel;
        bool marshalResult = originalFilter->Marshalling(parcel);
        EXPECT_TRUE(marshalResult);

        // Test Unmarshalling
        parcel.RewindRead(0);
        bool isValid = true;
        auto unmarshalledFilter = ImageFilterLazy::Unmarshalling(parcel, isValid);
        EXPECT_TRUE(isValid);
        ASSERT_NE(unmarshalledFilter, nullptr);

        // Verify type preservation
        EXPECT_EQ(unmarshalledFilter->GetType(), originalType);
        EXPECT_TRUE(unmarshalledFilter->IsLazy());

        auto unmarshalledObj = unmarshalledFilter->GetImageFilterObj();
        ASSERT_NE(unmarshalledObj, nullptr);
        EXPECT_EQ(unmarshalledObj->GetSubType(), originalSubType);

        // Test materialization
        auto originalMaterialized = originalFilter->Materialize();
        auto unmarshalledMaterialized = unmarshalledFilter->Materialize();

        ASSERT_NE(originalMaterialized, nullptr);
        ASSERT_NE(unmarshalledMaterialized, nullptr);
        EXPECT_FALSE(originalMaterialized->IsLazy());
        EXPECT_FALSE(unmarshalledMaterialized->IsLazy());

        // Test serialization data comparison for materialized filters
        auto originalSerializedData = originalMaterialized->Serialize();
        auto unmarshalledSerializedData = unmarshalledMaterialized->Serialize();
        // Both should have valid serialized data
        ASSERT_NE(originalSerializedData, nullptr);
        ASSERT_NE(unmarshalledSerializedData, nullptr);
        EXPECT_GT(originalSerializedData->GetSize(), 0);
        EXPECT_GT(unmarshalledSerializedData->GetSize(), 0);
        EXPECT_EQ(originalSerializedData->GetSize(), unmarshalledSerializedData->GetSize());

        // Memory comparison
        const uint8_t* originalBytes = static_cast<const uint8_t*>(originalSerializedData->GetData());
        const uint8_t* unmarshalledBytes = static_cast<const uint8_t*>(unmarshalledSerializedData->GetData());
        int memResult = memcmp(originalBytes, unmarshalledBytes, originalSerializedData->GetSize());
        EXPECT_EQ(memResult, 0);
    }
}
#endif

/*
 * @tc.name: CreateFromImageFilterObjInvalidType001
 * @tc.desc: Test ImageFilterLazy::CreateFromImageFilterObj with invalid object type
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterLazyTest, CreateFromImageFilterObjInvalidType001, TestSize.Level1)
{
    // Create a mock object with wrong type by inheriting from ImageFilterObj and modifying type
    class MockInvalidImageFilterObj : public ImageFilterObj {
    public:
        MockInvalidImageFilterObj() : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::BLUR)) {
            // Override the type to be SHADER_EFFECT instead of IMAGE_FILTER
            type_ = static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT);
        }

        std::shared_ptr<void> GenerateBaseObject() override {
            return nullptr; // Not used in this test
        }

#ifdef ROSEN_OHOS
        bool Marshalling(Parcel& parcel) override { return true; }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth) override {
            isValid = true;
            return true;
        }
#endif
    };

    // Create mock object with invalid type
    auto invalidObj = std::make_shared<MockInvalidImageFilterObj>();
    EXPECT_EQ(invalidObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));

    // Test the validation branch - should return nullptr due to invalid type
    auto result = ImageFilterLazy::CreateFromImageFilterObj(invalidObj);
    EXPECT_EQ(result, nullptr);

    // Test with NO_TYPE
    class MockNoTypeImageFilterObj : public ImageFilterObj {
    public:
        MockNoTypeImageFilterObj() : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::BLUR)) {
            // Override the type to be NO_TYPE instead of IMAGE_FILTER
            type_ = static_cast<int32_t>(Object::ObjectType::NO_TYPE);
        }

        std::shared_ptr<void> GenerateBaseObject() override { return nullptr; }

#ifdef ROSEN_OHOS
        bool Marshalling(Parcel& parcel) override { return true; }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth) override {
            isValid = true;
            return true;
        }
#endif
    };

    auto noTypeObj = std::make_shared<MockNoTypeImageFilterObj>();
    EXPECT_EQ(noTypeObj->GetType(), static_cast<int32_t>(Object::ObjectType::NO_TYPE));

    // Test with NO_TYPE - should also return nullptr
    auto noTypeResult = ImageFilterLazy::CreateFromImageFilterObj(noTypeObj);
    EXPECT_EQ(noTypeResult, nullptr);
}

/*
 * @tc.name: UnmarshallingCreateFilterLazyFailure001
 * @tc.desc: Test ImageFilterLazy::Unmarshalling CreateFromImageFilterObj failure branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterLazyTest, UnmarshallingCreateFilterLazyFailure001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    // Create a mock object that will cause CreateFromImageFilterObj to fail
    class MockFailingImageFilterObj : public ImageFilterObj {
    public:
        MockFailingImageFilterObj() : ImageFilterObj(static_cast<int32_t>(ImageFilter::FilterType::BLUR)) {
            // Override the type to be SHADER_EFFECT to make CreateFromImageFilterObj fail
            type_ = static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT);
        }

        std::shared_ptr<void> GenerateBaseObject() override {
            return nullptr;
        }

        bool Marshalling(Parcel& parcel) override { return true; }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth) override {
            isValid = true;
            return true;
        }
    };

    // Register a mock function that returns an object with wrong type
    auto originalFunc = ObjectHelper::Instance().GetFunc(
        static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER),
        static_cast<int32_t>(ImageFilter::FilterType::BLUR));

    // Register a mock function that returns an object that will cause CreateFromImageFilterObj to fail
    ObjectHelper::Instance().Register(
        static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER),
        static_cast<int32_t>(ImageFilter::FilterType::BLUR),
        [](Parcel& parcel, bool& isValid, int32_t depth) -> std::shared_ptr<Object> {
            auto obj = std::make_shared<MockFailingImageFilterObj>();
            return std::static_pointer_cast<Object>(obj);
        });

    // Create parcel with valid data for IMAGE_FILTER/BLUR
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
    parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::BLUR));

    // Test Unmarshalling - should fail due to CreateFromImageFilterObj returning nullptr
    bool isValid = true;
    auto result = ImageFilterLazy::Unmarshalling(parcel, isValid, 0);
    EXPECT_EQ(result, nullptr);

    // Restore original function
    if (originalFunc) {
        ObjectHelper::Instance().Register(
            static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER),
            static_cast<int32_t>(ImageFilter::FilterType::BLUR),
            originalFunc);
    }
#endif
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS