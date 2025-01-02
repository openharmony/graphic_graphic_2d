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

#include "common/rs_color.h"
#include "modifier/rs_showing_properties_freezer.h"
#include "render/rs_blur_filter.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

constexpr float SHOWING_FLOAT_NUM = 100.f;
constexpr int SHOWING_COLOR_NUM = 255;

namespace OHOS {
namespace Rosen {
class RSShowingPropertiesFreezerUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShowingPropertiesFreezerUnitTest::SetUpTestCase() {}
void RSShowingPropertiesFreezerUnitTest::TearDownTestCase() {}
void RSShowingPropertiesFreezerUnitTest::SetUp() {}
void RSShowingPropertiesFreezerUnitTest::TearDown() {}

/**
 * @tc.name: GetBoundsTest
 * @tc.desc: Verify the GetBounds
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetBoundsTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetBoundsTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBounds(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result = rsNode->GetShowingProperties().GetBounds();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetBoundsTest end";
}

/**
 * @tc.name: GetFrameTest
 * @tc.desc: Verify the GetFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetFrameTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetFrameTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFrame(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result = rsNode->GetShowingProperties().GetFrame();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetFrameTest end";
}

/**
 * @tc.name: GetPositionZTest
 * @tc.desc: Verify the GetPositionZ
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetPositionZTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetPositionZTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(SHOWING_FLOAT_NUM);
    auto result = rsNode->GetShowingProperties().GetPositionZ();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value() == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetPositionZTest end";
}

/**
 * @tc.name: GetPivotTest
 * @tc.desc: Verify the GetPivot
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetPivotTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetPivotTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivot(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result1 = rsNode->GetShowingProperties().GetPivot();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->x_ == SHOWING_FLOAT_NUM);

    rsNode->SetPivotZ(SHOWING_FLOAT_NUM);
    auto result2 = rsNode->GetShowingProperties().GetPivotZ();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetPivotTest end";
}

/**
 * @tc.name: GetRotationTest
 * @tc.desc: Verify the GetRotation
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetRotationTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetRotationTest start";
    auto rsNode = RSCanvasNode::Create();

    rsNode->SetRotation(Quaternion(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result1 = rsNode->GetShowingProperties().GetQuaternion();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->x_ == SHOWING_FLOAT_NUM);

    rsNode->SetRotation(SHOWING_FLOAT_NUM);
    auto result2 = rsNode->GetShowingProperties().GetRotation();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    rsNode->SetRotationX(SHOWING_FLOAT_NUM);
    auto result3 = rsNode->GetShowingProperties().GetRotationX();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == SHOWING_FLOAT_NUM);

    rsNode->SetRotationY(SHOWING_FLOAT_NUM);
    auto result4 = rsNode->GetShowingProperties().GetRotationY();
    EXPECT_TRUE(result4.has_value());
    EXPECT_TRUE(result4.value() == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetRotationTest end";
}

/**
 * @tc.name: GetCameraDistanceTest
 * @tc.desc: Verify the GetCameraDistance
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetCameraDistanceTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetCameraDistanceTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCameraDistance(SHOWING_FLOAT_NUM);
    auto result = rsNode->GetShowingProperties().GetCameraDistance();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value() == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetCameraDistanceTest end";
}

/**
 * @tc.name: GetTranslateTest
 * @tc.desc: Verify the GetTranslate
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetTranslateTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetTranslateTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslate(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result1 = rsNode->GetShowingProperties().GetTranslate();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->x_ == SHOWING_FLOAT_NUM);

    rsNode->SetTranslateZ(SHOWING_FLOAT_NUM);
    auto result2 = rsNode->GetShowingProperties().GetTranslateZ();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetTranslateTest end";
}

/**
 * @tc.name: GetScaleTest
 * @tc.desc: Verify the GetScale
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetScaleTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetScaleTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result = rsNode->GetShowingProperties().GetScale();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetScaleTest end";
}

/**
 * @tc.name: GetSkewTest
 * @tc.desc: Verify the GetSkew
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetSkewTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetSkewTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSkew(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result = rsNode->GetShowingProperties().GetSkew();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetSkewTest end";
}

/**
 * @tc.name: GetPerspTest
 * @tc.desc: Verify the GetPersp
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetPerspTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetPerspTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPersp(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result = rsNode->GetShowingProperties().GetPersp();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetPerspTest end";
}

/**
 * @tc.name: GetAlphaTest
 * @tc.desc: Verify the GetAlpha
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetAlphaTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetAlphaTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(SHOWING_FLOAT_NUM);
    auto result = rsNode->GetShowingProperties().GetAlpha();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value() == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetAlphaTest end";
}

/**
 * @tc.name: GetCornerRadiusTest
 * @tc.desc: Verify the GetCornerRadius
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetCornerRadiusTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetCornerRadiusTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result = rsNode->GetShowingProperties().GetCornerRadius();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetCornerRadiusTest end";
}

/**
 * @tc.name: GetColorTest
 * @tc.desc: Verify the GetColor
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetColorTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetColorTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetForegroundColor(SK_ColorRED);
    auto result1 = rsNode->GetShowingProperties().GetForegroundColor();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->GetRed() == SHOWING_COLOR_NUM);

    rsNode->SetBackgroundColor(SK_ColorRED);
    auto result2 = rsNode->GetShowingProperties().GetBackgroundColor();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2->GetRed() == SHOWING_COLOR_NUM);

    auto result3 = rsNode->GetShowingProperties().GetSurfaceBgColor();
    EXPECT_TRUE(!result3.has_value());
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetColorTest end";
}

/**
 * @tc.name: GetBgImageTest
 * @tc.desc: Verify the GetBgImage
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetBgImageTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetBgImageTest start";
    auto rsNode = RSCanvasNode::Create();

    rsNode->SetBgImageWidth(SHOWING_FLOAT_NUM);
    auto result1 = rsNode->GetShowingProperties().GetBgImageWidth();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value() == SHOWING_FLOAT_NUM);

    rsNode->SetBgImageHeight(SHOWING_FLOAT_NUM);
    auto result2 = rsNode->GetShowingProperties().GetBgImageHeight();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    rsNode->SetBgImagePositionX(SHOWING_FLOAT_NUM);
    auto result3 = rsNode->GetShowingProperties().GetBgImagePositionX();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == SHOWING_FLOAT_NUM);

    rsNode->SetBgImagePositionY(SHOWING_FLOAT_NUM);
    auto result4 = rsNode->GetShowingProperties().GetBgImagePositionY();
    EXPECT_TRUE(result4.has_value());
    EXPECT_TRUE(result4.value() == SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetBgImageTest end";
}

/**
 * @tc.name: GetBorderTest
 * @tc.desc: Verify the GetBorder
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetBorderTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetBorderTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderColor(SK_ColorRED);
    auto result1 = rsNode->GetShowingProperties().GetBorderColor();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value()[0].GetRed() == SHOWING_COLOR_NUM);

    rsNode->SetBorderWidth(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result2 = rsNode->GetShowingProperties().GetBorderWidth();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2->x_ == SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetBorderTest end";
}

/**
 * @tc.name: GetFilterTest
 * @tc.desc: Verify the GetFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetFilterTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetFilterTest start";
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    RSCanvasNode::SharedPtr rsNode = RSCanvasNode::Create();
    rsNode->SetFilter(filter);
    auto result1 = rsNode->GetShowingProperties().GetFilter();
    EXPECT_NE(result1, nullptr);
 
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    rsNode->SetBackgroundFilter(backgroundFilter);
    auto result2 = rsNode->GetShowingProperties().GetBackgroundFilter();
    EXPECT_NE(result2, nullptr);
 
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetFilterTest end";
}

/**
 * @tc.name: GetShadowTest
 * @tc.desc: Verify the GetShadow
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetShadowTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetShadowTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowColor(SK_ColorRED);
    auto result1 = rsNode->GetShowingProperties().GetShadowColor();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->GetRed() == SHOWING_COLOR_NUM);

    rsNode->SetShadowOffsetX(SHOWING_FLOAT_NUM);
    auto result2 = rsNode->GetShowingProperties().GetShadowOffsetX();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    rsNode->SetShadowOffsetY(SHOWING_FLOAT_NUM);
    auto result3 = rsNode->GetShowingProperties().GetShadowOffsetY();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == SHOWING_FLOAT_NUM);

    rsNode->SetShadowAlpha(SHOWING_FLOAT_NUM);
    auto result4 = rsNode->GetShowingProperties().GetShadowAlpha();
    EXPECT_TRUE(result4.has_value());
    EXPECT_TRUE(result4.value() == SHOWING_FLOAT_NUM);

    rsNode->SetShadowElevation(SHOWING_FLOAT_NUM);
    auto result5 = rsNode->GetShowingProperties().GetShadowElevation();
    EXPECT_TRUE(result5.has_value());
    EXPECT_TRUE(result5.value() == SHOWING_FLOAT_NUM);

    rsNode->SetShadowRadius(SHOWING_FLOAT_NUM);
    auto result6 = rsNode->GetShowingProperties().GetShadowRadius();
    EXPECT_TRUE(result6.has_value());
    EXPECT_TRUE(result6.value() == SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetShadowTest end";
}

/**
 * @tc.name: GetDegreeTest
 * @tc.desc: Verify the GetDegree
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetDegreeTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetDegreeTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSpherizeDegree(SHOWING_FLOAT_NUM);
    auto result1 = rsNode->GetShowingProperties().GetSpherizeDegree();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value() == SHOWING_FLOAT_NUM);

    rsNode->SetLightUpEffectDegree(SHOWING_FLOAT_NUM);
    auto result2 = rsNode->GetShowingProperties().GetLightUpEffectDegree();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    rsNode->SetDynamicDimDegree(SHOWING_FLOAT_NUM);
    auto result3 = rsNode->GetShowingProperties().GetDynamicDimDegree();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetDegreeTest end";
}

/**
 * @tc.name: GetAttractionValueTest
 * @tc.desc: Verify the GetAttractionValue
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerUnitTest, GetAttractionValueTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetAttractionValueTest start";
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAttractionEffect(SHOWING_FLOAT_NUM, Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result1 = rsNode->GetShowingProperties().GetAttractionFractionValue();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value() == SHOWING_FLOAT_NUM);
    auto result2 = rsNode->GetShowingProperties().GetAttractionDstPointValue();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2->x_ == SHOWING_FLOAT_NUM);

    float showingFloatNumber = 200.f;
    rsNode->SetAttractionEffectFraction(showingFloatNumber);
    auto result3 = rsNode->GetShowingProperties().GetAttractionFractionValue();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == showingFloatNumber);
    
    rsNode->SetAttractionEffectDstPoint(Vector2f(showingFloatNumber, 0.f));
    auto result4 = rsNode->GetShowingProperties().GetAttractionDstPointValue();
    EXPECT_TRUE(result4.has_value());
    EXPECT_TRUE(result4->x_ == showingFloatNumber);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerUnitTest GetAttractionValueTest end";
}
} // namespace Rosen
} // namespace OHOS