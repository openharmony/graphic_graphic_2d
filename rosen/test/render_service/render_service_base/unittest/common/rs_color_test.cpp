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

#include <cmath>

#include "gtest/gtest.h"

#include "common/rs_color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorTest::SetUpTestCase() {}
void RSColorTest::TearDownTestCase() {}
void RSColorTest::SetUp() {}
void RSColorTest::TearDown() {}

/**
 * @tc.name: Operator001
 * @tc.desc: test results of operator/
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, Operator001, TestSize.Level1)
{
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    // operator/ is overridden to do nothing when scale is equal to 0
    float scale = 0;
    RSColor scaledColor = color / scale;
    EXPECT_EQ(scaledColor.GetRed(), color.GetRed());
    EXPECT_EQ(scaledColor.GetGreen(), color.GetGreen());
    EXPECT_EQ(scaledColor.GetBlue(), color.GetBlue());
    EXPECT_EQ(scaledColor.GetAlpha(), color.GetAlpha());

    scale = 3;
    scaledColor = color / scale;
    int16_t scaledRed = 3;
    int16_t scaledGreen = 4;
    int16_t scaledBlue = 4;
    int16_t scaledAlpha = 4;
    EXPECT_EQ(scaledColor.GetRed(), scaledRed);
    EXPECT_EQ(scaledColor.GetGreen(), scaledGreen);
    EXPECT_EQ(scaledColor.GetBlue(), scaledBlue);
    EXPECT_EQ(scaledColor.GetAlpha(), scaledAlpha);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: verify function IsNearEqual
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, IsNearEqual001, TestSize.Level1)
{
    int16_t red = 100;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);

    int16_t threshold = 2;

    int16_t red1 = 101;
    RSColor otherColor1(red1, green, blue, alpha);
    EXPECT_TRUE(color.IsNearEqual(otherColor1, threshold));
    otherColor1.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_FALSE(color.IsNearEqual(otherColor1, threshold));

    int16_t red2 = 150;
    RSColor otherColor2(red2, green, blue, alpha);
    EXPECT_FALSE(color.IsNearEqual(otherColor2, threshold));
}

/**
 * @tc.name: RSColorCreateTest
 * @tc.desc: Verify function RSColor
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, RSColorCreateTest, TestSize.Level1)
{
    std::shared_ptr<RSColor> color;
    color = std::make_shared<RSColor>();
    EXPECT_FALSE(color == nullptr);
    color = std::make_shared<RSColor>(0);
    EXPECT_FALSE(color == nullptr);
    // for test
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    color = std::make_shared<RSColor>(red, green, blue);
    EXPECT_FALSE(color == nullptr);
}

/**
 * @tc.name: operatorTest002
 * @tc.desc: Verify function operator== operator+ operator- operator* operator*=
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, operatorTest002, TestSize.Level1)
{
    // for test
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    RSColor colorTest;
    float scale = 0;
    EXPECT_FALSE(color == colorTest);
    EXPECT_TRUE(color == (color + colorTest));
    EXPECT_TRUE(color == (color - colorTest));
    EXPECT_FALSE(color == (color * scale));
    EXPECT_TRUE(color == (color *= scale));
}

/**
 * @tc.name: AsRgbaIntTest
 * @tc.desc: Verify function AsRgbaInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, AsRgbaIntTest, TestSize.Level1)
{
    RSColor color(1);
    EXPECT_EQ(color.AsRgbaInt(), 1);
}

/**
 * @tc.name: FromRgbaIntTest
 * @tc.desc: Verify function FromRgbaInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, FromRgbaIntTest, TestSize.Level1)
{
    auto color = RSColor::FromRgbaInt(1);
    EXPECT_EQ(color.alpha_, 1);
}

/**
 * @tc.name: AsArgbIntTest
 * @tc.desc: Verify function AsArgbInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, AsArgbIntTest, TestSize.Level1)
{
    RSColor color(1);
    EXPECT_EQ(color.AsArgbInt(), 16777216);
}

/**
 * @tc.name: FromArgbIntTest
 * @tc.desc: Verify function FromArgbInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, FromArgbIntTest, TestSize.Level1)
{
    auto color = RSColor::FromArgbInt(1);
    EXPECT_EQ(color.alpha_, 0);
}

/**
 * @tc.name: AsBgraIntTest
 * @tc.desc: Verify function AsBgraInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, AsBgraIntTest, TestSize.Level1)
{
    RSColor color(1);
    EXPECT_EQ(color.AsBgraInt(), 1);
}

/**
 * @tc.name: FromBgraIntTest
 * @tc.desc: Verify function FromBgraInt
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, FromBgraIntTest, TestSize.Level1)
{
    auto color = RSColor::FromBgraInt(1);
    EXPECT_EQ(color.alpha_, 0);
}

/**
 * @tc.name: BlueTest
 * @tc.desc: Verify function GetBlue SetBlue
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, BlueTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetBlue(), 0);
    color.SetBlue(1);
    EXPECT_EQ(color.GetBlue(), 1);
}

/**
 * @tc.name: GreenTest
 * @tc.desc: Verify function GetGreen SetGreen
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, GreenTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetGreen(), 0);
    color.SetGreen(1);
    EXPECT_EQ(color.GetGreen(), 1);
}

/**
 * @tc.name: RedTest
 * @tc.desc: Verify function GetRed SetRed
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, RedTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetRed(), 0);
    color.SetRed(1);
    EXPECT_EQ(color.GetRed(), 1);
}

/**
 * @tc.name: AlphaTest
 * @tc.desc: Verify function GetAlpha SetAlpha
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, AlphaTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetAlpha(), 0);
    color.SetAlpha(1);
    EXPECT_EQ(color.GetAlpha(), 1);
}

/**
 * @tc.name: RedFTest
 * @tc.desc: Verify function GetRedF
 * @tc.type: FUNC
 * @tc.require: issuesIC9OUH
 */
HWTEST_F(RSColorTest, RedFTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetRedF(), 0.f);
    color.SetRed(255);
    EXPECT_EQ(color.GetRedF(), 1.f);
}

/**
 * @tc.name: GreenFTest
 * @tc.desc: Verify function GetGreenF
 * @tc.type: FUNC
 * @tc.require: issuesIC9OUH
 */
HWTEST_F(RSColorTest, GreenFTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetGreenF(), 0.f);
    color.SetGreen(255);
    EXPECT_EQ(color.GetGreenF(), 1.f);
}

/**
 * @tc.name: BlueFTest
 * @tc.desc: Verify function GetBlueF
 * @tc.type: FUNC
 * @tc.require: issuesIC9OUH
 */
HWTEST_F(RSColorTest, BlueFTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetBlueF(), 0.f);
    color.SetBlue(255);
    EXPECT_EQ(color.GetBlueF(), 1.f);
}

/**
 * @tc.name: AlphaFTest
 * @tc.desc: Verify function GetAlphaF
 * @tc.type: FUNC
 * @tc.require: issuesIC9OUH
 */
HWTEST_F(RSColorTest, AlphaFTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetAlphaF(), 0.f);
    color.SetAlpha(255);
    EXPECT_EQ(color.GetAlphaF(), 1.f);
}

/**
 * @tc.name: GetColor4fTest
 * @tc.desc: Verify function GetColor4f
 * @tc.type: FUNC
 * @tc.require: issuesIC9OUH
 */
HWTEST_F(RSColorTest, GetColor4fTest, TestSize.Level1)
{
    RSColor color;
    auto color4f = color.GetColor4f();
    EXPECT_EQ(color4f.redF_, 0.f);
    EXPECT_EQ(color4f.greenF_, 0.f);
    EXPECT_EQ(color4f.blueF_, 0.f);
    EXPECT_EQ(color4f.alphaF_, 0.f);
    color.SetRed(255);
    color.SetGreen(255);
    color.SetBlue(255);
    color.SetAlpha(255);
    color4f = color.GetColor4f();
    EXPECT_EQ(color4f.redF_, 1.f);
    EXPECT_EQ(color4f.greenF_, 1.f);
    EXPECT_EQ(color4f.blueF_, 1.f);
    EXPECT_EQ(color4f.alphaF_, 1.f);
}

/**
 * @tc.name: ColorSpaceTest
 * @tc.desc: Verify function GetColorSpace SetColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesIC9OUH
 */
HWTEST_F(RSColorTest, ColorSpaceTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    color.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_EQ(color.GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/**
 * @tc.name: MultiplyAlphaTest
 * @tc.desc: Verify function MultiplyAlpha
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSColorTest, MultiplyAlphaTest, TestSize.Level1)
{
    RSColor color;
    color.MultiplyAlpha(1.0f);
    EXPECT_EQ(color.GetAlpha(), 0);
}
} // namespace OHOS::Rosen