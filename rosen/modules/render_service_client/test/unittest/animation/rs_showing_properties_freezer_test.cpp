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
#include "render/rs_light_up_effect_filter.h"
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
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value() == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->x_ == SHOWING_FLOAT_NUM);

    canvasNode->SetPivotZ(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetPivotZ();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->x_ == SHOWING_FLOAT_NUM);

    canvasNode->SetRotation(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetRotation();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetRotationX(SHOWING_FLOAT_NUM);
    auto result3 = canvasNode->GetShowingProperties().GetRotationX();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetRotationY(SHOWING_FLOAT_NUM);
    auto result4 = canvasNode->GetShowingProperties().GetRotationY();
    EXPECT_TRUE(result4.has_value());
    EXPECT_TRUE(result4.value() == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value() == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->x_ == SHOWING_FLOAT_NUM);

    canvasNode->SetTranslateZ(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetTranslateZ();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

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
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetScaleTest end";
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
    canvasNode->SetSkew(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result = canvasNode->GetShowingProperties().GetSkew();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
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
    canvasNode->SetPersp(Vector2f(SHOWING_FLOAT_NUM, 0.f));
    auto result = canvasNode->GetShowingProperties().GetPersp();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value() == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result->x_ == SHOWING_FLOAT_NUM);
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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->GetRed() == SHOWING_COLOR_NUM);

    canvasNode->SetBackgroundColor(SK_ColorRED);
    auto result2 = canvasNode->GetShowingProperties().GetBackgroundColor();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2->GetRed() == SHOWING_COLOR_NUM);

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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetBgImageHeight(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetBgImageHeight();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetBgImagePositionX(SHOWING_FLOAT_NUM);
    auto result3 = canvasNode->GetShowingProperties().GetBgImagePositionX();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetBgImagePositionY(SHOWING_FLOAT_NUM);
    auto result4 = canvasNode->GetShowingProperties().GetBgImagePositionY();
    EXPECT_TRUE(result4.has_value());
    EXPECT_TRUE(result4.value() == SHOWING_FLOAT_NUM);

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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value()[0].GetRed() == SHOWING_COLOR_NUM);

    canvasNode->SetBorderWidth(Vector4f(SHOWING_FLOAT_NUM, 0.f, 0.f, 0.f));
    auto result2 = canvasNode->GetShowingProperties().GetBorderWidth();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2->x_ == SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetBorderTest end";
}

/**
 * @tc.name: GetFilterTest
 * @tc.desc: Verify the GetFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSShowingPropertiesFreezerTest, GetFilterTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetFilterTest start";
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBackgroundFilter(filter);
    auto result1 = canvasNode->GetShowingProperties().GetBackgroundFilter();
    EXPECT_TRUE(result1.has_value());

    canvasNode->SetFilter(filter);
    auto result2 = canvasNode->GetShowingProperties().GetFilter();
    EXPECT_TRUE(result2.has_value());

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetFilterTest end";
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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->GetRed() == SHOWING_COLOR_NUM);

    canvasNode->SetShadowOffsetX(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetShadowOffsetX();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetShadowOffsetY(SHOWING_FLOAT_NUM);
    auto result3 = canvasNode->GetShowingProperties().GetShadowOffsetY();
    EXPECT_TRUE(result3.has_value());
    EXPECT_TRUE(result3.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetShadowAlpha(SHOWING_FLOAT_NUM);
    auto result4 = canvasNode->GetShowingProperties().GetShadowAlpha();
    EXPECT_TRUE(result4.has_value());
    EXPECT_TRUE(result4.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetShadowElevation(SHOWING_FLOAT_NUM);
    auto result5 = canvasNode->GetShowingProperties().GetShadowElevation();
    EXPECT_TRUE(result5.has_value());
    EXPECT_TRUE(result5.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetShadowRadius(SHOWING_FLOAT_NUM);
    auto result6 = canvasNode->GetShowingProperties().GetShadowRadius();
    EXPECT_TRUE(result6.has_value());
    EXPECT_TRUE(result6.value() == SHOWING_FLOAT_NUM);

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
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value() == SHOWING_FLOAT_NUM);

    canvasNode->SetLightUpEffectDegree(SHOWING_FLOAT_NUM);
    auto result2 = canvasNode->GetShowingProperties().GetLightUpEffectDegree();
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value() == SHOWING_FLOAT_NUM);

    GTEST_LOG_(INFO) << "RSShowingPropertiesFreezerTest GetDegreeTest end";
}
} // namespace Rosen
} // namespace OHOS