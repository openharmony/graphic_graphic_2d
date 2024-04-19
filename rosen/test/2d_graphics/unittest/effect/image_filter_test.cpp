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

void ImageFilterTest::SetUpTestCase() {}
void ImageFilterTest::TearDownTestCase() {}
void ImageFilterTest::SetUp() {}
void ImageFilterTest::TearDown() {}

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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
