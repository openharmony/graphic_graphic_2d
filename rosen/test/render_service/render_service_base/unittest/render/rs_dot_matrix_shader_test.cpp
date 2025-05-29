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

#include "render/rs_dot_matrix_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDotMatrixShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDotMatrixShaderTest::SetUpTestCase() {}
void RSDotMatrixShaderTest::TearDownTestCase() {}
void RSDotMatrixShaderTest::SetUp() {}
void RSDotMatrixShaderTest::TearDown() {}

/**
 * @tc.name: CreateRSDotMatrixShaderTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, CreateRSDotMatrixShaderTest001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    EXPECT_EQ(testShader.GetShaderType(), RSShader::ShaderType::DOT_MATRIX);
}

/**
 * @tc.name: CreateRSDotMatrixShaderTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, CreateRSDotMatrixShaderTest002, TestSize.Level1)
{
    Drawing::Color color;
    RSDotMatrixShader testShader { color, 0.f, 0.f, color };
    EXPECT_EQ(testShader.GetShaderType(), RSShader::ShaderType::DOT_MATRIX);
}

/**
 * @tc.name: SetNormalParams001
 * @tc.desc: Verify function SetNormalParams
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetNormalParams001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    Drawing::Color color;
    testShader.SetNormalParams(color, 1.f, 1.f, color);
    EXPECT_NE(testShader.params_, nullptr);
}

/**
 * @tc.name: SetRotateEffect001
 * @tc.desc: Verify function SetRotateEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetRotateEffect001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RotateEffectParams params;
    testShader.SetRotateEffect(params);
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::ROTATE);
}

/**
 * @tc.name: SetRippleEffect001
 * @tc.desc: Verify function SetRippleEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetRippleEffect001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::RIPPLE);
}

/**
 * @tc.name: SetNoneEffect001
 * @tc.desc: Verify function SetRippleEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, SetNoneEffect001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::RIPPLE);
    testShader.SetNoneEffect();
    EXPECT_NE(testShader.params_, nullptr);
    EXPECT_EQ(testShader.params_->effectType_, DotMatrixEffectType::NONE);
}

/**
 * @tc.name: MakeAndGetDrawingShader001
 * @tc.desc: Verify function MakeDrawingShader and GetDrawingShader
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeAndGetDrawingShader001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    RectF rect;
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MarshallingAndUnmarshalling001
 * @tc.desc: Verify function Marshalling and Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MarshallingAndUnmarshalling001, TestSize.Level1)
{
    RSDotMatrixShader testShader;
    RippleEffectParams params;
    testShader.SetRippleEffect(params);
    Parcel parcel;
    EXPECT_TRUE(testShader.Marshalling(parcel));
    bool needReset = true;
    EXPECT_FALSE(testShader.Unmarshalling(parcel, needReset));
    EXPECT_FALSE(needReset);
}

/**
 * @tc.name: MakeDrawingShaderTest3281
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3281, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3282
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3282, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_GRAY;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3283
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3283, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_LTGRAY;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3284
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3284, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_WHITE;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3285
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3285, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLACK;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3286
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3286, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_DKGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3287
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3287, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_GRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3288
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3288, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_LTGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest3289
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest3289, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_WHITE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32810
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32810, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_RED;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32811
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32811, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_GREEN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32812
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32812, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLUE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32813
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32813, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32814
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32814, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_CYAN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32815
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32815, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_MAGENTA;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32816
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32816, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_FOREGROUND;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest328161
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest328161, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLACK;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32817
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32817, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_DKGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32818
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32818, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_GRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32819
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32819, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_LTGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32820
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32820, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_WHITE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32821
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32821, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_RED;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32822
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32822, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_GREEN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32823
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32823, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLUE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32824
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32824, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32825
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32825, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}
/**
 * @tc.name: MakeDrawingShaderTest32826
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32826, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_CYAN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32827
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32827, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_MAGENTA;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32828
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32828, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_FOREGROUND;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32829
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32829, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLACK;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32830
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32830, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_DKGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32831
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32831, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_GRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest328321
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest328321, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_LTGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32832
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32832, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_WHITE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32833
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32833, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_RED;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32834
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32834, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_GREEN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32835
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32835, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLUE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32836
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32836, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32837
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32837, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_CYAN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32838
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32838, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_MAGENTA;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32839
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32839, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_FOREGROUND;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32840
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32840, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_BLACK;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32841
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32841, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_DKGRAY;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32842
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32842, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_RED;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32843
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32843, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_GREEN;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32844
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32844, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_BLUE;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32845
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32845, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_YELLOW;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32846
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32846, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_CYAN;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32847
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32847, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_MAGENTA;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32848
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32848, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_FOREGROUND;
    float dotSpacing = 10.f;
    float dotRadius = 10.f;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32849
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32849, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32850
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32850, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32851
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32851, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32852
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32852, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32853
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32853, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_BLACK;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32854
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32854, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_DKGRAY;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32855
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32855, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_GRAY;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32856
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32856, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_LTGRAY;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32857
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32857, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_WHITE;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32858
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32858, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_RED;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32859
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32859, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_GREEN;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32860
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32860, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_BLUE;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32861
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32861, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_YELLOW;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32862
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32862, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_CYAN;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32863
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32863, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_MAGENTA;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32864
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32864, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_FOREGROUND;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32865
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32865, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLACK;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32866
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32866, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_DKGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32867
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32867, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_GRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32868
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32868, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_LTGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32869
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32869, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_WHITE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32870
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32870, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_RED;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32871
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32871, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_GREEN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);
    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32872
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32872, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLUE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32873
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32873, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);
    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32874
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32874, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_CYAN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32875
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32875, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_MAGENTA;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32876
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32876, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MAX;
    Drawing::Color bgColor = Drawing::Color::COLOR_FOREGROUND;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32877
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32877, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLACK;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32878
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32878, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_DKGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32879
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32879, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_GRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32880
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32880, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_LTGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32881
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32881, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_WHITE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32882
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32882, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_RED;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32883
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32883, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_GREEN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32884
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32884, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLUE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32885
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32885, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32886
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32886, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}
/**
 * @tc.name: MakeDrawingShaderTest32887
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32887, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_CYAN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32888
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32888, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_MAGENTA;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32889
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32889, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MIN;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_FOREGROUND;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32890
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32890, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLACK;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32891
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32891, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_DKGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32892
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32892, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_GRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32893
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32893, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_LTGRAY;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32894
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32894, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_WHITE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32895
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32895, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_RED;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32896
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32896, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_GREEN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32897
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32897, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_BLUE;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32898
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32898, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_YELLOW;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest32899
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest32899, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_CYAN;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest1
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest1, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_MAGENTA;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}

/**
 * @tc.name: MakeDrawingShaderTest2
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSDotMatrixShaderTest, MakeDrawingShaderTest2, TestSize.Level1)
{
    Drawing::Color dotColor = Drawing::Color::COLOR_TRANSPARENT;
    float dotSpacing = FLT_MAX;
    float dotRadius = FLT_MIN;
    Drawing::Color bgColor = Drawing::Color::COLOR_FOREGROUND;
    RSDotMatrixShader testShader(dotColor, dotSpacing, dotRadius, bgColor);

    RectF rect(0.f, 0.f, 2048.f, 2048.f);
    testShader.MakeDrawingShader(rect, 0.);
    EXPECT_EQ(testShader.GetDrawingShader(), nullptr);
}
} // namespace OHOS::Rosen
