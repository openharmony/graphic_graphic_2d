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
class RSShowingPropertiesFreezerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShowingPropertiesFreezerTest::SetUpTestCase() {}
void RSShowingPropertiesFreezerTest::TearDownTestCase() {}
void RSShowingPropertiesFreezerTest::SetUp() {}
void RSShowingPropertiesFreezerTest::TearDown() {}

/**
 * @tc.name: GetBoundsTest
 * @tc.desc: Verify the GetBounds
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetBoundsTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetBoundsTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result = canvasNode->GetShowingProperties().GetBounds();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value().x_, SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetBoundsTest end";
}

/**
 * @tc.name: GetFrameTest
 * @tc.desc: Verify the GetFrame
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetFrameTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetFrameTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetFrame(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result = canvasNode->GetShowingProperties().GetFrame();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value().x_, SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetFrameTest end";
}

/**
 * @tc.name: GetPositionZTest
 * @tc.desc: Verify the GetPositionZ
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetPositionZTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetPositionZTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetPositionZ(SHOWING_FLOAT_NUM);
    auto result = canvasNode->GetShowingProperties().GetPositionZ();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetPositionZTest end";
}

/**
 * @tc.name: GetPivotTest
 * @tc.desc: Verify the GetPivot
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetPivotTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetPivotTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetPivot(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result1 = canvasNode->GetShowingProperties().GetPivot();
    ASSERT_TRUE(result1.has_value());
    EXPECT_FLOAT_EQ(result1.value().x_, SHOWING_FLOAT_NUM);

    canvasNode->SetPivotZ(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetPivotZ();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value(), SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetPivotTest end";
}

/**
 * @tc.name: GetRotationTest
 * @tc.desc: Verify the GetRotation
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetRotationTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetRotationTest start";
    auto canvasNode = RSCanvasNode::Create();

    canvasNode->SetRotation(Quaternion(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result1 = canvasNode->GetShowingProperties().GetQuaternion();
    ASSERT_TRUE(result1.has_value());
    EXPECT_FLOAT_EQ(result1.value().x_, SHOWING_FLOAT_NUM);

    canvasNode->SetRotation(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetRotation();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetRotationX(SHOWING_FLOAT_NUM);
    auto result3 = canvasNode->GetShowingProperties().GetRotationX();
    ASSERT_TRUE(result3.has_value());
    EXPECT_FLOAT_EQ(result3.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetRotationY(SHOWING_FLOAT_NUM);
    auto result4 = canvasNode->GetShowingProperties().GetRotationY();
    ASSERT_TRUE(result4.has_value());
    EXPECT_FLOAT_EQ(result4.value(), SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetRotationTest end";
}

/**
 * @tc.name: GetCameraDistanceTest
 * @tc.desc: Verify the GetCameraDistance
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetCameraDistanceTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetCameraDistanceTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetCameraDistance(SHOWING_FLOAT_NUM);
    auto result = canvasNode->GetShowingProperties().GetCameraDistance();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetCameraDistanceTest end";
}

/**
 * @tc.name: GetTranslateTest
 * @tc.desc: Verify the GetTranslate
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetTranslateTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetTranslateTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetTranslate(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result1 = canvasNode->GetShowingProperties().GetTranslate();
    ASSERT_TRUE(result1.has_value());
    EXPECT_FLOAT_EQ(result1.value().x_, SHOWING_FLOAT_NUM);

    canvasNode->SetTranslateZ(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetTranslateZ();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value(), SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetTranslateTest end";
}

/**
 * @tc.name: GetScaleTest
 * @tc.desc: Verify the GetScale
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetScaleTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetScaleTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetScale(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result = canvasNode->GetShowingProperties().GetScale();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value().x_, SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetScaleTest end";
}

/**
 * @tc.name: GetScaleZTest
 * @tc.desc: Verify the GetScaleZ
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetScaleZTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetScaleZTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetScaleZ(5.f);
    auto result = canvasNode->GetShowingProperties().GetScaleZ();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), 5.f);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetScaleZTest end";
}

/**
 * @tc.name: GetSkewTest
 * @tc.desc: Verify the GetSkew
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetSkewTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetSkewTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(Vector3f(SHOWING_FLOAT_NUM, 0.f, 1.f));
    auto result = canvasNode->GetShowingProperties().GetSkew();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value().x_, SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetSkewTest end";
}

/**
 * @tc.name: GetPerspTest
 * @tc.desc: Verify the GetPersp
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetPerspTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetPerspTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result = canvasNode->GetShowingProperties().GetPersp();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value().x_, SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetPerspTest end";
}

/**
 * @tc.name: GetAlphaTest
 * @tc.desc: Verify the GetAlpha
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetAlphaTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetAlphaTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetAlpha(SHOWING_FLOAT_NUM);
    auto result = canvasNode->GetShowingProperties().GetAlpha();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value(), SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetAlphaTest end";
}

/**
 * @tc.name: GetCornerRadiusTest
 * @tc.desc: Verify the GetCornerRadius
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetCornerRadiusTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetCornerRadiusTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetCornerRadius(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result = canvasNode->GetShowingProperties().GetCornerRadius();
    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value().x_, SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetCornerRadiusTest end";
}

/**
 * @tc.name: GetColorTest
 * @tc.desc: Verify the GetColor
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetColorTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetColorTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetForegroundColor(SK_ColorRED);
    auto result1 = canvasNode->GetShowingProperties().GetForegroundColor();
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value().GetRed() == SHOWING_COLOR_NUM);

    RSColor color = Color::FromArgbInt(SK_ColorRED);
    color.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    canvasNode->SetBackgroundColor(color);
    auto result2 = canvasNode->GetShowingProperties().GetBackgroundColor();
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value().GetRed() == SHOWING_COLOR_NUM);

    auto result3 = canvasNode->GetShowingProperties().GetSurfaceBgColor();
    EXPECT_TRUE(!result3.has_value());
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetColorTest end";
}

/**
 * @tc.name: GetBgImageTest
 * @tc.desc: Verify the GetBgImage
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetBgImageTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetBgImageTest start";
    auto canvasNode = RSCanvasNode::Create();

    canvasNode->SetBgImageWidth(SHOWING_FLOAT_NUM);
    auto result1 = canvasNode->GetShowingProperties().GetBgImageWidth();
    ASSERT_TRUE(result1.has_value());
    EXPECT_FLOAT_EQ(result1.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetBgImageHeight(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetBgImageHeight();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetBgImagePositionX(SHOWING_FLOAT_NUM);
    auto result3 = canvasNode->GetShowingProperties().GetBgImagePositionX();
    ASSERT_TRUE(result3.has_value());
    EXPECT_FLOAT_EQ(result3.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetBgImagePositionY(SHOWING_FLOAT_NUM);
    auto result4 = canvasNode->GetShowingProperties().GetBgImagePositionY();
    ASSERT_TRUE(result4.has_value());
    EXPECT_FLOAT_EQ(result4.value(), SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetBgImageTest end";
}

/**
 * @tc.name: GetBorderTest
 * @tc.desc: Verify the GetBorder
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetBorderTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetBorderTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBorderColor(SK_ColorRED);
    auto result1 = canvasNode->GetShowingProperties().GetBorderColor();
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value()[0].GetRed() == SHOWING_COLOR_NUM);

    canvasNode->SetBorderWidth(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result2 = canvasNode->GetShowingProperties().GetBorderWidth();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value().x_, SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetBorderTest end";
}

/**
 * @tc.name: GetShadowTest
 * @tc.desc: Verify the GetShadow
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetShadowTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetShadowTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetShadowColor(SK_ColorRED);
    auto result1 = canvasNode->GetShowingProperties().GetShadowColor();
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value().GetRed() == SHOWING_COLOR_NUM);

    canvasNode->SetShadowOffsetX(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetShadowOffsetX();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetShadowOffsetY(SHOWING_FLOAT_NUM);
    auto result3 = canvasNode->GetShowingProperties().GetShadowOffsetY();
    ASSERT_TRUE(result3.has_value());
    EXPECT_FLOAT_EQ(result3.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetShadowAlpha(SHOWING_FLOAT_NUM);
    auto result4 = canvasNode->GetShowingProperties().GetShadowAlpha();
    ASSERT_TRUE(result4.has_value());
    EXPECT_FLOAT_EQ(result4.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetShadowElevation(SHOWING_FLOAT_NUM);
    auto result5 = canvasNode->GetShowingProperties().GetShadowElevation();
    ASSERT_TRUE(result5.has_value());
    EXPECT_FLOAT_EQ(result5.value(), SHOWING_FLOAT_NUM);

    canvasNode = RSCanvasNode::Create();
    canvasNode->SetShadowRadius(SHOWING_FLOAT_NUM);
    auto result6 = canvasNode->GetShowingProperties().GetShadowRadius();
    ASSERT_TRUE(result6.has_value());
    EXPECT_FLOAT_EQ(result6.value(), SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetShadowTest end";
}

/**
 * @tc.name: GetDegreeTest
 * @tc.desc: Verify the GetDegree
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetDegreeTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetDegreeTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetSpherizeDegree(SHOWING_FLOAT_NUM);
    auto result1 = canvasNode->GetShowingProperties().GetSpherizeDegree();
    ASSERT_TRUE(result1.has_value());
    EXPECT_FLOAT_EQ(result1.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetLightUpEffectDegree(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetLightUpEffectDegree();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value(), SHOWING_FLOAT_NUM);

    canvasNode->SetDynamicDimDegree(SHOWING_FLOAT_NUM);
    auto result3 = canvasNode->GetShowingProperties().GetDynamicDimDegree();
    ASSERT_TRUE(result3.has_value());
    EXPECT_FLOAT_EQ(result3.value(), SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetDegreeTest end";
}

/**
 * @tc.name: GetHDRUIBrightnessTest
 * @tc.desc: Get the brightness.
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetHDRUIBrightnessTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetHDRUIBrightnessTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetHDRUIBrightness(SHOWING_FLOAT_NUM);
    auto result1 = canvasNode->GetShowingProperties().GetHDRUIBrightness();
    ASSERT_TRUE(result1.has_value());
    EXPECT_FLOAT_EQ(result1.value(), SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetHDRUIBrightnessTest end";
}

/**
 * @tc.name: GetAttractionValueTest
 * @tc.desc: Verify the GetAttractionValue
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetAttractionValueTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetAttractionValueTest start";
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetAttractionEffect(SHOWING_FLOAT_NUM, Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result1 = canvasNode->GetShowingProperties().GetAttractionFractionValue();
    ASSERT_TRUE(result1.has_value());
    EXPECT_FLOAT_EQ(result1.value(), SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetAttractionDstPointValue();
    ASSERT_TRUE(result2.has_value());
    EXPECT_FLOAT_EQ(result2.value().x_, SHOWING_FLOAT_NUM);

    canvasNode = RSCanvasNode::Create();
    float showingFloatNumber = 200.f;
    canvasNode->SetAttractionEffectFraction(showingFloatNumber);
    auto result3 = canvasNode->GetShowingProperties().GetAttractionFractionValue();
    ASSERT_TRUE(result3.has_value());
    EXPECT_FLOAT_EQ(result3.value(), showingFloatNumber);

    canvasNode->SetAttractionEffectDstPoint(Vector2f(showingFloatNumber, 0.f));
    auto result4 = canvasNode->GetShowingProperties().GetAttractionDstPointValue();
    ASSERT_TRUE(result4.has_value());
    EXPECT_FLOAT_EQ(result4.value().x_, showingFloatNumber);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetAttractionValueTest end";
}
} // namespace Rosen
} // namespace OHOS