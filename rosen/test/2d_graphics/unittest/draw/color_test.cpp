/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "draw/color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ColorTest::SetUpTestCase() {}
void ColorTest::TearDownTestCase() {}
void ColorTest::SetUp() {}
void ColorTest::TearDown() {}

/*
 * @tc.name: ColorQuadSetARGAndGetARGB001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, ColorQuadSetARGAndGetARGB001, TestSize.Level1)
{
    Color::ColorQuadSetARGB(255, 200, 145, 1);
    EXPECT_EQ(0, Color::ColorQuadGetR(0));
    EXPECT_EQ(255, Color::ColorQuadGetG(Color::COLOR_YELLOW));
    EXPECT_EQ(0, Color::ColorQuadGetB(Color::COLOR_GREEN));
    EXPECT_EQ(255, Color::ColorQuadGetA(Color::COLOR_BLUE));
}

/*
 * @tc.name: ColorQuadSetARGAndGetARGB002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, ColorQuadSetARGAndGetARGB002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    Color::ColorQuadSetARGB(255, 200, 145, 1);
    uint32_t r = ((200 >> 16) & 0xff);
    ASSERT_EQ(r, Color::ColorQuadGetR(200));
    uint32_t g = ((145 >> 8) & 0xff);
    ASSERT_EQ(g, Color::ColorQuadGetG(145));
    uint32_t b = ((1 >> 0) & 0xff);
    ASSERT_EQ(b, Color::ColorQuadGetB(1));
    uint32_t a = ((255 >> 24) & 0xff);
    ASSERT_EQ(a, Color::ColorQuadGetA(255));
}

/*
 * @tc.name: CreateInstance001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, CreateInstance001, TestSize.Level1)
{
    auto color = std::make_unique<Color>(44, 23, 56, 0);
    EXPECT_TRUE(nullptr != color);
    EXPECT_EQ(44, color->GetRed());
    EXPECT_EQ(23, color->GetGreen());
    EXPECT_EQ(56, color->GetBlue());
    EXPECT_EQ(0, color->GetAlpha());
}

/*
 * @tc.name: CreateInstance002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, CreateInstance002, TestSize.Level1)
{
    auto color2 = std::make_unique<Color>(123, 33, 50, 1);
    EXPECT_TRUE(nullptr != color2);
    EXPECT_EQ(123, color2->GetRed());
    EXPECT_EQ(33, color2->GetGreen());
    EXPECT_EQ(50, color2->GetBlue());
    EXPECT_EQ(1, color2->GetAlpha());
}

/*
 * @tc.name: CopyConstructor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, CopyConstructor001, TestSize.Level1)
{
    Color color;
    Color color2(color);
    EXPECT_TRUE(color == color2);
}

/*
 * @tc.name: CopyConstructor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, CopyConstructor002, TestSize.Level1)
{
    Color color = Color(123, 33, 50, 1);
    Color color2(color);
    EXPECT_TRUE(color == color2);
}

/*
 * @tc.name: CopyConstructor003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, CopyConstructor003, TestSize.Level1)
{
    Color color;
    Color color2(color);
    EXPECT_FALSE(color != color2);
}

/*
 * @tc.name: CopyConstructor004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, CopyConstructor004, TestSize.Level1)
{
    Color color = Color(50, 1, 123, 33);
    Color color2(color);
    EXPECT_FALSE(color != color2);
}

/*
 * @tc.name: ConstructorRGBA001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, ConstructorRGBA001, TestSize.Level1)
{
    auto color = std::make_unique<Color>(255, 255, 100, 1);
    EXPECT_TRUE(nullptr != color);
    EXPECT_EQ(255, color->GetRed());
    EXPECT_EQ(255, color->GetGreen());
    EXPECT_EQ(100, color->GetBlue());
    EXPECT_EQ(1, color->GetAlpha());
}

/*
 * @tc.name: ConstructorRGBA002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, ConstructorRGBA002, TestSize.Level1)
{
    auto color = std::make_unique<Color>(100, 34, 100, 50);
    EXPECT_TRUE(nullptr != color);
    EXPECT_EQ(100, color->GetRed());
    EXPECT_EQ(34, color->GetGreen());
    EXPECT_EQ(100, color->GetBlue());
    EXPECT_EQ(50, color->GetAlpha());
}

/*
 * @tc.name: ConstructorColorQuad001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, ConstructorColorQuad001, TestSize.Level1)
{
    auto color = std::make_unique<Color>(210);
    EXPECT_TRUE(nullptr != color);
}

/*
 * @tc.name: ConstructorColorQuad002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, ConstructorColorQuad002, TestSize.Level1)
{
    auto color = std::make_unique<Color>(100);
    EXPECT_TRUE(nullptr != color);
}

/*
 * @tc.name: GetterAndSetterOfRed001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfRed001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetRed(155);
    auto red = color->GetRed();
    EXPECT_TRUE(red == 155);
}

/*
 * @tc.name: GetterAndSetterOfRed002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfRed002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetRed(250);
    auto red = color->GetRed();
    EXPECT_TRUE(red == 250);
}

/*
 * @tc.name: GetterAndSetterOfGreen001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfGreen001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetGreen(250);
    auto green = color->GetGreen();
    EXPECT_TRUE(green == 250);
}

/*
 * @tc.name: GetterAndSetterOfGreen002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfGreen002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetGreen(100);
    auto green = color->GetGreen();
    EXPECT_TRUE(green == 100);
}

/*
 * @tc.name: GetterAndSetterOfBlue001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfBlue001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetBlue(100);
    auto blue = color->GetBlue();
    EXPECT_TRUE(blue == 100);
}

/*
 * @tc.name: GetterAndSetterOfBlue002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfBlue002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetBlue(10);
    auto blue = color->GetBlue();
    EXPECT_TRUE(blue == 10);
}

/*
 * @tc.name: GetterAndSetterOfAlpha001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfAlpha001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetAlpha(0);
    auto alpha = color->GetAlpha();
    EXPECT_TRUE(alpha == 0);
}

/*
 * @tc.name: GetterAndSetterOfAlpha002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfAlpha002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetAlpha(1);
    auto alpha = color->GetAlpha();
    EXPECT_TRUE(alpha == 1);
}

/*
 * @tc.name: GetterAndSetterOfRedF001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfRedF001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetRedF(10.0f);
    Color4f expectColor4f = {1.0f, 0.0f, 0.0f, 0.0f};
    ASSERT_EQ(expectColor4f.redF_, color->GetRedF());
}

/*
 * @tc.name: GetterAndSetterOfRedF002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfRedF002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetRedF(-21.0f);
    Color4f expectColor4f = {0.0f, 0.0f, 0.0f, 0.0f};
    ASSERT_EQ(expectColor4f.redF_, color->GetRedF());
}

/*
 * @tc.name: GetterAndSetterOfRedF003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfRedF003, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetRedF(0.5f);
    Color4f expectColor4f = {0.5f, 0.0f, 0.0f, 0.0f};
    ASSERT_TRUE(std::abs(expectColor4f.redF_ - color->GetRedF()) < 0.01);
}

/*
 * @tc.name: GetterAndSetterOfGreenF001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfGreenF001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetGreenF(10.0f);
    Color4f expectColor4f = {0.0f, 1.0f, 0.0f, 0.0f};
    ASSERT_EQ(expectColor4f.greenF_, color->GetGreenF());
}

/*
 * @tc.name: GetterAndSetterOfGreenF002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfGreenF002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetGreenF(-102.0f);
    Color4f expectColor4f = {0.0f, 0.0f, 0.0f, 0.0f};
    ASSERT_EQ(expectColor4f.greenF_, color->GetGreenF());
}

/*
 * @tc.name: GetterAndSetterOfGreenF003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfGreenF003, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetGreenF(0.3f);
    Color4f expectColor4f = {0.0f, 0.3f, 0.0f, 0.0f};
    ASSERT_TRUE(std::abs(expectColor4f.greenF_ - color->GetGreenF()) < 0.01);
}

/*
 * @tc.name: GetterAndSetterOfBlueF001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfBlueF001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetBlueF(11.5f);
    Color4f expectColor4f = {0.0f, 0.0f, 1.0f, 0.0f};
    ASSERT_EQ(expectColor4f.blueF_, color->GetBlueF());
}

/*
 * @tc.name: GetterAndSetterOfBlueF002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfBlueF002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetBlueF(-10.0f);
    Color4f expectColor4f = {0.0f, 0.0f, 0.0f, 0.0f};
    ASSERT_EQ(expectColor4f.blueF_, color->GetBlueF());
}

/*
 * @tc.name: GetterAndSetterOfBlueF003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfBlueF003, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetBlueF(0.5f);
    Color4f expectColor4f = {0.0f, 0.0f, 0.5f, 0.0f};
    ASSERT_TRUE(std::abs(expectColor4f.blueF_ - color->GetBlueF()) < 0.01);
}

/*
 * @tc.name: GetterAndSetterOfAlphaF001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfAlphaF001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetAlphaF(55.0f);
    Color4f expectColor4f = {0.0f, 0.0f, 0.0f, 1.0f};
    ASSERT_EQ(expectColor4f.alphaF_, color->GetAlphaF());
}

/*
 * @tc.name: GetterAndSetterOfAlphaF002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfAlphaF002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetAlphaF(-1.0f);
    Color4f expectColor4f = {0.0f, 0.0f, 0.0f, 0.0f};
    ASSERT_EQ(expectColor4f.alphaF_, color->GetAlphaF());
}

/*
 * @tc.name: GetterAndSetterOfAlphaF003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, GetterAndSetterOfAlphaF003, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetAlphaF(0.6f);
    Color4f expectColor4f = {0.0f, 0.0f, 0.0f, 0.6f};
    ASSERT_TRUE(std::abs(expectColor4f.alphaF_ - color->GetAlphaF()) < 0.01);
}

/*
 * @tc.name: SetRgb001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, SetRgb001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetRgb(255, 255, 255, 255);
}

/*
 * @tc.name: SetRgb002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, SetRgb002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetRgb(0, 0, 0);
}

/*
 * @tc.name: SetRgbF002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, SetRgbF002, TestSize.Level1)
{
    auto color = std::make_unique<Color>(121);
    ASSERT_TRUE(color != nullptr);
    color->SetRgbF(55.3f, 23.4f, 50.5f, 0.5f);
}

/*
 * @tc.name: SetColorQuad001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, SetColorQuad001, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetColorQuad(Color::COLOR_WHITE);
    ASSERT_EQ(color->CastToColorQuad(), Color::COLOR_WHITE);
}

/*
 * @tc.name: SetColorQuad002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, SetColorQuad002, TestSize.Level1)
{
    auto color = std::make_unique<Color>();
    ASSERT_TRUE(color != nullptr);
    color->SetColorQuad(Color::COLOR_DKGRAY);
    ASSERT_EQ(color->CastToColorQuad(), Color::COLOR_DKGRAY);
}

/*
 * @tc.name: OverrideEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, OverrideEquals001, TestSize.Level1)
{
    Color colorA;
    Color colorB = Color(255, 255, 100, 1);
    EXPECT_FALSE(colorA == colorB);
}

/*
 * @tc.name: OverrideEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, OverrideEquals002, TestSize.Level1)
{
    Color colorA;
    Color colorB;
    EXPECT_TRUE(colorA == colorB);
}

/*
 * @tc.name: OverrideNotEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, OverrideNotEquals001, TestSize.Level1)
{
    Color colorA;
    Color colorB;
    EXPECT_FALSE(colorA != colorB);
}

/*
 * @tc.name: OverrideNotEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorTest, OverrideNotEquals002, TestSize.Level1)
{
    Color colorA;
    Color colorB = Color(255, 255, 100, 1);
    EXPECT_TRUE(colorA != colorB);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
