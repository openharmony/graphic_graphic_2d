/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "render/rs_linear_gradient_blur_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class LinearGradientBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void LinearGradientBlurFilterTest::SetUpTestCase() {}
void LinearGradientBlurFilterTest::TearDownTestCase() {}
void LinearGradientBlurFilterTest::SetUp() {}
void LinearGradientBlurFilterTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, testInterface, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    float width = 100;
    float height = 100;
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, width, height);
    EXPECT_TRUE(filter != nullptr);

    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image;

    filter->DrawImageRect(canvas, image, src, dst);
}

/**
 * @tc.name: ComputeScaleTest001
 * @tc.desc: Verify function ComputeScale
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, ComputeScaleTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    filter->ComputeScale(100, 100, true);
    EXPECT_EQ(filter->imageScale_, 1.f);
    filter->ComputeScale(100, 100, false);
    EXPECT_EQ(filter->imageScale_, 0.5f);
    filter->ComputeScale(10, 10, false);
    EXPECT_EQ(filter->imageScale_, 0.7f);
}

/**
 * @tc.name: CalcDirectionBiasTest001
 * @tc.desc: Verify function CalcDirectionBias
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, CalcDirectionBiasTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    Drawing::Matrix mat;
    uint8_t directionBias = 0;
    EXPECT_EQ(filter->CalcDirectionBias(mat), directionBias);
}

/**
 * @tc.name: TransformGradientBlurDirectionTest001
 * @tc.desc: Verify function TransformGradientBlurDirection
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, TransformGradientBlurDirectionTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    uint8_t direction = 5;
    uint8_t directionBias = 6;
    filter->TransformGradientBlurDirection(direction, directionBias);
    direction = 6;
    directionBias = 3;
    filter->TransformGradientBlurDirection(direction, directionBias);
    direction = 1;
    filter->TransformGradientBlurDirection(direction, directionBias);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: GetGradientDirectionPointsTest001
 * @tc.desc: Verify function GetGradientDirectionPoints
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, GetGradientDirectionPointsTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    Drawing::Point pts[2];
    Drawing::Rect clipBounds = { 1.f, 1.f, 1.f, 1.f };
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::BOTTOM);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::TOP);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::RIGHT);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::LEFT);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::RIGHT_BOTTOM);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::LEFT_TOP);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::LEFT_BOTTOM);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::RIGHT_TOP);
    filter->GetGradientDirectionPoints(pts, clipBounds, GradientDirection::LEFT);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: MakeAlphaGradientShaderTest001
 * @tc.desc: Verify function MakeAlphaGradientShader
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, MakeAlphaGradientShaderTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    Drawing::Rect clipBounds = { 1.f, 1.f, 1.f, 1.f };
    EXPECT_NE(filter->MakeAlphaGradientShader(clipBounds, linearGradientBlurPara, 1), nullptr);
    EXPECT_NE(filter->MakeAlphaGradientShader(clipBounds, linearGradientBlurPara, 0), nullptr);
}

/**
 * @tc.name: MakeHorizontalMeanBlurEffectTest001
 * @tc.desc: Verify function MakeHorizontalMeanBlurEffect
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, MakeHorizontalMeanBlurEffectTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    filter->MakeHorizontalMeanBlurEffect();
    EXPECT_NE(filter->horizontalMeanBlurShaderEffect_, nullptr);
}

/**
 * @tc.name: MakeVerticalMeanBlurEffectTest001
 * @tc.desc: Verify function MakeVerticalMeanBlurEffect
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, MakeVerticalMeanBlurEffectTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    filter->MakeVerticalMeanBlurEffect();
    EXPECT_NE(filter->verticalMeanBlurShaderEffect_, nullptr);
}

/**
 * @tc.name: DrawMeanLinearGradientBlurTest001
 * @tc.desc: Verify function DrawMeanLinearGradientBlur
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, DrawMeanLinearGradientBlurTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect dst = { 1.f, 1.f, 1.f, 1.f };
    std::shared_ptr<Drawing::Image>image=nullptr;
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    filter->DrawMeanLinearGradientBlur(image, canvas, 0.f, alphaGradientShader, dst);
    EXPECT_NE(filter->verticalMeanBlurShaderEffect_, nullptr);
}

/**
 * @tc.name: DrawMaskLinearGradientBlurTest001
 * @tc.desc: Verify function DrawMaskLinearGradientBlur
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, DrawMaskLinearGradientBlurTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    std::shared_ptr<Drawing::Image> imagef = nullptr;
    Drawing::Canvas canvas;
    Drawing::Rect dst = { 1.f, 1.f, 1.f, 1.f };
    auto image = std::make_shared<Drawing::Image>();
    auto blurFilter = std::static_pointer_cast<RSDrawingFilter>(linearGradientBlurPara->LinearGradientBlurFilter_);
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    filter->DrawMaskLinearGradientBlur(imagef, canvas, blurFilter, alphaGradientShader, dst);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_EQ(filter->GetDescription(), "RSLinearGradientBlurFilter");
}

/**
 * @tc.name: GetDetailedDescriptionTest001
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(LinearGradientBlurFilterTest, GetDetailedDescriptionTest001, TestSize.Level1)
{
    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, 100, 100);
    EXPECT_TRUE(filter != nullptr);
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    EXPECT_EQ(filter->GetDetailedDescription(),
        "RSLinearGradientBlurFilterBlur, radius: " + std::to_string(filter->linearGradientBlurPara_->blurRadius_));
}
} // namespace Rosen
} // namespace OHOS