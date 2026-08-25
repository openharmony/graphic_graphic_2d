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

/**
 * @tc.name: MultiplyAlphaNaNTest
 * @tc.desc: Verify MultiplyAlpha treats NaN alpha as 0 instead of corrupting alpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, MultiplyAlphaNaNTest, TestSize.Level1)
{
    RSColor color(10, 11, 12, 255);
    color.MultiplyAlpha(NAN);
    EXPECT_EQ(color.GetAlpha(), 0);
    RSColor bt2020Color(10, 11, 12, 255);
    bt2020Color.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    bt2020Color.MultiplyAlpha(NAN);
    EXPECT_EQ(bt2020Color.GetAlphaF(), 0.0f);
}

/**
 * @tc.name: ConvertToP3ColorSpaceTest
 * @tc.desc: Verify function ConvertToP3ColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesIC90UH
 */
HWTEST_F(RSColorTest, ConvertToP3ColorSpaceTest, TestSize.Level1)
{
    RSColor color;
    color.ConvertToP3ColorSpace();
    EXPECT_EQ(color.GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/**
 * @tc.name: ConvertToSRGBColorSpaceTest
 * @tc.desc: Verify function ConvertToSRGBColorSpace
 * @tc.type: FUNC
 * @tc.require: issuesIC90UH
 */
HWTEST_F(RSColorTest, ConvertToSRGBColorSpaceTest, TestSize.Level1)
{
    RSColor color;
    color.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_EQ(color.GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    color.ConvertToSRGBColorSpace();
    EXPECT_EQ(color.GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: PlaceholderOperatorsTest
 * @tc.desc: Verify branches when RSColor is a placeholder (UNLIKELY branches)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, PlaceholderOperatorsTest, TestSize.Level1)
{
    // construct placeholder colors using the explicit constructor
    RSColor ph;
    ph.placeholder_ = static_cast<uint16_t>(ColorPlaceholder::SURFACE);
    RSColor ph2;
    ph2.placeholder_ = static_cast<uint16_t>(ColorPlaceholder::SURFACE);
    RSColor normal(10, 11, 12, 13);

    // operator+ and operator- should early-return when either side is a placeholder
    EXPECT_TRUE(ph == (ph + normal));
    EXPECT_TRUE(ph == (normal + ph));
    EXPECT_TRUE(ph == (ph - normal));
    EXPECT_TRUE(ph == (normal - ph));
    EXPECT_TRUE(ph == (ph + ph2));

    // operator* should early-return when left operand is a placeholder
    EXPECT_TRUE(ph == (ph * 2.0f));
}

/**
 * @tc.name: HeadroomTest
 * @tc.desc: Verify function GetHeadroom SetHeadroom
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, HeadroomTest, TestSize.Level1)
{
    RSColor color;
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 1.0f);
    color.SetHeadroom(0.5f);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 1.0f);
    color.SetHeadroom(3.5f);
    std::string out;
    color.Dump(out);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), 3.5f);

    RSColor colorTest;
    colorTest.SetHeadroom(1.5f);
    RSColor colorAdd1 = color + colorTest;
    EXPECT_FLOAT_EQ(colorAdd1.GetHeadroom(), 3.5f);
    RSColor colorAdd2 = colorTest + color;
    EXPECT_FLOAT_EQ(colorAdd2.GetHeadroom(), 3.5f);

    RSColor colorReduce1 = color - colorTest;
    EXPECT_FLOAT_EQ(colorReduce1.GetHeadroom(), 3.5f);
    RSColor colorReduce2 = colorTest - color;
    EXPECT_FLOAT_EQ(colorReduce2.GetHeadroom(), 3.5f);
}

/**
 * @tc.name: Float16Test
 * @tc.desc: Verify function Float16ToFloat32 Float32ToFloat16
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, Float16Test, TestSize.Level1)
{
    RSColor color;
    EXPECT_EQ(color.Float16ToFloat32(0x416F), 2.71679688f); // 2.717 ~= 2.71679688
    EXPECT_EQ(color.Float16ToFloat32(0x7C00), std::numeric_limits<float>::infinity());
    EXPECT_EQ(color.Float16ToFloat32(0xFC00), -std::numeric_limits<float>::infinity());
    EXPECT_FALSE(std::isfinite(color.Float16ToFloat32(0x7C01)));
    EXPECT_EQ(color.Float16ToFloat32(0x8001), -5.960464477539063e-8); // subnomal: -5.960464477539063e-8

    EXPECT_EQ(color.Float32ToFloat16(0.453f), 0x373F); // 0.453 for test
    EXPECT_EQ(color.Float32ToFloat16(std::numeric_limits<float>::infinity()), 0x7C00);
    EXPECT_EQ(color.Float32ToFloat16(-std::numeric_limits<float>::infinity()), 0xFC00);
    EXPECT_TRUE(color.Float32ToFloat16(std::numeric_limits<float>::quiet_NaN()) == 0x7C01);
    EXPECT_EQ(color.Float32ToFloat16(5.960464477539063e-8), 0x0001); // subnomal: 5.960464477539063e-8
}

/**
 * @tc.name: BT2020Test
 * @tc.desc: Verify BT2020 Use
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, BT2020Test, TestSize.Level1)
{
    float red = 0.5f;
    float green = 1.0f;
    float blue = 1.0f;
    float alpha = 0.25f;
    float headroom = 2.5f;
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020;
    RSColor color(red, green, blue, alpha, colorSpace, headroom);
    std::string out;
    color.Dump(out);
    EXPECT_FLOAT_EQ(color.GetRedF(), red);
    EXPECT_FLOAT_EQ(color.GetGreenF(), green);
    EXPECT_FLOAT_EQ(color.GetBlueF(), blue);
    EXPECT_FLOAT_EQ(color.GetAlphaF(), alpha);
    EXPECT_FLOAT_EQ(color.GetHeadroom(), headroom);
    EXPECT_TRUE(color.GetColorSpace() == colorSpace);

    float red1 = 0.0f;
    float green1 = 1.0f;
    float blue1 = 0.5f;
    float alpha1 = 0.25f;
    GraphicColorGamut colorSpace1 = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    RSColor color1(red1, green1, blue1, alpha1, colorSpace1, headroom);
    EXPECT_EQ(color1.GetRed(), 0);
    EXPECT_EQ(color1.GetGreen(), 255);
    EXPECT_EQ(color1.GetBlue(), 128);
    EXPECT_EQ(color1.GetAlpha(), 64);
    EXPECT_FLOAT_EQ(color1.GetHeadroom(), headroom);
    EXPECT_TRUE(color1.GetColorSpace() == colorSpace1);

    RSColor color2(0.0f, 0.0f, 0.0f, 1.0f, colorSpace, 2.0f);
    EXPECT_EQ(color2.AsRgbaInt(), 255);
    RSColor color3(0.0f, 0.0f, 1.0f, 0.0f, colorSpace, 2.0f);
    EXPECT_EQ(color3.AsArgbInt(), 255);
    EXPECT_EQ(color2.AsBgraInt(), 255);
    color2.MultiplyAlpha(0.5f);
    EXPECT_FLOAT_EQ(color2.GetAlphaF(), 0.5f);
}

/**
 * @tc.name: BT2020Test1
 * @tc.desc: Verify BT2020 Use
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, BT2020Test1, TestSize.Level1)
{
    RSColor color1(0.2f, 0.0f, 0.5f, 1.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor color2(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, 2.0f);

    EXPECT_TRUE(RSColor(0.1f, 0.0f, 0.25f, 0.5f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 2.0f) == (color1 + color2));
    EXPECT_TRUE(RSColor(0.1f, 0.0f, 0.25f, 0.5f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 2.0f) == (color1 - color2));
    EXPECT_FALSE(RSColor(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 2.0f) == (color2 - color1));

    float scale = 0;
    RSColor colorTest;
    EXPECT_FALSE(color1 == colorTest);
    EXPECT_TRUE(color1 == (color1 + colorTest));
    EXPECT_TRUE(color1 == (color1 - colorTest));
    EXPECT_FALSE(color1 == (color1 * scale));
    EXPECT_TRUE(color1 == (color1 *= scale));

    RSColor color3(0.5f, 0.1f, 0.0f, 1.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, 2.0f);
    RSColor color4(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, 1.0f);
    EXPECT_TRUE(color3 == (color3 + color4));
    EXPECT_TRUE(color3 == (color3 - color4));
}

/**
 * @tc.name: BT2020Test2
 * @tc.desc: Verify BT2020 Use
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, BT2020Test2, TestSize.Level1)
{
    RSColor color1(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, 2.0f);
    color1.ConvertToBT2020ColorSpace();
    EXPECT_TRUE(RSColor(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 2.0f) == color1);
    color1.ConvertToSRGBColorSpace();
    EXPECT_TRUE(RSColor(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, 2.0f) == color1);

    RSColor color2(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, 2.0f);
    color2.ConvertToBT2020ColorSpace();
    EXPECT_TRUE(RSColor(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 2.0f) == color2);
    color2.ConvertToP3ColorSpace();
    EXPECT_TRUE(RSColor(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, 2.0f) == color2);

    RSColor color3(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 2.0f);
    color3.ConvertToSRGBColorSpace();
    EXPECT_TRUE(RSColor(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, 2.0f) == color3);
    color3.ConvertToP3ColorSpace();
    EXPECT_TRUE(RSColor(0.0f, 0.0f, 0.0f, 0.0f,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, 2.0f) == color3);
}

/**
 * @tc.name: BT2020Test3
 * @tc.desc: Verify BT2020 Use
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorTest, BT2020Test3, TestSize.Level1)
{
    int16_t red = 255;
    int16_t green = 0;
    int16_t blue = 0;
    int16_t alpha = 255;
    RSColor color1(red, green, blue, alpha, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_EQ(color1.GetRed(), red);
    EXPECT_EQ(color1.GetGreen(), green);
    EXPECT_EQ(color1.GetBlue(), blue);
    EXPECT_EQ(color1.GetAlpha(), alpha);
    int16_t red1 = 0;
    int16_t green1 = 255;
    int16_t blue1 = 255;
    int16_t alpha1 = 0;
    color1.SetRed(red1);
    color1.SetGreen(green1);
    color1.SetBlue(blue1);
    color1.SetAlpha(alpha1);
    EXPECT_EQ(color1.GetRed(), red1);
    EXPECT_EQ(color1.GetGreen(), green1);
    EXPECT_EQ(color1.GetBlue(), blue1);
    EXPECT_EQ(color1.GetAlpha(), alpha1);

    RSColor color2(red, green, blue, alpha, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    EXPECT_EQ(color2.GetRed(), red);
    EXPECT_EQ(color2.GetGreen(), green);
    EXPECT_EQ(color2.GetBlue(), blue);
    EXPECT_EQ(color2.GetAlpha(), alpha);
    color2.SetRed(red1);
    color2.SetGreen(green1);
    color2.SetBlue(blue1);
    color2.SetAlpha(alpha1);
    EXPECT_EQ(color2.GetRed(), red1);
    EXPECT_EQ(color2.GetGreen(), green1);
    EXPECT_EQ(color2.GetBlue(), blue1);
    EXPECT_EQ(color2.GetAlpha(), alpha1);
}

/**
 * @tc.name: IsAbsNearEqual001
 * @tc.desc: Verify IsAbsNearEqual with same colorspace (int path)
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqual001, TestSize.Level1)
{
    RSColor color(100, 100, 100, 100);
    RSColor target(105, 105, 105, 105);
    RSColor threshold(10, 10, 10, 10);
    // diff is 5, threshold is 10 -> within threshold -> true
    EXPECT_TRUE(color.IsAbsNearEqual(target, threshold));

    RSColor smallThreshold(3, 3, 3, 3);
    // diff is 5, threshold is 3 -> outside threshold -> false
    EXPECT_FALSE(color.IsAbsNearEqual(target, smallThreshold));

    // exact match -> true
    RSColor zeroThreshold(0, 0, 0, 0);
    EXPECT_TRUE(color.IsAbsNearEqual(color, zeroThreshold));

    // negative threshold values: abs is used, so -10 is same as 10
    RSColor negThreshold(-10, -10, -10, -10);
    EXPECT_TRUE(color.IsAbsNearEqual(target, negThreshold));
}

/**
 * @tc.name: IsAbsNearEqual002
 * @tc.desc: Verify IsAbsNearEqual with different colorspace (BT2020 conversion path)
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqual002, TestSize.Level1)
{
    // Same values but different color spaces -> takes the BT2020 conversion branch
    RSColor color(100, 100, 100, 100, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    RSColor target(100, 100, 100, 100, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    RSColor threshold(10, 10, 10, 10);
    // After conversion both should be close -> true
    EXPECT_TRUE(color.IsAbsNearEqual(target, threshold));

    // Different values in different color spaces
    RSColor diffTarget(200, 200, 200, 200, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_FALSE(color.IsAbsNearEqual(diffTarget, threshold));
}

/**
 * @tc.name: IsAbsNearEqual003
 * @tc.desc: Verify IsAbsNearEqual returns false when placeholder differs (same colorspace int path)
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqual003, TestSize.Level1)
{
    RSColor color(100, 100, 100, 100);
    RSColor target(100, 100, 100, 100);
    RSColor threshold(10, 10, 10, 10);
    // same placeholder -> true
    EXPECT_TRUE(color.IsAbsNearEqual(target, threshold));

    // different placeholder -> false (placeholder_ != target.placeholder_)
    target.SetPlaceholder(ColorPlaceholder::SURFACE);
    EXPECT_FALSE(color.IsAbsNearEqual(target, threshold));
}

/**
 * @tc.name: IsAbsNearEqual004
 * @tc.desc: Verify IsAbsNearEqual returns false when placeholder differs (BT2020 conversion path)
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqual004, TestSize.Level1)
{
    RSColor color(100, 100, 100, 100, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    RSColor target(100, 100, 100, 100, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    RSColor threshold(10, 10, 10, 10);
    // different color space triggers BT2020 conversion; same placeholder -> true
    EXPECT_TRUE(color.IsAbsNearEqual(target, threshold));

    // different placeholder in BT2020 path -> false
    target.SetPlaceholder(ColorPlaceholder::SURFACE);
    EXPECT_FALSE(color.IsAbsNearEqual(target, threshold));
}
/**
 * @tc.name: IsAbsNearEqual005
 * @tc.desc: Verify IsAbsNearEqual BT2020 vs SRGB cross-color-space conversion path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqual005, TestSize.Level1)
{
    RSColor bt2020Color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor srgbColor(128, 128, 128, 128, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    // different colorspace triggers BT2020 conversion path
    // BT2020 color stays (already BT2020), SRGB color gets converted
    // large threshold -> close after conversion -> true
    RSColor largeThreshold(0.1f, 0.1f, 0.1f, 0.1f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_TRUE(bt2020Color.IsAbsNearEqual(srgbColor, largeThreshold));

    // zero threshold -> not exactly equal after conversion -> false
    RSColor zeroThreshold(0.0f, 0.0f, 0.0f, 0.0f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_FALSE(bt2020Color.IsAbsNearEqual(srgbColor, zeroThreshold));

    // very different values -> false even with large threshold
    RSColor farSRGB(255, 255, 255, 255, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_FALSE(bt2020Color.IsAbsNearEqual(farSRGB, largeThreshold));
}

/**
 * @tc.name: IsAbsNearEqual006
 * @tc.desc: Verify IsAbsNearEqual BT2020 vs P3 cross-color-space conversion path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqual006, TestSize.Level1)
{
    RSColor bt2020Color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor p3Color(128, 128, 128, 128, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    // different colorspace triggers BT2020 conversion path
    RSColor largeThreshold(0.1f, 0.1f, 0.1f, 0.1f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_TRUE(bt2020Color.IsAbsNearEqual(p3Color, largeThreshold));

    // zero threshold -> not exactly equal -> false
    RSColor zeroThreshold(0.0f, 0.0f, 0.0f, 0.0f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_FALSE(bt2020Color.IsAbsNearEqual(p3Color, zeroThreshold));
}

/**
 * @tc.name: IsAbsNearEqual007
 * @tc.desc: Verify IsAbsNearEqual headroom mismatch in BT2020 cross-color-space path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqual007, TestSize.Level1)
{
    // BT2020 color with headroom 2.0 vs SRGB color with default headroom 1.0
    RSColor bt2020Color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 2.0f);
    RSColor srgbColor(128, 128, 128, 128, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    // large threshold so RGBA comparison passes, but headroom mismatch causes false
    RSColor largeThreshold(1.0f, 1.0f, 1.0f, 1.0f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_FALSE(bt2020Color.IsAbsNearEqual(srgbColor, largeThreshold));

    // same headroom -> true with large threshold
    RSColor bt2020SameHeadroom(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_TRUE(bt2020SameHeadroom.IsAbsNearEqual(srgbColor, largeThreshold));
}


/**
 * @tc.name: IsAbsNearEqualBT2020Test
 * @tc.desc: Verify IsAbsNearEqual with BT2020 colors
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, IsAbsNearEqualBT2020Test, TestSize.Level1)
{
    // Same colorspace uses int16 bit-pattern comparison, zero threshold means exact match
    RSColor color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor target(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor threshold(0.0f, 0.0f, 0.0f, 0.0f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_TRUE(color.IsAbsNearEqual(target, threshold));

    RSColor diffTarget(0.9f, 0.9f, 0.9f, 0.9f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    EXPECT_FALSE(color.IsAbsNearEqual(diffTarget, threshold));
}

/**
 * @tc.name: TakeAbsMaxFrom001
 * @tc.desc: Verify TakeAbsMaxFrom takes the larger absolute value per channel
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, TakeAbsMaxFrom001, TestSize.Level1)
{
    RSColor color(10, 20, 30, 40);
    RSColor target(50, 10, 60, 5);
    color.TakeAbsMaxFrom(target);
    // red: |10| < |50| -> 50
    EXPECT_EQ(color.GetRed(), 50);
    // green: |20| > |10| -> stays 20
    EXPECT_EQ(color.GetGreen(), 20);
    // blue: |30| < |60| -> 60
    EXPECT_EQ(color.GetBlue(), 60);
    // alpha: |40| > |5| -> stays 40
    EXPECT_EQ(color.GetAlpha(), 40);
}

/**
 * @tc.name: TakeAbsMaxFrom002
 * @tc.desc: Verify TakeAbsMaxFrom with all smaller target values
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, TakeAbsMaxFrom002, TestSize.Level1)
{
    RSColor color(100, 100, 100, 100);
    RSColor target(10, 20, 30, 40);
    color.TakeAbsMaxFrom(target);
    // all target values are smaller -> no change
    EXPECT_EQ(color.GetRed(), 100);
    EXPECT_EQ(color.GetGreen(), 100);
    EXPECT_EQ(color.GetBlue(), 100);
    EXPECT_EQ(color.GetAlpha(), 100);
}

/**
 * @tc.name: TakeAbsMaxFrom003
 * @tc.desc: Verify TakeAbsMaxFrom with negative values
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, TakeAbsMaxFrom003, TestSize.Level1)
{
    RSColor color(10, 10, 10, 10);
    RSColor target(-50, -5, -60, -3);
    color.TakeAbsMaxFrom(target);
    // red: |10| < |−50| = 50 -> -50
    EXPECT_EQ(color.GetRed(), -50);
    // green: |10| > |−5| = 5 -> stays 10
    EXPECT_EQ(color.GetGreen(), 10);
    // blue: |10| < |−60| = 60 -> -60
    EXPECT_EQ(color.GetBlue(), -60);
    // alpha: |10| > |−3| = 3 -> stays 10
    EXPECT_EQ(color.GetAlpha(), 10);
}

/**
 * @tc.name: TakeAbsMaxFrom004
 * @tc.desc: Verify TakeAbsMaxFrom with equal magnitude (strict <, no change)
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, TakeAbsMaxFrom004, TestSize.Level1)
{
    RSColor color(50, -50, 50, -50);
    RSColor target(-50, 50, -50, 50);
    color.TakeAbsMaxFrom(target);
    // all |50| == |−50|, strict < means no change
    EXPECT_EQ(color.GetRed(), 50);
    EXPECT_EQ(color.GetGreen(), -50);
    EXPECT_EQ(color.GetBlue(), 50);
    EXPECT_EQ(color.GetAlpha(), -50);
}

/**
 * @tc.name: OperatorMultiplyColor001
 * @tc.desc: Verify operator*(const RSColor&) for normal (SRGB) path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorMultiplyColor001, TestSize.Level1)
{
    RSColor color(100, 200, 50, 255);
    RSColor other(2, 2, 2, 2);
    RSColor result = color * other;
    EXPECT_EQ(result.GetRed(), round(100 * 2));
    EXPECT_EQ(result.GetGreen(), round(200 * 2));
    EXPECT_EQ(result.GetBlue(), round(50 * 2));
    EXPECT_EQ(result.GetAlpha(), round(255 * 2));
}

/**
 * @tc.name: OperatorMultiplyColor002
 * @tc.desc: Verify operator*(const RSColor&) for placeholder path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorMultiplyColor002, TestSize.Level1)
{
    RSColor ph;
    ph.placeholder_ = static_cast<uint16_t>(ColorPlaceholder::SURFACE);
    RSColor normal(100, 200, 50, 255);
    // placeholder * normal -> returns placeholder itself
    RSColor result = ph * normal;
    EXPECT_TRUE(ph == result);
}

/**
 * @tc.name: OperatorMultiplyColor003
 * @tc.desc: Verify operator*(const RSColor&) for BT2020 path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorMultiplyColor003, TestSize.Level1)
{
    RSColor color(0.5f, 0.4f, 0.3f, 0.2f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor other(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor result = color * other;
    // BT2020 colors are stored as Float16, which loses precision for non-power-of-2 values.
    EXPECT_NEAR(result.GetRedF(), 0.25f, 0.005f);
    EXPECT_NEAR(result.GetGreenF(), 0.2f, 0.005f);
    EXPECT_NEAR(result.GetBlueF(), 0.15f, 0.005f);
    EXPECT_NEAR(result.GetAlphaF(), 0.1f, 0.005f);
    EXPECT_TRUE(result.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
}

/**
 * @tc.name: OperatorMultiplyAssignColor001
 * @tc.desc: Verify operator*=(const RSColor&) for normal (SRGB) path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorMultiplyAssignColor001, TestSize.Level1)
{
    RSColor color(100, 200, 50, 255);
    RSColor other(2, 2, 2, 2);
    color *= other;
    EXPECT_EQ(color.GetRed(), round(100 * 2));
    EXPECT_EQ(color.GetGreen(), round(200 * 2));
    EXPECT_EQ(color.GetBlue(), round(50 * 2));
    EXPECT_EQ(color.GetAlpha(), round(255 * 2));
}

/**
 * @tc.name: OperatorMultiplyAssignColor002
 * @tc.desc: Verify operator*=(const RSColor&) for placeholder path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorMultiplyAssignColor002, TestSize.Level1)
{
    RSColor ph;
    ph.placeholder_ = static_cast<uint16_t>(ColorPlaceholder::SURFACE);
    RSColor original = ph;
    RSColor normal(100, 200, 50, 255);
    ph *= normal;
    // placeholder *= normal -> returns placeholder itself (no change)
    EXPECT_TRUE(ph == original);
}

/**
 * @tc.name: OperatorMultiplyAssignColor003
 * @tc.desc: Verify operator*=(const RSColor&) for BT2020 path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorMultiplyAssignColor003, TestSize.Level1)
{
    RSColor color(0.5f, 0.4f, 0.3f, 0.2f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor other(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    color *= other;
    // BT2020 colors are stored as Float16, which loses precision for non-power-of-2 values.
    EXPECT_NEAR(color.GetRedF(), 0.25f, 0.005f);
    EXPECT_NEAR(color.GetGreenF(), 0.2f, 0.005f);
    EXPECT_NEAR(color.GetBlueF(), 0.15f, 0.005f);
    EXPECT_NEAR(color.GetAlphaF(), 0.1f, 0.005f);
}

/**
 * @tc.name: OperatorSubtractColor001
 * @tc.desc: Verify operator- with BT2020 cross-color-space conversion path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorSubtractColor001, TestSize.Level1)
{
    // BT2020 - SRGB: either operand is BT2020 -> BT2020 conversion path
    RSColor bt2020Color(0.5f, 0.5f, 0.5f, 0.5f, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, 1.0f);
    RSColor srgbColor(128, 128, 128, 128, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    RSColor result = bt2020Color - srgbColor;
    // result colorspace should be BT2020 (operator- sets colorSpace=BT2020 when either is BT2020)
    EXPECT_TRUE(result.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    // result values should be finite
    EXPECT_TRUE(std::isfinite(result.GetRedF()));
    EXPECT_TRUE(std::isfinite(result.GetGreenF()));
    EXPECT_TRUE(std::isfinite(result.GetBlueF()));
    EXPECT_TRUE(std::isfinite(result.GetAlphaF()));
}

/**
 * @tc.name: OperatorSubtractColor002
 * @tc.desc: Verify operator- with SRGB vs P3 cross-color-space conversion path
 * @tc.type: FUNC
 */
HWTEST_F(RSColorTest, OperatorSubtractColor002, TestSize.Level1)
{
    // SRGB - P3: neither is BT2020, either is P3 -> P3 conversion path
    RSColor srgbColor(100, 100, 100, 100, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    RSColor p3Color(100, 100, 100, 100, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    RSColor result = srgbColor - p3Color;
    // result colorspace should be DISPLAY_P3 (operator- sets colorSpace=P3 when either is P3)
    EXPECT_TRUE(result.GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}
} // namespace OHOS::Rosen