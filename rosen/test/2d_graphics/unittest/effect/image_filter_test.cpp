/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "effect/image_filter.h"
#include "effect/image_filter_lazy.h"
#include "effect_test_utils.h"
#include "image/image.h"
#include "utils/object_helper.h"
#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#endif
#include "transaction/rs_marshalling_helper.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ImageFilterTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void ImageFilterTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}
void ImageFilterTest::SetUp() {}
void ImageFilterTest::TearDown() {}

// Create a mock filter that will return null from Serialize
class MockImageFilter : public ImageFilter {
public:
    MockImageFilter() : ImageFilter(FilterType::BLUR) {}

    // Override Serialize to return null to simulate empty data scenario
    std::shared_ptr<Data> Serialize() const override
    {
        return nullptr;
    }
};

/*
 * @tc.name: CreateBlurImageFilterTest001
 * @tc.desc: test for creating a filter that blurs its input by the separate X and Y sinma value.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateBlurImageFilterTest001, TestSize.Level1)
{
    auto input = ImageFilter::CreateBlurImageFilter(10.0f, 10.0f, TileMode::CLAMP, nullptr);
    EXPECT_TRUE(input != nullptr);
    auto imageFilter = ImageFilter::CreateBlurImageFilter(10.0f, 10.0f, TileMode::CLAMP, input);
    EXPECT_TRUE(imageFilter != nullptr);
    auto imageFilter1 = ImageFilter::CreateBlurImageFilter(-1.0f, -1.0f, TileMode::CLAMP, input);
    EXPECT_TRUE(imageFilter1 != nullptr);
    auto imageFilter2 = ImageFilter::CreateBlurImageFilter(-1.0f, 10.0f, TileMode::CLAMP, input);
    EXPECT_TRUE(imageFilter2 != nullptr);
    auto imageFilter3 = ImageFilter::CreateBlurImageFilter(10.0f, -1.0f, TileMode::CLAMP, input);
    EXPECT_TRUE(imageFilter3 != nullptr);
}

/*
 * @tc.name: CreateColorFilterImageFilterTest001
 * @tc.desc: test for creating a filter that applies the color filter to the input filter results.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateColorFilterImageFilterTest001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    auto input = ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    EXPECT_TRUE(input != nullptr);
    auto imageFilter = ImageFilter::CreateColorFilterImageFilter(*colorFilter, input);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateOffsetImageFilterTest001
 * @tc.desc: test for creating a filter that offsets the input filter by the given vector.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateOffsetImageFilterTest001, TestSize.Level1)
{
    auto input = ImageFilter::CreateOffsetImageFilter(10.0f, 10.0f, nullptr);
    EXPECT_TRUE(input != nullptr);
    auto imageFilter = ImageFilter::CreateOffsetImageFilter(10.0f, 10.0f, input);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateArithmeticImageFilterTest001
 * @tc.desc: test for creating a filter that implements a custom blend mode.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateArithmeticImageFilterTest001, TestSize.Level1)
{
    std::vector<scalar> coefficients;
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    auto f1 = ImageFilter::CreateBlurImageFilter(10.0f, 10.0f, TileMode::CLAMP, nullptr);
    auto f2 = ImageFilter::CreateOffsetImageFilter(10.0f, 10.0f, nullptr);
    auto imageFilter = ImageFilter::CreateArithmeticImageFilter(coefficients, true, f1, f2);
    EXPECT_TRUE(imageFilter != nullptr);
    imageFilter = ImageFilter::CreateArithmeticImageFilter(coefficients, false, f1, f2);
    EXPECT_TRUE(imageFilter != nullptr);
    std::vector<scalar> coefficients1;
    coefficients1.push_back(-1.0f);
    coefficients1.push_back(0.0f);
    coefficients1.push_back(10.0f);
    coefficients1.push_back(10.0f);
    imageFilter = ImageFilter::CreateArithmeticImageFilter(coefficients1, true, f1, f2);
    EXPECT_TRUE(imageFilter != nullptr);
    imageFilter = ImageFilter::CreateArithmeticImageFilter(coefficients, true, nullptr, f2);
    EXPECT_TRUE(imageFilter != nullptr);
    imageFilter = ImageFilter::CreateArithmeticImageFilter(coefficients, true, f1, nullptr);
    EXPECT_TRUE(imageFilter != nullptr);
    imageFilter = ImageFilter::CreateArithmeticImageFilter(coefficients, true, nullptr, nullptr);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateComposeImageFilterTest001
 * @tc.desc: test for creating a filter that composes f1 with f2.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateComposeImageFilterTest001, TestSize.Level1)
{
    auto imageFilter1 = ImageFilter::CreateBlurImageFilter(10.0f, 20.0f, TileMode::CLAMP, nullptr);
    auto imageFilter2 = ImageFilter::CreateOffsetImageFilter(30.0f, 40.0f, nullptr);
    auto imageFilter = ImageFilter::CreateComposeImageFilter(imageFilter1, imageFilter2);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: ImageFilterCreateTest001
 * @tc.desc: testing the ImageFilter Construction Method
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, ImageFilterCreateTest001, TestSize.Level1)
{
    auto imageFilter = std::make_shared<ImageFilter>(ImageFilter::FilterType::BLUR, 10.0f, 10.0f, nullptr);
    ASSERT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: ImageFilterCreateTest002
 * @tc.desc: testing the ImageFilter Construction Method
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, ImageFilterCreateTest002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    auto imageFilter = std::make_shared<ImageFilter>(ImageFilter::FilterType::COLOR_FILTER, *colorFilter, nullptr);
    ASSERT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: ImageFilterCreateTest003
 * @tc.desc: testing the ImageFilter Construction Method
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, ImageFilterCreateTest003, TestSize.Level1)
{
    std::vector<scalar> coefficients;
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    coefficients.push_back(10.0f);
    auto imageFilter = std::make_shared<ImageFilter>(ImageFilter::FilterType::ARITHMETIC, coefficients,
        true, nullptr, nullptr);
    ASSERT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: ImageFilterCreateTest004
 * @tc.desc: testing the ImageFilter Construction Method
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, ImageFilterCreateTest004, TestSize.Level1)
{
    auto imageFilter1 = ImageFilter::CreateBlurImageFilter(10.0f, 20.0f, TileMode::CLAMP, nullptr);
    auto imageFilter2 = ImageFilter::CreateOffsetImageFilter(30.0f, 40.0f, nullptr);
    auto imageFilter = std::make_shared<ImageFilter>(ImageFilter::FilterType::COMPOSE, imageFilter1, imageFilter2);
    ASSERT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateBlendImageFilterTest001
 * @tc.desc: test for creating a filter takes an BlendMode
 *           and uses it to composite the two filters together.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateBlendImageFilterTest001, TestSize.Level1)
{
    auto imageFilter1 = ImageFilter::CreateBlurImageFilter(10.0f, 20.0f, TileMode::CLAMP, nullptr);
    auto imageFilter2 = ImageFilter::CreateOffsetImageFilter(30.0f, 40.0f, nullptr);
    auto imageFilter = ImageFilter::CreateBlendImageFilter(BlendMode::CLEAR, imageFilter1, imageFilter2);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateBlendImageFilterTest002
 * @tc.desc: test for creating a filter takes an invalid BlendMode
 *           and uses it to composite the two filters together.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateBlendImageFilterTest002, TestSize.Level1)
{
    auto imageFilter1 = ImageFilter::CreateBlurImageFilter(10.0f, 20.0f, TileMode::CLAMP, nullptr);
    auto imageFilter2 = ImageFilter::CreateOffsetImageFilter(30.0f, 40.0f, nullptr);
    auto imageFilter = ImageFilter::CreateBlendImageFilter(BlendMode::EXCLUSION, imageFilter1, imageFilter2);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateBlendImageFilterTest003
 * @tc.desc: test for creating a filter takes a BlendMode
 *           and uses it to composite the two filters together.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateBlendImageFilterTest003, TestSize.Level1)
{
    auto imageFilter2 = ImageFilter::CreateOffsetImageFilter(30.0f, 40.0f, nullptr);
    auto imageFilter = ImageFilter::CreateBlendImageFilter(BlendMode::CLEAR, nullptr, imageFilter2);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateBlendImageFilterTest004
 * @tc.desc: test for creating a filter takes a BlendMode
 *           and uses it to composite the two filters together.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateBlendImageFilterTest004, TestSize.Level1)
{
    auto imageFilter1 = ImageFilter::CreateBlurImageFilter(10.0f, 20.0f, TileMode::CLAMP, nullptr);
    auto imageFilter = ImageFilter::CreateBlendImageFilter(BlendMode::CLEAR, imageFilter1, nullptr);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateBlendImageFilterTest005
 * @tc.desc: test for creating a filter takes a BlendMode
 *           and uses it to composite the two filters together.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateBlendImageFilterTest005, TestSize.Level1)
{
    auto imageFilter = ImageFilter::CreateBlendImageFilter(BlendMode::CLEAR, nullptr, nullptr);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateShaderImageFilterTest001
 * @tc.desc: test for creating a filter that fills the output with the per-pixel evaluation of the ShaderEffect.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateShaderImageFilterTest001, TestSize.Level1)
{
    std::shared_ptr<ShaderEffect> effect = ShaderEffect::CreateColorShader(0);
    auto imageFilter = ImageFilter::CreateShaderImageFilter(effect);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateShaderImageFilterTest002
 * @tc.desc: test for creating a filter that with invalid input.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateShaderImageFilterTest002, TestSize.Level1)
{
    auto imageFilter = ImageFilter::CreateShaderImageFilter(nullptr);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateShaderImageFilterTest003
 * @tc.desc: test for creating a filter that with invalid input.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateShaderImageFilterTest003, TestSize.Level1)
{
    std::shared_ptr<ShaderEffect> effect = ShaderEffect::CreateColorShader(0);
    Rect rect {0, 0, 100.0f, 100.0f};
    auto imageFilter = ImageFilter::CreateShaderImageFilter(effect, rect);
    EXPECT_TRUE(imageFilter != nullptr);
}

/*
 * @tc.name: CreateImageImageFilterTest001
 * @tc.desc: test for creating a filter that with image.
 * @tc.type: FUNC
 * @tc.require: I77M3W
 */
HWTEST_F(ImageFilterTest, CreateImageImageFilterTest001, TestSize.Level1)
{
    std::shared_ptr<Image> image = std::make_shared<Image>();
    Rect rect {0, 0, 100.0f, 100.0f};
    Rect rect2 {0, 0, 10.0f, 10.0f};
    SamplingOptions options;
    auto imageFilter = ImageFilter::CreateImageImageFilter(image, rect, rect2, options);
    EXPECT_TRUE(imageFilter != nullptr);
    auto imageFilter2 = ImageFilter::CreateImageImageFilter(nullptr, rect, rect2, options);
    EXPECT_TRUE(imageFilter2 != nullptr);
}

/*
 * @tc.name: CreateHDSampleImageFilterTest001
 * @tc.desc: test for creating HDSampleImageFilter.
 * @tc.type: FUNC
 * @tc.require:ICR1ZE
 */
HWTEST_F(ImageFilterTest, CreateHDSampleImageFilterTest001, TestSize.Level1)
{
    std::shared_ptr<Image> image = std::make_shared<Image>();
    Rect srcRect {0, 0, 100.0f, 100.0f};
    Rect dstRect {0, 0, 10.0f, 10.0f};
    HDSampleInfo info;
    auto imageFilter = ImageFilter::CreateHDSampleImageFilter(image, srcRect, dstRect, info);
    EXPECT_TRUE(imageFilter != nullptr);
 
    auto imageFilter1 = ImageFilter::CreateHDSampleImageFilter(nullptr, srcRect, dstRect, info);
    EXPECT_TRUE(imageFilter1 == nullptr);
 
    Rect srcRect1 {100.0f, 100.0f, 100.0f, 100.0f};
    auto imageFilter2 = ImageFilter::CreateHDSampleImageFilter(image, srcRect1, dstRect, info);
    EXPECT_TRUE(imageFilter2 == nullptr);
 
    Rect dstRect1 {10.0f, 10.0f, 10.0f, 10.0f};
    auto imageFilter3 = ImageFilter::CreateHDSampleImageFilter(image, srcRect, dstRect1, info);
    EXPECT_TRUE(imageFilter3 == nullptr);
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: UnmarshallingCompleteRoundTrip001
 * @tc.desc: Test complete round-trip marshalling and serialization data consistency
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterTest, UnmarshallingCompleteRoundTrip001, TestSize.Level1)
{
    // Test 1: Complete round-trip with multiple filter types
    std::vector<std::shared_ptr<ImageFilter>> testFilters;
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    // BlurFilter
    testFilters.push_back(ImageFilter::CreateBlurImageFilter(3.0f, 3.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect));
    // OffsetFilter
    testFilters.push_back(ImageFilter::CreateOffsetImageFilter(5.0f, 10.0f, nullptr, cropRect));

    // ColorFilterImageFilter
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, BlendMode::MULTIPLY);
    if (colorFilter) {
        testFilters.push_back(ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr, cropRect));
    }

    for (auto& originalFilter : testFilters) {
        ASSERT_NE(originalFilter, nullptr);
        auto originalType = originalFilter->GetType();

        // Marshal and unmarshal
        Parcel parcel;
        bool marshalResult = originalFilter->Marshalling(parcel);
        EXPECT_TRUE(marshalResult);

        bool isValid = true;
        auto unmarshaledFilter = ImageFilter::Unmarshalling(parcel, isValid);
        EXPECT_NE(unmarshaledFilter, nullptr);
        EXPECT_TRUE(isValid);
        EXPECT_EQ(unmarshaledFilter->GetType(), originalType);

        // Verify data serialization consistency
        auto originalData = originalFilter->Serialize();
        auto unmarshaledData = unmarshaledFilter->Serialize();
        if (originalData && unmarshaledData) {
            EXPECT_EQ(originalData->GetSize(), unmarshaledData->GetSize());
            const uint8_t* originalBytes = static_cast<const uint8_t*>(originalData->GetData());
            const uint8_t* unmarshaledBytes = static_cast<const uint8_t*>(unmarshaledData->GetData());
            int memResult = memcmp(originalBytes, unmarshaledBytes, originalData->GetSize());
            EXPECT_EQ(memResult, 0);
        } else {
            // If either serialization fails, both should fail consistently
            EXPECT_EQ(originalData, unmarshaledData);
        }
    }
}

/*
 * @tc.name: UnmarshallingErrorHandling001
 * @tc.desc: Test ImageFilter::Unmarshalling error handling scenarios and boundary conditions
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageFilterTest, UnmarshallingErrorHandling001, TestSize.Level1)
{
    // Test 1: Empty parcel
    {
        Parcel emptyParcel;
        bool isValid = true;
        auto result = ImageFilter::Unmarshalling(emptyParcel, isValid);
        EXPECT_EQ(result, nullptr);
    }

    // Test 2: NO_TYPE - 1 (negative boundary)
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::NO_TYPE) - 1);
        bool isValid = true;
        auto result = ImageFilter::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected due to invalid type
    }

    // Test 3: NO_TYPE (lower boundary) - can construct empty ImageFilter
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::NO_TYPE));
        parcel.WriteInt32(false);
        bool isValid = true;
        auto result = ImageFilter::Unmarshalling(parcel, isValid);
        EXPECT_NE(result, nullptr); // Should succeed, creating empty ImageFilter
        EXPECT_TRUE(isValid);
        if (result) {
            EXPECT_EQ(result->GetType(), ImageFilter::FilterType::NO_TYPE);
        }
    }

    // Test 4: LAZY_IMAGE_FILTER (should be rejected in normal ImageFilter unmarshalling)
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::LAZY_IMAGE_FILTER));
        bool isValid = true;
        auto result = ImageFilter::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected
    }

    // Test 5: Beyond LAZY_IMAGE_FILTER (upper boundary + 1)
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::LAZY_IMAGE_FILTER) + 1);
        bool isValid = true;
        auto result = ImageFilter::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected due to invalid type
    }

    // Test 6: Large invalid value
    {
        Parcel parcel;
        parcel.WriteInt32(999);
        bool isValid = true;
        auto result = ImageFilter::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected
    }
}

/*
 * @tc.name: MarshallingEmptyData001
 * @tc.desc: Test ImageFilter::Marshalling with empty Serialize data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, MarshallingEmptyData001, TestSize.Level1)
{
    auto mockFilter = std::make_shared<MockImageFilter>();
    Parcel parcel;
    // Should succeed even with null Serialize data
    bool result = mockFilter->Marshalling(parcel);
    EXPECT_TRUE(result);

    // Verify the parcel contains the expected structure
    // Read type
    int32_t type;
    EXPECT_TRUE(parcel.ReadInt32(type));
    EXPECT_EQ(type, static_cast<int32_t>(ImageFilter::FilterType::BLUR));

    // Read hasData flag - should be false
    bool hasData;
    EXPECT_TRUE(parcel.ReadBool(hasData));
    EXPECT_FALSE(hasData);

    // No more data should be available since hasData was false
    EXPECT_EQ(parcel.GetDataSize() - parcel.GetReadPosition(), 0);
}

/*
 * @tc.name: UnmarshallingEmptyData001
 * @tc.desc: Test ImageFilter::Unmarshalling with empty data marker
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, UnmarshallingEmptyData001, TestSize.Level1)
{
    Parcel parcel;
    // Write type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::BLUR)));
    // Write hasData as false to simulate empty data scenario
    EXPECT_TRUE(parcel.WriteBool(false));

    // Should successfully create empty filter
    bool isValid = true;
    auto filter = ImageFilter::Unmarshalling(parcel, isValid);
    EXPECT_NE(filter, nullptr);
    EXPECT_TRUE(isValid);
    EXPECT_EQ(filter->GetType(), ImageFilter::FilterType::BLUR);
}

/*
 * @tc.name: MarshallingUnmarshallingEmptyData001
 * @tc.desc: Test round-trip Marshalling and Unmarshalling with empty data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, MarshallingUnmarshallingEmptyData001, TestSize.Level1)
{
    auto originalFilter = std::make_shared<MockImageFilter>();
    Parcel parcel;
    // Marshal - should succeed with empty data
    bool marshalResult = originalFilter->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal - should create empty filter with correct type
    bool isValid = true;
    auto unmarshaledFilter = ImageFilter::Unmarshalling(parcel, isValid);
    EXPECT_NE(unmarshaledFilter, nullptr);
    EXPECT_TRUE(isValid);
    EXPECT_EQ(unmarshaledFilter->GetType(), ImageFilter::FilterType::BLUR);

    // Serialize validation - both should return null consistently
    auto originalData = originalFilter->Serialize();
    auto unmarshaledData = unmarshaledFilter->Serialize();
    EXPECT_EQ(originalData, nullptr);
    EXPECT_EQ(unmarshaledData, nullptr);
}

/*
 * @tc.name: MarshallingCallbackFailure001
 * @tc.desc: Test ImageFilter::Marshalling with callback failure to cover (!callback(parcel, data)) branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, MarshallingCallbackFailure001, TestSize.Level1)
{
    // Create a valid filter with non-null Serialize data
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto filter = ImageFilter::CreateBlurImageFilter(5.0f, 5.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(filter, nullptr);

    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataMarshallingCallback();

    // Set a failing callback to trigger the (!callback(parcel, data)) branch
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return false; // Always fail
        }
    );

    Parcel parcel;
    bool result = filter->Marshalling(parcel);
    // Should fail due to callback failure
    EXPECT_FALSE(result);

    // Restore original callback
    ObjectHelper::Instance().SetDataMarshallingCallback(originalCallback);
}

/*
 * @tc.name: UnmarshallingCallbackNull001
 * @tc.desc: Test ImageFilter::Unmarshalling with null callback to cover (if (!callback)) branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, UnmarshallingCallbackNull001, TestSize.Level1)
{
    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Set null callback to trigger the (if (!callback)) branch
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);

    Parcel parcel;
    // Write valid type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::BLUR)));
    // Write hasData as true to reach the callback check
    EXPECT_TRUE(parcel.WriteBool(true));

    bool isValid = true;
    auto result = ImageFilter::Unmarshalling(parcel, isValid);
    // Should fail due to null callback
    EXPECT_EQ(result, nullptr);

    // Restore original callback
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalCallback);
}

/*
 * @tc.name: CreateBlurImageFilterLazyInput001
 * @tc.desc: Test ImageFilter::CreateBlurImageFilter with lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, CreateBlurImageFilterLazyInput001, TestSize.Level1)
{
    // Create a lazy filter as input
    auto lazyFilter = ImageFilterLazy::CreateBlur(2.0f, 3.0f, TileMode::CLAMP);
    ASSERT_NE(lazyFilter, nullptr);
    EXPECT_TRUE(lazyFilter->IsLazy());

    // CreateBlurImageFilter should return nullptr with lazy input
    auto result = ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, TileMode::CLAMP, lazyFilter);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: CreateColorFilterImageFilterLazyInput001
 * @tc.desc: Test ImageFilter::CreateColorFilterImageFilter with lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, CreateColorFilterImageFilterLazyInput001, TestSize.Level1)
{
    // Create a lazy filter as input
    auto lazyFilter = ImageFilterLazy::CreateOffset(5.0f, 5.0f);
    ASSERT_NE(lazyFilter, nullptr);
    EXPECT_TRUE(lazyFilter->IsLazy());

    // Create a color filter
    auto colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    ASSERT_NE(colorFilter, nullptr);

    // CreateColorFilterImageFilter should return nullptr with lazy input
    auto result = ImageFilter::CreateColorFilterImageFilter(*colorFilter, lazyFilter);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: CreateOffsetImageFilterLazyInput001
 * @tc.desc: Test ImageFilter::CreateOffsetImageFilter with lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, CreateOffsetImageFilterLazyInput001, TestSize.Level1)
{
    // Create a lazy filter as input
    auto lazyFilter = ImageFilterLazy::CreateBlur(3.0f, 3.0f, TileMode::CLAMP);
    ASSERT_NE(lazyFilter, nullptr);
    EXPECT_TRUE(lazyFilter->IsLazy());

    // CreateOffsetImageFilter should return nullptr with lazy input
    auto result = ImageFilter::CreateOffsetImageFilter(10.0f, 10.0f, lazyFilter);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: CreateGradientBlurImageFilterLazyInput001
 * @tc.desc: Test ImageFilter::CreateGradientBlurImageFilter with lazy input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, CreateGradientBlurImageFilterLazyInput001, TestSize.Level1)
{
    // Create a lazy filter as input
    auto lazyFilter = ImageFilterLazy::CreateOffset(2.0f, 2.0f);
    ASSERT_NE(lazyFilter, nullptr);
    EXPECT_TRUE(lazyFilter->IsLazy());

    // CreateGradientBlurImageFilter should return nullptr with lazy input
    std::vector<std::pair<float, float>> fractionStops = {{0.0f, 0.5f}, {1.0f, 1.0f}};
    auto result = ImageFilter::CreateGradientBlurImageFilter(5.0f, fractionStops,
        GradientDir::LEFT, GradientBlurType::ALPHA_BLEND, lazyFilter);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: CreateArithmeticImageFilterLazyInput001
 * @tc.desc: Test ImageFilter::CreateArithmeticImageFilter with lazy inputs
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, CreateArithmeticImageFilterLazyInput001, TestSize.Level1)
{
    // Create lazy filters as inputs
    auto lazyBackground = ImageFilterLazy::CreateBlur(1.0f, 1.0f, TileMode::CLAMP);
    auto lazyForeground = ImageFilterLazy::CreateOffset(2.0f, 2.0f);
    ASSERT_NE(lazyBackground, nullptr);
    ASSERT_NE(lazyForeground, nullptr);
    EXPECT_TRUE(lazyBackground->IsLazy());
    EXPECT_TRUE(lazyForeground->IsLazy());

    // CreateArithmeticImageFilter should return nullptr with lazy background
    std::vector<scalar> coefficients = {1.0f, 0.0f, 0.0f, 0.0f};
    auto result1 = ImageFilter::CreateArithmeticImageFilter(coefficients, true, lazyBackground, nullptr);
    EXPECT_EQ(result1, nullptr);

    // CreateArithmeticImageFilter should return nullptr with lazy foreground
    auto result2 = ImageFilter::CreateArithmeticImageFilter(coefficients, true, nullptr, lazyForeground);
    EXPECT_EQ(result2, nullptr);
}

/*
 * @tc.name: CreateComposeImageFilterLazyInput001
 * @tc.desc: Test ImageFilter::CreateComposeImageFilter with lazy inputs
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, CreateComposeImageFilterLazyInput001, TestSize.Level1)
{
    // Create lazy filters as inputs
    auto lazyFilter1 = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::CLAMP);
    auto lazyFilter2 = ImageFilterLazy::CreateOffset(3.0f, 3.0f);
    ASSERT_NE(lazyFilter1, nullptr);
    ASSERT_NE(lazyFilter2, nullptr);
    EXPECT_TRUE(lazyFilter1->IsLazy());
    EXPECT_TRUE(lazyFilter2->IsLazy());

    // CreateComposeImageFilter should return nullptr with lazy f1
    auto result1 = ImageFilter::CreateComposeImageFilter(lazyFilter1, nullptr);
    EXPECT_EQ(result1, nullptr);

    // CreateComposeImageFilter should return nullptr with lazy f2
    auto result2 = ImageFilter::CreateComposeImageFilter(nullptr, lazyFilter2);
    EXPECT_EQ(result2, nullptr);

    // CreateComposeImageFilter should return nullptr with both lazy filters
    auto result3 = ImageFilter::CreateComposeImageFilter(lazyFilter1, lazyFilter2);
    EXPECT_EQ(result3, nullptr);
}

/*
 * @tc.name: CreateBlendImageFilterLazyInput001
 * @tc.desc: Test ImageFilter::CreateBlendImageFilter with lazy inputs
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, CreateBlendImageFilterLazyInput001, TestSize.Level1)
{
    // Create lazy filters as inputs
    auto lazyBackground = ImageFilterLazy::CreateOffset(1.0f, 2.0f);
    auto lazyForeground = ImageFilterLazy::CreateBlur(3.0f, 4.0f, TileMode::CLAMP);
    ASSERT_NE(lazyBackground, nullptr);
    ASSERT_NE(lazyForeground, nullptr);
    EXPECT_TRUE(lazyBackground->IsLazy());
    EXPECT_TRUE(lazyForeground->IsLazy());

    // CreateBlendImageFilter should return nullptr with lazy background
    auto result1 = ImageFilter::CreateBlendImageFilter(BlendMode::SRC_OVER, lazyBackground, nullptr);
    EXPECT_EQ(result1, nullptr);

    // CreateBlendImageFilter should return nullptr with lazy foreground
    auto result2 = ImageFilter::CreateBlendImageFilter(BlendMode::SRC_OVER, nullptr, lazyForeground);
    EXPECT_EQ(result2, nullptr);

    // CreateBlendImageFilter should return nullptr with both lazy filters
    auto result3 = ImageFilter::CreateBlendImageFilter(BlendMode::SRC_OVER, lazyBackground, lazyForeground);
    EXPECT_EQ(result3, nullptr);
}

/*
 * @tc.name: MarshallingWriteTypeFailure001
 * @tc.desc: Test ImageFilter::Marshalling with WriteInt32(type) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, MarshallingWriteTypeFailure001, TestSize.Level1)
{
    // Create a valid filter
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto filter = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(filter, nullptr);

    // Create buffer to fill parcel capacity (200K minimum)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Fill parcel completely, then try Marshalling (should fail on WriteInt32(type))
    Parcel parcel;
    parcel.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult = parcel.WriteBuffer(fillBuffer.data(), BUFFER_SIZE);
    EXPECT_TRUE(fillResult);

    bool result = filter->Marshalling(parcel);
    EXPECT_FALSE(result); // Should fail due to WriteInt32 failure
}

/*
 * @tc.name: MarshallingWriteHasDataFailure001
 * @tc.desc: Test ImageFilter::Marshalling with WriteBool(hasValidData) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, MarshallingWriteHasDataFailure001, TestSize.Level1)
{
    // Create a valid filter
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto filter = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(filter, nullptr);

    // Create buffer to fill parcel capacity (200K minimum)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Fill parcel leaving space for int32 only (4 bytes), should fail on WriteBool
    Parcel parcel;
    parcel.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult = parcel.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 4);
    EXPECT_TRUE(fillResult);

    bool result = filter->Marshalling(parcel);
    EXPECT_FALSE(result); // Should fail due to WriteBool failure
}

/*
 * @tc.name: MarshallingCallbackNull001
 * @tc.desc: Test ImageFilter::Marshalling with null DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, MarshallingCallbackNull001, TestSize.Level1)
{
    // Create a valid filter
    Rect cropRect(0.0f, 0.0f, 100.0f, 100.0f);
    auto filter = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f, TileMode::CLAMP, nullptr,
        ImageBlurType::GAUSS, cropRect);
    ASSERT_NE(filter, nullptr);

    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataMarshallingCallback();

    // Set null callback to trigger the (if (!callback)) branch
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);

    Parcel parcel;
    bool result = filter->Marshalling(parcel);
    EXPECT_FALSE(result); // Should fail due to null callback

    // Restore original callback
    ObjectHelper::Instance().SetDataMarshallingCallback(originalCallback);
}

/*
 * @tc.name: UnmarshallingReadHasDataFailure001
 * @tc.desc: Test ImageFilter::Unmarshalling with ReadBool(hasData) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, UnmarshallingReadHasDataFailure001, TestSize.Level1)
{
    Parcel parcel;
    // Write valid type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::BLUR)));
    // Don't write hasData bool, leaving parcel incomplete

    bool isValid = true;
    auto result = ImageFilter::Unmarshalling(parcel, isValid);
    EXPECT_EQ(result, nullptr); // Should fail due to ReadBool failure
}

/*
 * @tc.name: UnmarshallingCallbackReturnNull001
 * @tc.desc: Test ImageFilter::Unmarshalling with callback returning null data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ImageFilterTest, UnmarshallingCallbackReturnNull001, TestSize.Level1)
{
    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Set callback that returns null data to trigger the (if (!data)) branch
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            return nullptr; // Always return null
        }
    );

    Parcel parcel;
    // Write valid type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ImageFilter::FilterType::BLUR)));
    // Write hasData as true to reach the callback
    EXPECT_TRUE(parcel.WriteBool(true));

    bool isValid = true;
    auto result = ImageFilter::Unmarshalling(parcel, isValid);
    EXPECT_EQ(result, nullptr); // Should fail due to null data from callback

    // Restore original callback
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalCallback);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
