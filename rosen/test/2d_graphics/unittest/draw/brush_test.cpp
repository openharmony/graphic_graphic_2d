/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "draw/brush.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class BrushTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BrushTest::SetUpTestCase() {}
void BrushTest::TearDownTestCase() {}
void BrushTest::SetUp() {}
void BrushTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CreateAndDestroy001, TestSize.Level1)
{
    std::unique_ptr<Brush> brush = std::make_unique<Brush>();
    ASSERT_TRUE(nullptr != brush);
}

/**
 * @tc.name: CopyConstructor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CopyConstructor001, TestSize.Level1)
{
    Brush oldBrush;
    Brush newBrush = oldBrush;
    EXPECT_TRUE(oldBrush == newBrush);
}

/**
 * @tc.name: CopyConstructor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CopyConstructor002, TestSize.Level1)
{
    Brush brush;
    auto brush2 = std::make_unique<Brush>(brush);
    EXPECT_TRUE(brush2 != nullptr);
}

/**
 * @tc.name: CreateBrushWithColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CreateBrushWithColor001, TestSize.Level1)
{
    Color color;
    std::unique_ptr<Brush> brush = std::make_unique<Brush>(color);
    EXPECT_TRUE(nullptr != brush);
}

/**
 * @tc.name: CreateBrushWithShaderEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CreateBrushWithShaderEffect001, TestSize.Level1)
{
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    auto brush = std::make_unique<Brush>(shaderEffect);
    EXPECT_TRUE(nullptr != brush);
}

/**
 * @tc.name: CreateBrushWithShaderEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CreateBrushWithShaderEffect002, TestSize.Level1)
{
    auto shaderEffect2 = ShaderEffect::CreateColorShader(55);
    auto brush = std::make_unique<Brush>(shaderEffect2);
    EXPECT_TRUE(nullptr != brush);
}

/**
 * @tc.name: CreateBrushWithRGB001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CreateBrushWithRGB001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>(222);
    EXPECT_TRUE(nullptr != brush);
}

/**
 * @tc.name: CreateBrushWithRGB002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, CreateBrushWithRGB002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>(123);
    EXPECT_TRUE(nullptr != brush);
}

/**
 * @tc.name: GetterAndSetterOfColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfColor001, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetColor(color);
    auto color1 = brush->GetColor();
    EXPECT_TRUE(color1 == color);
}

/**
 * @tc.name: GetterAndSetterOfColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfColor002, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetColor(color);
    auto color2 = brush->GetColor();
    EXPECT_TRUE(color2 == color);
}

/**
 * @tc.name: SetARGB001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetARGB001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetARGB(255, 255, 255, 1);
}

/**
 * @tc.name: SetARGB002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetARGB002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetARGB(0, 0, 0, 1);
}

/**
 * @tc.name: GetColor4f001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetColor4f001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    Color4f color4f = { -1.5f, -2.5f, -3.5f, -4.5f };
    brush->SetColor(color4f, colorSpace);
    Color4f expectColor4f = { 0.0f, 0.0f, 0.0f, 0.0f };
    EXPECT_TRUE(std::abs(expectColor4f.redF_ - brush->GetColor4f().redF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.greenF_ - brush->GetColor4f().greenF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.blueF_ - brush->GetColor4f().blueF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.alphaF_ - brush->GetColor4f().alphaF_) < 0.01);
}

/**
 * @tc.name: GetColor4f002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetColor4f002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    Color4f color4f = { 10.5f, 20.5f, 30.5f, 40.5f };
    brush->SetColor(color4f, colorSpace);
    Color4f expectColor4f = { 1.0f, 1.0f, 1.0f, 1.0f };
    EXPECT_TRUE(std::abs(expectColor4f.redF_ - brush->GetColor4f().redF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.greenF_ - brush->GetColor4f().greenF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.blueF_ - brush->GetColor4f().blueF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.alphaF_ - brush->GetColor4f().alphaF_) < 0.01);
}

/**
 * @tc.name: GetColor4f003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetColor4f003, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    Color4f color4f = { 0.2f, 0.3f, 0.4f, 0.5f };
    brush->SetColor(color4f, colorSpace);
    Color4f expectColor4f = { 0.2f, 0.3f, 0.4f, 0.5f };
    EXPECT_TRUE(std::abs(expectColor4f.redF_ - brush->GetColor4f().redF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.greenF_ - brush->GetColor4f().greenF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.blueF_ - brush->GetColor4f().blueF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.alphaF_ - brush->GetColor4f().alphaF_) < 0.01);
}

/**
 * @tc.name: GetColorSpace001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetColorSpace001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    Color4f color4f;
    brush->SetColor(color4f, colorSpace);
    ASSERT_EQ(colorSpace, brush->GetColorSpace());
}

/**
 * @tc.name: GetColorSpace002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetColorSpace002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::REF_IMAGE);
    Color4f color4f;
    brush->SetColor(color4f, colorSpace);
    ASSERT_EQ(colorSpace, brush->GetColorSpace());
}

/**
 * @tc.name: SetColorWithColor4fAndColorSpace001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetColorWithColor4fAndColorSpace001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    Color4f color;
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    ASSERT_TRUE(colorSpace != nullptr);
    brush->SetColor(color, colorSpace);
}

/**
 * @tc.name: SetColorWithColor4fAndColorSpace002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetColorWithColor4fAndColorSpace002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    Color4f color;
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB_LINEAR);
    ASSERT_TRUE(colorSpace != nullptr);
    brush->SetColor(color, colorSpace);
}

/**
 * @tc.name: GetterSetterOfAlpha001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterSetterOfAlpha001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetAlpha(1);
    auto alpha = brush->GetAlpha();
    EXPECT_TRUE(alpha == 1);
}

/**
 * @tc.name: GetterSetterOfAlpha002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterSetterOfAlpha002, TestSize.Level1)
{
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    ASSERT_TRUE(shaderEffect != nullptr);
    auto brush = std::make_unique<Brush>(shaderEffect);
    ASSERT_TRUE(brush != nullptr);
    brush->SetAlpha(220);
    auto alpha1 = brush->GetAlpha();
    EXPECT_TRUE(alpha1 == 220);
}

/**
 * @tc.name: SetAlphaF001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetAlphaF001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetAlphaF(5.5f);
}

/**
 * @tc.name: SetAlphaF002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetAlphaF002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetAlphaF(15.5f);
}

/**
 * @tc.name: GetterAndSetterOfBlendMode001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfBlendMode001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetBlendMode(BlendMode::PLUS);
    auto blendMode = brush->GetBlendMode();
    EXPECT_TRUE(blendMode == BlendMode::PLUS);
}

/**
 * @tc.name: GetterAndSetterOfBlendMode002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfBlendMode002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetBlendMode(BlendMode::SRC_IN);
    auto blendMode = brush->GetBlendMode();
    EXPECT_TRUE(blendMode == BlendMode::SRC_IN);
}

/**
 * @tc.name: GetterAndSetterOfFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfFilter001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    Filter filter;
    brush->SetFilter(filter);
    auto filter2 = brush->GetFilter();
    EXPECT_TRUE(filter2 == filter);
}

/**
 * @tc.name: GetterAndSetterOfFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfFilter002, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>(color);
    ASSERT_TRUE(brush != nullptr);
    Filter filter3;
    brush->SetFilter(filter3);
    auto filter2 = brush->GetFilter();
    EXPECT_TRUE(filter2 == filter3);
}

/**
 * @tc.name: GetterAndSetterOfShaderEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfShaderEffect001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    ASSERT_TRUE(shaderEffect != nullptr);
    brush->SetShaderEffect(shaderEffect);
    auto shaderEffect2 = brush->GetShaderEffect();
    EXPECT_TRUE(shaderEffect2 != nullptr);
}

/**
 * @tc.name: GetterAndSetterOfShaderEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, GetterAndSetterOfShaderEffect002, TestSize.Level1)
{
    Color color;
    auto brush = std::make_unique<Brush>(color);
    ASSERT_TRUE(brush != nullptr);
    auto shaderEffect = ShaderEffect::CreateColorShader(22);
    ASSERT_TRUE(shaderEffect != nullptr);
    brush->SetShaderEffect(shaderEffect);
    auto shaderEffect3 = brush->GetShaderEffect();
    EXPECT_TRUE(shaderEffect3 != nullptr);
}

/**
 * @tc.name: SetAntiAliasAndIsAntiAlias001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetAntiAliasAndIsAntiAlias001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetAntiAlias(true);
    ASSERT_EQ(true, brush->IsAntiAlias());
}

/**
 * @tc.name: SetAntiAliasAndIsAntiAlias002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, SetAntiAliasAndIsAntiAlias002, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    brush->SetAntiAlias(false);
    ASSERT_EQ(false, brush->IsAntiAlias());
}

/**
 * @tc.name: IsEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, IsEquals001, TestSize.Level1)
{
    auto brush = std::make_unique<Brush>();
    ASSERT_TRUE(brush != nullptr);
    Color color;
    auto newBrush = std::make_unique<Brush>(color);
    ASSERT_TRUE(newBrush != nullptr);
    EXPECT_FALSE(brush == newBrush);
    brush->Reset();
}

/**
 * @tc.name: IsEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, IsEquals002, TestSize.Level1)
{
    int rgba = 210;
    auto brush = std::make_unique<Brush>(rgba);
    ASSERT_TRUE(brush != nullptr);
    ColorQuad rgba2 = 123;
    Color color1(rgba2);
    auto newBrush = std::make_unique<Brush>(color1);
    ASSERT_TRUE(newBrush != nullptr);
    EXPECT_FALSE(brush == newBrush);
    brush->Reset();
}

/**
 * @tc.name: IsEquals003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 * @tc.author:
 */
HWTEST_F(BrushTest, IsEquals003, TestSize.Level1)
{
    Brush brush1;
    // 1.f 2.f  3.f and 0x12345678 is setted to compare.
    float radius = 1.f;
    Point point{2.f, 3.f};
    Color color = Color(0x12345678);
    std::shared_ptr<BlurDrawLooper> blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(radius,
        point.GetX(), point.GetY(), color);
    brush1.SetLooper(blurDrawLooper);
    Brush brush2 = brush1;
    EXPECT_TRUE(brush1 == brush2);
    brush2.SetLooper(nullptr);
    EXPECT_TRUE(brush1 != brush2);
}

/**
 * @tc.name: IsNotEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, IsNotEquals001, TestSize.Level1)
{
    Color color(2, 4, 10, 1);
    auto brush = std::make_shared<Brush>(color);
    ASSERT_TRUE(brush != nullptr);
    auto newBrush = brush;
    EXPECT_FALSE(brush != newBrush);
}

/**
 * @tc.name: IsNotEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BrushTest, IsNotEquals002, TestSize.Level1)
{
    auto brush = std::make_shared<Brush>();
    ASSERT_TRUE(brush != nullptr);
    auto newBrush = brush;
    EXPECT_FALSE(brush != newBrush);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
