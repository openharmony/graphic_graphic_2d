/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <cstddef>

#include "gtest/gtest.h"

#include "effect/color_filter.h"
#ifdef ROSEN_OHOS
#include "effect_test_utils.h"
#include "transaction/rs_marshalling_helper.h"
#include "utils/object_helper.h"
#include <message_parcel.h>
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ColorFilterTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void ColorFilterTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}
void ColorFilterTest::SetUp() {}
void ColorFilterTest::TearDown() {}

/**
 * @tc.name: CreateBlendModeColorFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateBlendModeColorFilter001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(11, OHOS::Rosen::Drawing::BlendMode::CLEAR);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateBlendModeColorFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateBlendModeColorFilter002, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(10, OHOS::Rosen::Drawing::BlendMode::SRC);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateComposeColorFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateComposeColorFilter001, TestSize.Level1)
{
    ColorFilter colorFilter1(ColorFilter::FilterType::NO_TYPE);
    ColorFilter colorFilter2(ColorFilter::FilterType::MATRIX);
    auto colorFilter = ColorFilter::CreateComposeColorFilter(colorFilter1, colorFilter2);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateComposeColorFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateComposeColorFilter002, TestSize.Level1)
{
    ColorFilter colorFilter1(ColorFilter::FilterType::COMPOSE);
    ColorFilter colorFilter2(ColorFilter::FilterType::MATRIX);
    auto colorFilter = ColorFilter::CreateComposeColorFilter(colorFilter1, colorFilter2);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateMatrixColorFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateMatrixColorFilter001, TestSize.Level1)
{
    ColorMatrix colorMatrix;
    auto colorFilter = ColorFilter::CreateMatrixColorFilter(colorMatrix);
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateLinearToSrgbGamma001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateLinearToSrgbGamma001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateSrgbGammaToLinear001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateSrgbGammaToLinear001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: CreateLumaColorFilter001
 * @tc.desc: test for creating a ColorFilter that its type is luma.
 * @tc.type: FUNC
 * @tc.require: I73UXK
 */
HWTEST_F(ColorFilterTest, CreateLumaColorFilter001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateLumaColorFilter();
    EXPECT_FALSE(nullptr == colorFilter);
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, GetType001, TestSize.Level1)
{
    ColorFilter::FilterType filter = ColorFilter::FilterType::NO_TYPE;
    auto colorFilter = std::make_shared<ColorFilter>(filter);
    auto filterType = colorFilter->GetType();
    EXPECT_TRUE(filterType == ColorFilter::FilterType::NO_TYPE);
}

/**
 * @tc.name: GetType002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, GetType002, TestSize.Level1)
{
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::MATRIX);
    auto filterType = colorFilter->GetType();
    EXPECT_TRUE(filterType == ColorFilter::FilterType::MATRIX);
}

/**
 * @tc.name: Compose001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, Compose001, TestSize.Level1)
{
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::MATRIX);
    ASSERT_TRUE(colorFilter != nullptr);
    ColorFilter filter(ColorFilter::FilterType::BLEND_MODE);
    colorFilter->Compose(filter);
}

/**
 * @tc.name: Compose002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, Compose002, TestSize.Level1)
{
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR);
    ASSERT_TRUE(colorFilter != nullptr);
    ColorFilter filter(ColorFilter::FilterType::COMPOSE);
    colorFilter->Compose(filter);
}

/**
 * @tc.name: CreateInstance001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance001, TestSize.Level1)
{
    auto colorFilter =
        std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, 10, OHOS::Rosen::Drawing::BlendMode::DST);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance002, TestSize.Level1)
{
    auto colorFilter = std::make_unique<ColorFilter>(
        ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR, 2, OHOS::Rosen::Drawing::BlendMode::SRC);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance003, TestSize.Level1)
{
    ColorMatrix colorMatrix;
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, colorMatrix);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance004, TestSize.Level1)
{
    ColorMatrix colorMatrix;
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR, colorMatrix);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance005, TestSize.Level1)
{
    ColorFilter colorFilter1(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    ColorFilter colorFilter2(ColorFilter::FilterType::BLEND_MODE);
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE, colorFilter1, colorFilter2);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance006, TestSize.Level1)
{
    ColorFilter colorFilter1(ColorFilter::FilterType::BLEND_MODE);
    ColorFilter colorFilter2(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    auto colorFilter =
        std::make_unique<ColorFilter>(ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR, colorFilter1, colorFilter2);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance007, TestSize.Level1)
{
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::BLEND_MODE);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateInstance008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateInstance008, TestSize.Level1)
{
    auto colorFilter = std::make_unique<ColorFilter>(ColorFilter::FilterType::NO_TYPE);
    EXPECT_FALSE(colorFilter == nullptr);
}

/**
 * @tc.name: CreateLightingColorFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: IAZ845
 * @tc.author:
 */
HWTEST_F(ColorFilterTest, CreateLightingColorFilter001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateLightingColorFilter(0xff00c800, 0xff001400);
    EXPECT_FALSE(nullptr == colorFilter);
}

#ifdef ROSEN_OHOS
/**
 * @tc.name: MarshallingRoundTrip001
 * @tc.desc: Test complete Marshalling/Unmarshalling cycle for multiple filter types
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, MarshallingRoundTrip001, TestSize.Level1)
{
    // Test multiple filter types in one comprehensive test
    std::vector<std::shared_ptr<ColorFilter>> testFilters;

    // BlendMode filter
    testFilters.push_back(ColorFilter::CreateBlendModeColorFilter(0xFF00FF00, BlendMode::SRC_OVER));

    // Matrix filter with custom values
    ColorMatrix matrix;
    matrix.SetScale(1.5f, 0.8f, 2.0f, 1.0f);
    testFilters.push_back(ColorFilter::CreateMatrixColorFilter(matrix));

    // Lighting filter
    testFilters.push_back(ColorFilter::CreateLightingColorFilter(0xFF808080, 0x80404040));

    for (auto& origFilter : testFilters) {
        ASSERT_TRUE(origFilter != nullptr);

        // Get original serialized data
        auto origData = origFilter->Serialize();
        ASSERT_TRUE(origData != nullptr);
        ASSERT_GT(origData->GetSize(), 0u);

        Parcel parcel;
        // Test Marshalling
        ASSERT_TRUE(origFilter->Marshalling(parcel));

        // Test Unmarshalling
        bool isValid = true;
        auto newFilter = ColorFilter::Unmarshalling(parcel, isValid);
        ASSERT_TRUE(newFilter != nullptr);
        ASSERT_TRUE(isValid);
        EXPECT_EQ(newFilter->GetType(), origFilter->GetType());
        // Verify the deserialized filter produces the same data
        auto newData = newFilter->Serialize();
        ASSERT_TRUE(newData != nullptr);
        ASSERT_EQ(newData->GetSize(), origData->GetSize());
        // Compare memory content byte by byte
        EXPECT_EQ(memcmp(newData->GetData(), origData->GetData(), origData->GetSize()), 0);
    }
}

/**
 * @tc.name: MarshallingEmptyData001
 * @tc.desc: Test Marshalling/Unmarshalling with empty data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, MarshallingEmptyData001, TestSize.Level1)
{
    auto origFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::NO_TYPE);
    ASSERT_TRUE(origFilter != nullptr);

    Parcel parcel;
    // Test Marshalling
    ASSERT_TRUE(origFilter->Marshalling(parcel));

    // Test Unmarshalling
    bool isValid = true;
    auto newFilter = ColorFilter::Unmarshalling(parcel, isValid);
    ASSERT_TRUE(newFilter != nullptr);
    ASSERT_TRUE(isValid);
    EXPECT_EQ(newFilter->GetType(), ColorFilter::FilterType::NO_TYPE);
}

/**
 * @tc.name: MarshallingComposeFilter001
 * @tc.desc: Test Marshalling/Unmarshalling with Compose filter
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, MarshallingComposeFilter001, TestSize.Level1)
{
    // Create two filters to compose
    auto filter1 = ColorFilter::CreateBlendModeColorFilter(0xFF0000FF, BlendMode::MULTIPLY);
    auto filter2 = ColorFilter::CreateLightingColorFilter(0xFFFFFFFF, 0x00202020);
    ASSERT_TRUE(filter1 != nullptr);
    ASSERT_TRUE(filter2 != nullptr);
    auto origFilter = ColorFilter::CreateComposeColorFilter(*filter1, *filter2);
    ASSERT_TRUE(origFilter != nullptr);

    // Get original serialized data
    auto origData = origFilter->Serialize();
    ASSERT_TRUE(origData != nullptr);
    ASSERT_GT(origData->GetSize(), 0u);

    Parcel parcel;
    // Test Marshalling
    ASSERT_TRUE(origFilter->Marshalling(parcel));

    // Test Unmarshalling
    bool isValid = true;
    auto newFilter = ColorFilter::Unmarshalling(parcel, isValid);
    ASSERT_TRUE(newFilter != nullptr);
    ASSERT_TRUE(isValid);
    EXPECT_EQ(newFilter->GetType(), ColorFilter::FilterType::COMPOSE);

    // Verify the deserialized filter produces the same data
    auto newData = newFilter->Serialize();
    ASSERT_TRUE(newData != nullptr);
    ASSERT_EQ(newData->GetSize(), origData->GetSize());

    // Compare memory content byte by byte - this validates the complete round trip
    EXPECT_EQ(memcmp(newData->GetData(), origData->GetData(), origData->GetSize()), 0);
}

/**
 * @tc.name: MarshallingNoCallback001
 * @tc.desc: Test Marshalling when DataMarshallingCallback is not registered
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, MarshallingNoCallback001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF00FF00, BlendMode::SRC_OVER);
    ASSERT_TRUE(colorFilter != nullptr);

    // Save original callback
    auto originalCallback = ObjectHelper::Instance().GetDataMarshallingCallback();

    // Clear any existing callback
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);

    Parcel parcel;
    EXPECT_FALSE(colorFilter->Marshalling(parcel));

    // Restore original callback
    ObjectHelper::Instance().SetDataMarshallingCallback(originalCallback);
}

/**
 * @tc.name: MarshallingCallbackFailed001
 * @tc.desc: Test Marshalling when DataMarshallingCallback fails
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, MarshallingCallbackFailed001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF00FF00, BlendMode::SRC_OVER);
    ASSERT_TRUE(colorFilter != nullptr);

    // Save original callback
    auto originalCallback = ObjectHelper::Instance().GetDataMarshallingCallback();

    // Register a callback that always fails
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Data> data) -> bool {
            return false; // Always fail
        });

    Parcel parcel;
    EXPECT_FALSE(colorFilter->Marshalling(parcel));

    // Restore original callback
    ObjectHelper::Instance().SetDataMarshallingCallback(originalCallback);
}

/**
 * @tc.name: MarshallingWriteFailureTest001
 * @tc.desc: Test ColorFilter::Marshalling write branch failures by filling parcel to capacity
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, MarshallingWriteFailureTest001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::CreateBlendModeColorFilter(0xFF00FF00, BlendMode::SRC_OVER);
    ASSERT_TRUE(colorFilter != nullptr);

    // Create buffer to fill parcel capacity (200K)
    const size_t BUFFER_SIZE = 200 * 1024;
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Test 1: Fill parcel completely, then try Marshalling (should fail on WriteInt32(type))
    MessageParcel parcel1;
    parcel1.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult1 = parcel1.WriteBuffer(fillBuffer.data(), BUFFER_SIZE);
    EXPECT_TRUE(fillResult1);
    // Now parcel is full, Marshalling should fail on WriteInt32
    bool result1 = colorFilter->Marshalling(parcel1);
    EXPECT_FALSE(result1);

    // Test 2: Fill parcel leaving space for type only (4 bytes)
    MessageParcel parcel2;
    parcel2.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult2 = parcel2.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 4);
    EXPECT_TRUE(fillResult2);
    // Should fail on WriteBool(hasValidData)
    bool result2 = colorFilter->Marshalling(parcel2);
    EXPECT_FALSE(result2);
}

/**
 * @tc.name: UnmarshallingErrorHandling001
 * @tc.desc: Test Unmarshalling error handling scenarios
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, UnmarshallingErrorHandling001, TestSize.Level1)
{
    // Test 1: Invalid type
    {
        // Test negative type (below NO_TYPE)
        Parcel parcel;
        parcel.WriteInt32(-1); // Below NO_TYPE

        bool isValid = true;
        auto filter = ColorFilter::Unmarshalling(parcel, isValid);
        EXPECT_TRUE(filter == nullptr);

        // Test large invalid type (above LIGHTING)
        Parcel parcel2;
        parcel2.WriteInt32(999); // Above LIGHTING

        isValid = true;
        filter = ColorFilter::Unmarshalling(parcel2, isValid);
        EXPECT_TRUE(filter == nullptr);
    }

    // Test 2: Empty parcel (ReadInt32 fails)
    {
        Parcel emptyParcel;
        bool isValid = true;
        auto filter = ColorFilter::Unmarshalling(emptyParcel, isValid);
        EXPECT_TRUE(filter == nullptr);
    }

    // Test 3: ReadBool fails (incomplete parcel)
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ColorFilter::FilterType::BLEND_MODE));
        // Don't write bool, so ReadBool will fail
        bool isValid = true;
        auto filter = ColorFilter::Unmarshalling(parcel, isValid);
        EXPECT_TRUE(filter == nullptr);
    }
}

/**
 * @tc.name: UnmarshallingNoCallback001
 * @tc.desc: Test Unmarshalling when DataUnmarshallingCallback is not registered
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, UnmarshallingNoCallback001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(ColorFilter::FilterType::BLEND_MODE));
    parcel.WriteBool(true); // Has data

    // Save original callback
    auto originalCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Clear any existing callback
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);

    bool isValid = true;
    auto filter = ColorFilter::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(filter == nullptr);

    // Restore original callback
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalCallback);
}

/**
 * @tc.name: UnmarshallingCallbackReturnNull001
 * @tc.desc: Test Unmarshalling when DataUnmarshallingCallback returns null
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(ColorFilterTest, UnmarshallingCallbackReturnNull001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(ColorFilter::FilterType::BLEND_MODE));
    parcel.WriteBool(true); // Has data

    // Save original callback
    auto originalCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Register a callback that returns null
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Data> {
            return nullptr; // Always return null
        });

    bool isValid = true;
    auto filter = ColorFilter::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(filter == nullptr);

    // Restore original callback
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalCallback);
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
