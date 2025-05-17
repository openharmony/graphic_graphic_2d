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

void ColorFilterTest::SetUpTestCase() {}
void ColorFilterTest::TearDownTestCase() {}
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
