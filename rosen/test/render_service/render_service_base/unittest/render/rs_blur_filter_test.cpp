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

#include "render/rs_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBlurFilterTest::SetUpTestCase() {}
void RSBlurFilterTest::TearDownTestCase() {}
void RSBlurFilterTest::SetUp() {}
void RSBlurFilterTest::TearDown() {}

/**
 * @tc.name: RSBlurFilterTest001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTest001, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float blurRadiusX1 = 1.2f;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);

    float threshold = 1.0f;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: RSBlurFilterTestFLT_MIN001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTestFLT_MIN001, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = FLT_MIN;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = FLT_MIN;

    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: RSBlurFilterTestFLT_MAX001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTestFLT_MAX001, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = FLT_MAX;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = FLT_MAX;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: RSBlurFilterTestFLT_MAX002
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTestFLT_MAX002, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MIN;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = FLT_MAX;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = FLT_MAX;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: RSBlurFilterTestFLT_MIN002
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTestFLT_MIN002, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = FLT_MAX;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = FLT_MAX;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: RSBlurFilterTestFLT_MIN003
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTestFLT_MIN003, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = FLT_MAX;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = FLT_MAX;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: RSBlurFilterTestFLT_MIN004
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTestFLT_MIN004, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = FLT_MAX;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = FLT_MIN;
    EXPECT_FALSE(filter->IsNearEqual(other1, threshold));
    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: RSBlurFilterTestFLT_MAX005
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, RSBlurFilterTestFLT_MAX005, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = FLT_MIN;
    auto other1 = std::make_shared<RSBlurFilter>(blurRadiusX1, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = FLT_MIN;
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
    EXPECT_TRUE(filter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: IsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsNearZero001, TestSize.Level1)
{
    float blurRadiusX = 0.2f;
    float blurRadiusY = 0.2f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);

    float threshold = 0.5f;
    EXPECT_TRUE(filter->IsNearZero(threshold));
}

/**
 * @tc.name: GetBlurRadiusXTest
 * @tc.desc: Verify function GetBlurRadiusX
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusXTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    float blurRadiusX1 = filter->GetBlurRadiusX();
    EXPECT_EQ(blurRadiusX, blurRadiusX1);
}

/**
 * @tc.name: GetBlurRadiusYTest
 * @tc.desc: Verify function GetBlurRadiusY
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusYTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    float blurRadiusY1 = filter->GetBlurRadiusY();
    EXPECT_EQ(blurRadiusY, blurRadiusY1);
}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetDescriptionTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_EQ(blurFilter->GetDescription(), "RSBlurFilter blur radius is " + std::to_string(blurRadiusX) + " sigma");
}

/**
 * @tc.name: IsValidTest
 * @tc.desc: Verify function IsValid
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, IsValidTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_TRUE(blurFilter->IsValid());
}

/**
 * @tc.name: ComposeTest
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, ComposeTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto composedFilter = blurFilter1->Compose(blurFilter2);
    EXPECT_NE(composedFilter, nullptr);
}

/**
 * @tc.name: AddTest
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, AddTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::DEFAULT, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: SubTest
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, SubTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::DEFAULT, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: MultiplyTest
 * @tc.desc: Verify function Multiply
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, MultiplyTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Multiply(1.0f);
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: NegateTest
 * @tc.desc: Verify function Negate
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, NegateTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Negate();
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: SetGreyCoefTest
 * @tc.desc: Verify function SetGreyCoef
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, SetGreyCoefTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    Vector2f vector2;
    std::optional<Vector2f> greyCoef = vector2;
    blurFilter->SetGreyCoef(greyCoef);
    EXPECT_EQ(blurFilter->greyCoef_, greyCoef);
}

/**
 * @tc.name: CanSkipFrameTest
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameTest, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_TRUE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: CanSkipFrameTest001
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameTest001, TestSize.Level1)
{
    float blurRadiusX = 0.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_FALSE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: CanSkipFrameTest002
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameTest002, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 0.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_FALSE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: GetDetailedDescriptionTest
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, GetDetailedDescriptionTest, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    blurFilter->GetDetailedDescription();
    EXPECT_EQ(blurFilter->GetDetailedDescription(),
        "RSBlurFilterBlur, radius: 27.000000 sigma, greyCoef1: 0.000000, greyCoef2: 0.000000");

    std::optional<Vector2f> greyCoef({ 1.f, 1.f });
    blurFilter->SetGreyCoef(greyCoef);
    EXPECT_EQ(blurFilter->GetDetailedDescription(),
        "RSBlurFilterBlur, radius: 27.000000 sigma, greyCoef1: 1.000000, greyCoef2: 1.000000");
}

/**
 * @tc.name: SubTest002
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubTest002, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    EXPECT_EQ(blurFilter->Sub(nullptr), blurFilter);
}

/**
 * @tc.name: SubTest003
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubTest003, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::DEFAULT, 1.0f);

    EXPECT_EQ(blurFilter->Sub(materialFilter), blurFilter);
    EXPECT_NE(blurFilter->Sub(blurFilter), blurFilter);
}

/**
 * @tc.name: IsEqualTest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsEqualTest001, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::DEFAULT, 1.0f);

    EXPECT_FALSE(blurFilter->IsEqual(materialFilter));
    EXPECT_TRUE(blurFilter->IsEqual(nullptr));
}

/**
 * @tc.name: DrawImageRect
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurFilterTest, DrawImageRect, TestSize.Level1)
{
    RSBlurFilter rsBlurFilter(0, 1);
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    rsBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(rsBlurFilter.greyCoef_.has_value());

    std::optional<Vector2f> greyCoef({ 1.f, 1.f });
    rsBlurFilter.SetGreyCoef(greyCoef);
    rsBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_NE(&rsBlurFilter, nullptr);
}

/********************************** */

/**
 * @tc.name: IsNearZeroFLT_MAX001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsNearZeroFLT_MAX001, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = 0.5f;
    EXPECT_FALSE(filter->IsNearZero(threshold));
}

/**
 * @tc.name: IsNearZeroFLT_MIN001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsNearZeroFLT_MIN001, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float threshold = 0.5f;
    EXPECT_TRUE(filter->IsNearZero(threshold));
}

/**
 * @tc.name: GetBlurRadiusXFLT_MINTest
 * @tc.desc: Verify function GetBlurRadiusX
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusXFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = filter->GetBlurRadiusX();
    EXPECT_EQ(blurRadiusX, blurRadiusX1);
}

/**
 * @tc.name: GetBlurRadiusXFLT_MAXTest
 * @tc.desc: Verify function GetBlurRadiusX
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusXFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusX1 = filter->GetBlurRadiusX();
    EXPECT_EQ(blurRadiusX, blurRadiusX1);
}

/**
 * @tc.name: GetBlurRadiusYFLT_MAXTest
 * @tc.desc: Verify function GetBlurRadiusY
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusYFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusY1 = filter->GetBlurRadiusY();
    EXPECT_EQ(blurRadiusY, blurRadiusY1);
}
/**
 * @tc.name: GetBlurRadiusYFLT_MINTest
 * @tc.desc: Verify function GetBlurRadiusY
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetBlurRadiusYFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto filter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(filter, nullptr);
    float blurRadiusY1 = filter->GetBlurRadiusY();
    EXPECT_EQ(blurRadiusY, blurRadiusY1);
}
/**
 * @tc.name: GetDescriptionFLT_MINTest
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetDescriptionFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_EQ(blurFilter->GetDescription(), "RSBlurFilter blur radius is " + std::to_string(blurRadiusX) + " sigma");
}

/**
 * @tc.name: GetDescriptionFLT_MAXTest
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, GetDescriptionFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_EQ(blurFilter->GetDescription(), "RSBlurFilter blur radius is " + std::to_string(blurRadiusX) + " sigma");
}

/**
 * @tc.name: IsValidFLT_MAXTest
 * @tc.desc: Verify function IsValid
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, IsValidFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_TRUE(blurFilter->IsValid());
}

/**
 * @tc.name: IsValidFLT_MINTest
 * @tc.desc: Verify function IsValid
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, IsValidFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_FALSE(blurFilter->IsValid());
}

/**
 * @tc.name: ComposeFLT_MINTest
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, ComposeFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter2, nullptr);
    auto composedFilter = blurFilter1->Compose(blurFilter2);
    EXPECT_NE(composedFilter, nullptr);
}

/**
 * @tc.name: ComposeFLT_MAXTest
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, ComposeFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter2, nullptr);
    auto composedFilter = blurFilter1->Compose(blurFilter2);
    EXPECT_NE(composedFilter, nullptr);
}

/**
 * @tc.name: ComposeFLT_MINTest327
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, ComposeFLT_MINTest327, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter2, nullptr);
    auto composedFilter = blurFilter1->Compose(blurFilter2);
    EXPECT_NE(composedFilter, nullptr);
}

/**
 * @tc.name: AddFASTAVERAGETest
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, AddFASTAVERAGETest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::FASTAVERAGE, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}
/**
 * @tc.name: AddAVERAGETest
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, AddAVERAGETest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::AVERAGE, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: AddPRE_DEFINEDTest
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, AddPRE_DEFINEDTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::PRE_DEFINED, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: SubPRE_DEFINEDTest
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, SubPRE_DEFINEDTest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::PRE_DEFINED, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: SubAVERAGETest
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, SubAVERAGETest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::AVERAGE, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: SubFASTAVERAGETest
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, SubFASTAVERAGETest, TestSize.Level1)
{
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    auto blurFilter2 = blurFilter1->Add(nullptr);
    EXPECT_NE(blurFilter2, nullptr);

    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::FASTAVERAGE, 1.0f);
    auto blurFilter3 = blurFilter1->Add(materialFilter);
    EXPECT_NE(blurFilter3, nullptr);

    auto rhs = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    auto blurFilter4 = blurFilter1->Add(rhs);
    EXPECT_NE(blurFilter4, nullptr);
}

/**
 * @tc.name: MultiplyFLT_MINTest
 * @tc.desc: Verify function Multiply
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, MultiplyFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = blurFilter1->Multiply(1.0f);
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: MultiplyFLT_MAXTest
 * @tc.desc: Verify function Multiply
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, MultiplyFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = blurFilter1->Multiply(1.0f);
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: NegateFLT_MAXTest
 * @tc.desc: Verify function Negate
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, NegateFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = blurFilter1->Negate();
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: NegateFLT_MINTest
 * @tc.desc: Verify function Negate
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, NegateFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter1 = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter1, nullptr);
    auto blurFilter2 = blurFilter1->Negate();
    EXPECT_NE(blurFilter2, nullptr);
}

/**
 * @tc.name: SetGreyCoefFLT_MINTest
 * @tc.desc: Verify function SetGreyCoef
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, SetGreyCoefFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    Vector2f vector2;
    std::optional<Vector2f> greyCoef = vector2;
    blurFilter->SetGreyCoef(greyCoef);
    EXPECT_EQ(blurFilter->greyCoef_, greyCoef);
}

/**
 * @tc.name: SetGreyCoefFLT_MAXTest
 * @tc.desc: Verify function SetGreyCoef
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, SetGreyCoefFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    Vector2f vector2;
    std::optional<Vector2f> greyCoef = vector2;
    blurFilter->SetGreyCoef(greyCoef);
    EXPECT_EQ(blurFilter->greyCoef_, greyCoef);
}

/**
 * @tc.name: CanSkipFrameFLT_MAXTest
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameFLT_MAXTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_TRUE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: CanSkipFrameFLT_MINTest
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameFLT_MINTest, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_FALSE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: CanSkipFrameFLT_MINTest001
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameFLT_MINTest001, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_FALSE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: CanSkipFrameTest001
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameFLT_MAXTest001, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_TRUE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: CanSkipFrameFLT_MAXTest002
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameFLT_MAXTest002, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_TRUE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: CanSkipFrameFLT_MINTest002
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, CanSkipFrameFLT_MINTest002, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_FALSE(blurFilter->CanSkipFrame());
}

/**
 * @tc.name: SubFLT_MINTest002
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubFLT_MINTest002, TestSize.Level1)
{
    float blurRadiusX = FLT_MIN;
    float blurRadiusY = FLT_MIN;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_EQ(blurFilter->Sub(nullptr), blurFilter);
}

/**
 * @tc.name: SubFLT_MAXTest002
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubFLT_MAXTest002, TestSize.Level1)
{
    float blurRadiusX = FLT_MAX;
    float blurRadiusY = FLT_MAX;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    EXPECT_EQ(blurFilter->Sub(nullptr), blurFilter);
}

/**
 * @tc.name: SubPRE_DEFINEDTest003
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubPRE_DEFINEDTest003, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::PRE_DEFINED, 1.0f);

    EXPECT_EQ(blurFilter->Sub(materialFilter), blurFilter);
    EXPECT_NE(blurFilter->Sub(blurFilter), blurFilter);
}

/**
 * @tc.name: SubAVERAGETest003
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubAVERAGETest003, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::AVERAGE, 1.0f);

    EXPECT_EQ(blurFilter->Sub(materialFilter), blurFilter);
    EXPECT_NE(blurFilter->Sub(blurFilter), blurFilter);
}

/**
 * @tc.name: SubFASTAVERAGETest003
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, SubFASTAVERAGETest003, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::FASTAVERAGE, 1.0f);

    EXPECT_EQ(blurFilter->Sub(materialFilter), blurFilter);
    EXPECT_NE(blurFilter->Sub(blurFilter), blurFilter);
}

/**
 * @tc.name: IsEqualPRE_DEFINEDTest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsEqualPRE_DEFINEDTest001, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::PRE_DEFINED, 1.0f);

    EXPECT_FALSE(blurFilter->IsEqual(materialFilter));
    EXPECT_TRUE(blurFilter->IsEqual(nullptr));
}

/**
 * @tc.name: IsEqualAVERAGETest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsEqualAVERAGETest001, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::AVERAGE, 1.0f);

    EXPECT_FALSE(blurFilter->IsEqual(materialFilter));
    EXPECT_TRUE(blurFilter->IsEqual(nullptr));
}

/**
 * @tc.name: IsEqualFASTAVERAGETest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSBlurFilterTest, IsEqualFASTAVERAGETest001, TestSize.Level1)
{
    float blurRadiusX = 27.0f;
    float blurRadiusY = 26.0f;
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadiusX, blurRadiusY);
    ASSERT_NE(blurFilter, nullptr);
    auto materialFilter = RSFilter::CreateMaterialFilter(1, 1.0f, BLUR_COLOR_MODE::FASTAVERAGE, 1.0f);

    EXPECT_FALSE(blurFilter->IsEqual(materialFilter));
    EXPECT_TRUE(blurFilter->IsEqual(nullptr));
}

/**
 * @tc.name: DrawImageRectFLT_MAX
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurFilterTest, DrawImageRectFLT_MAX, TestSize.Level1)
{
    RSBlurFilter rsBlurFilter(0, 1);
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    rsBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(rsBlurFilter.greyCoef_.has_value());

    std::optional<Vector2f> greyCoef({ FLT_MAX, FLT_MAX });
    rsBlurFilter.SetGreyCoef(greyCoef);
    rsBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_NE(&rsBlurFilter, nullptr);
}

/**
 * @tc.name: DrawImageRectFLT_MIN
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurFilterTest, DrawImageRectFLT_MIN, TestSize.Level1)
{
    RSBlurFilter rsBlurFilter(0, 1);
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    rsBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(rsBlurFilter.greyCoef_.has_value());

    std::optional<Vector2f> greyCoef({ FLT_MIN, FLT_MIN });
    rsBlurFilter.SetGreyCoef(greyCoef);
    rsBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_NE(&rsBlurFilter, nullptr);
}
} // namespace OHOS::Rosen
