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

#include "draw/pen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PenTest::SetUpTestCase() {}
void PenTest::TearDownTestCase() {}
void PenTest::SetUp() {}
void PenTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, CreateAndDestroy001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
}

/**
 * @tc.name: CopyConstructor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, CopyConstructor001, TestSize.Level1)
{
    Pen oldPen;
    Pen pen{oldPen};
    ASSERT_TRUE(pen == oldPen);
    pen.SetWidth(1);
    ASSERT_TRUE(pen != oldPen);
}

/**
 * @tc.name: CreateByColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, CreateByColor001, TestSize.Level1)
{
    Color color = Color(255, 255, 255, 255);
    auto pen = std::make_unique<Pen>(color);
    ASSERT_TRUE(pen != nullptr);
}

/**
 * @tc.name: CreateByColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, CreateByColor002, TestSize.Level1)
{
    Color color = Color(11, 12, 13, 14);
    auto pen = std::make_unique<Pen>(color);
    ASSERT_TRUE(pen != nullptr);
}

/**
 * @tc.name: CreateByRGBA001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, CreateByRGBA001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>(255);
    ASSERT_TRUE(pen != nullptr);
}

/**
 * @tc.name: CreateByRGBA002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, CreateByRGBA002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>(100);
    ASSERT_TRUE(pen != nullptr);
}

/**
 * @tc.name: GetterAndSetterOfColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfColor001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    Color color;
    pen->SetColor(color);
    auto colorNew = pen->GetColor();
    EXPECT_TRUE(color == colorNew);
}

/**
 * @tc.name: GetterAndSetterOfColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfColor002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    Color color = Color(11, 12, 13, 14);
    pen->SetColor(color);
    auto colorNew = pen->GetColor();
    EXPECT_TRUE(color == colorNew);
}

/**
 * @tc.name: SetColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetColor001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetColor(255); // color value
}

/**
 * @tc.name: SetColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetColor002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetColor(100); // color value
}

/**
 * @tc.name: SetARGB001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetARGB001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetARGB(100, 100, 10, 1); // color value
}

/**
 * @tc.name: SetARGB002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetARGB002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>(11);
    ASSERT_TRUE(pen != nullptr);
    pen->SetARGB(100, 2, 10, 0); // color value
}

/**
 * @tc.name: GetColor4f001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetColor4f001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    Color4f color4f = { 1.5f, 2.5f, 3.5f, 4.5f };
    pen->SetColor(color4f, colorSpace);
    Color4f expectColor4f = { 1.0f, 1.0f, 1.0f, 1.0f };
    EXPECT_TRUE(std::abs(expectColor4f.redF_ - pen->GetColor4f().redF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.greenF_ - pen->GetColor4f().greenF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.blueF_ - pen->GetColor4f().blueF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.alphaF_ - pen->GetColor4f().alphaF_) < 0.01);
}

/**
 * @tc.name: GetColor4f002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetColor4f002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    Color4f color4f = { -10.5f, -20.5f, -30.5f, -40.5f };
    pen->SetColor(color4f, colorSpace);
    Color4f expectColor4f = { 0.0f, 0.0f, 0.0f, 0.0f };
    EXPECT_TRUE(std::abs(expectColor4f.redF_ - pen->GetColor4f().redF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.greenF_ - pen->GetColor4f().greenF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.blueF_ - pen->GetColor4f().blueF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.alphaF_ - pen->GetColor4f().alphaF_) < 0.01);
}

/**
 * @tc.name: GetColor4f003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetColor4f003, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    Color4f color4f = { 0.3f, 0.4f, 0.5f, 0.6f };
    pen->SetColor(color4f, colorSpace);
    Color4f expectColor4f = { 0.3f, 0.4f, 0.5f, 0.6f };
    EXPECT_TRUE(std::abs(expectColor4f.redF_ - pen->GetColor4f().redF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.greenF_ - pen->GetColor4f().greenF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.blueF_ - pen->GetColor4f().blueF_) < 0.01);
    EXPECT_TRUE(std::abs(expectColor4f.alphaF_ - pen->GetColor4f().alphaF_) < 0.01);
}

/**
 * @tc.name: GetColorSpace001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetColorSpace001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB_LINEAR);
    Color4f color4f;
    pen->SetColor(color4f, colorSpace);
    ASSERT_EQ(colorSpace, pen->GetColorSpace());
}

/**
 * @tc.name: GetColorSpace002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetColorSpace002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    Color4f color4f;
    pen->SetColor(color4f, colorSpace);
    ASSERT_EQ(colorSpace, pen->GetColorSpace());
}

/**
 * @tc.name: SetColorWith2001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetColorWith2001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    Color4f color4f;
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
    ASSERT_TRUE(colorSpace != nullptr);
    pen->SetColor(color4f, colorSpace);
}

/**
 * @tc.name: SetColorWith2002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetColorWith2002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    Color4f color4f;
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::REF_IMAGE);
    ASSERT_TRUE(colorSpace != nullptr);
    pen->SetColor(color4f, colorSpace);
}

/**
 * @tc.name: GetterAndSetterOfAlpha001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfAlpha001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    uint32_t alpha = 1;
    pen->SetAlpha(alpha);
    ASSERT_EQ(alpha, pen->GetAlpha());
}

/**
 * @tc.name: GetterAndSetterOfAlpha002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfAlpha002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    uint32_t alpha = 0;
    pen->SetAlpha(alpha);
    ASSERT_EQ(alpha, pen->GetAlpha());
}

/**
 * @tc.name: GetterAndSetterOfAlphaf001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfAlphaf001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    scalar alpha = 0;
    pen->SetAlphaF(alpha);
    ASSERT_TRUE(IsScalarAlmostEqual(alpha, pen->GetAlphaF()));
}

/**
 * @tc.name: GetterAndSetterOfWidth001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfWidth001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    scalar w = 0;
    pen->SetWidth(w);
    ASSERT_TRUE(IsScalarAlmostEqual(w, pen->GetWidth()));
}

/**
 * @tc.name: SetAlphaF001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetAlphaF001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetAlphaF(0.5f);
}

/**
 * @tc.name: SetAlphaF002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetAlphaF002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetAlphaF(15.0f);
}

/**
 * @tc.name: GetterAndSetterOfMiterLimit001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfMiterLimit001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetMiterLimit(100.0f);
    auto limit = pen->GetMiterLimit();
    ASSERT_EQ(limit, 100.0f);
}

/**
 * @tc.name: GetterAndSetterOfMiterLimit002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfMiterLimit002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetMiterLimit(20.5f);
    auto limit = pen->GetMiterLimit();
    ASSERT_EQ(limit, 20.5f);
}

/**
 * @tc.name: GetterAndSetterOfCapStyle001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfCapStyle001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetCapStyle(Pen::CapStyle::FLAT_CAP);
    ASSERT_EQ(Pen::CapStyle::FLAT_CAP, pen->GetCapStyle());
}

/**
 * @tc.name: GetterAndSetterOfCapStyle002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfCapStyle002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetCapStyle(Pen::CapStyle::ROUND_CAP);
    ASSERT_EQ(Pen::CapStyle::ROUND_CAP, pen->GetCapStyle());
}

/**
 * @tc.name: GetterAndSetterOfJoinStyle001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfJoinStyle001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
    ASSERT_EQ(Pen::JoinStyle::ROUND_JOIN, pen->GetJoinStyle());
}

/**
 * @tc.name: GetterAndSetterOfJoinStyle002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfJoinStyle002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetJoinStyle(Pen::JoinStyle::BEVEL_JOIN);
    ASSERT_EQ(Pen::JoinStyle::BEVEL_JOIN, pen->GetJoinStyle());
}

/**
 * @tc.name: GetterAndSetterOfBlendMode001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfBlendMode001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetBlendMode(BlendMode::SRC_IN);
    ASSERT_EQ(BlendMode::SRC_IN, pen->GetBlendMode());
}

/**
 * @tc.name: GetterAndSetterOfBlendMode002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfBlendMode002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetBlendMode(BlendMode::SRC_ATOP);
    ASSERT_EQ(BlendMode::SRC_ATOP, pen->GetBlendMode());
}

/**
 * @tc.name: GetterAndSetterOfBlender001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfBlender001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    std::shared_ptr<Blender> blender = std::make_unique<Blender>();
    pen->SetBlender(blender);
    ASSERT_EQ(blender, pen->GetBlender());
}

/**
 * @tc.name: GetterAndSetterOfAntiAlias001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfAntiAlias001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetAntiAlias(true);
    auto result = pen->IsAntiAlias();
    EXPECT_TRUE(result == true);
}

/**
 * @tc.name: GetterAndSetterOfAntiAlias002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfAntiAlias002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    pen->SetAntiAlias(false);
    auto result = pen->IsAntiAlias();
    EXPECT_TRUE(result == false);
}

/**
 * @tc.name: GetterAndSetterOfPathEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfPathEffect001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto pathEffect = std::make_shared<PathEffect>(PathEffect::PathEffectType::DASH, 0.2f);
    ASSERT_TRUE(pathEffect != nullptr);
    pen->SetPathEffect(pathEffect);
    auto result = pen->GetPathEffect();
    EXPECT_TRUE(result == pathEffect);
}

/**
 * @tc.name: GetterAndSetterOfPathEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfPathEffect002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto pathEffect = std::make_shared<PathEffect>(PathEffect::PathEffectType::COMPOSE, 0.1f);
    ASSERT_TRUE(pathEffect != nullptr);
    pen->SetPathEffect(pathEffect);
    auto result = pen->GetPathEffect();
    EXPECT_TRUE(result == pathEffect);
}

/**
 * @tc.name: GetterAndSetterOfFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfFilter001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    Filter filter;
    pen->SetFilter(filter);
    auto result = pen->GetFilter();
    EXPECT_EQ(result, filter);
    EXPECT_TRUE(pen->HasFilter());
}

/**
 * @tc.name: GetterAndSetterOfFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfFilter002, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>(255);
    ASSERT_TRUE(pen != nullptr);
    Filter filter;
    pen->SetFilter(filter);
    auto result = pen->GetFilter();
    EXPECT_EQ(result, filter);
    EXPECT_TRUE(pen->HasFilter());
}

/**
 * @tc.name: GetterAndSetterOfShaderEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfShaderEffect001, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto shaderEffect = ShaderEffect::CreateColorShader(1);
    ASSERT_TRUE(shaderEffect != nullptr);
    pen->SetShaderEffect(shaderEffect);
    auto result = pen->GetShaderEffect();
    EXPECT_EQ(result, shaderEffect);
}

/**
 * @tc.name: GetterAndSetterOfPathEffect003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfPathEffect003, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto pathEffect = std::make_shared<PathEffect>(PathEffect::PathEffectType::NO_TYPE, 0.5f);
    ASSERT_TRUE(pathEffect != nullptr);
    pen->SetPathEffect(pathEffect);
    auto result = pen->GetPathEffect();
    EXPECT_EQ(result, pathEffect);
    pen->Reset();
}

/**
 * @tc.name: GetterAndSetterOfPathEffect004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, GetterAndSetterOfPathEffect004, TestSize.Level1)
{
    auto pen = std::make_unique<Pen>();
    ASSERT_TRUE(pen != nullptr);
    auto pathEffect = std::make_shared<PathEffect>(PathEffect::PathEffectType::CORNER, 0.2f);
    ASSERT_TRUE(pathEffect != nullptr);
    pen->SetPathEffect(pathEffect);
    auto result = pen->GetPathEffect();
    EXPECT_EQ(result, pathEffect);
    pen->Reset();
}

/**
 * @tc.name: OverrideEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, OverrideEquals001, TestSize.Level1)
{
    Pen pen1;
    Pen pen2 = Pen(255);
    EXPECT_FALSE(pen1 == pen2);
}

/**
 * @tc.name: OverrideEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, OverrideEquals002, TestSize.Level1)
{
    Pen pen1;
    Pen pen2 = pen1;
    EXPECT_TRUE(pen1 == pen2);
}

/**
 * @tc.name: OverrideNotEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, OverrideNotEquals001, TestSize.Level1)
{
    Pen pen1;
    Pen pen2 = Pen(255);
    EXPECT_TRUE(pen1 != pen2);
}

/**
 * @tc.name: OverrideNotEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, OverrideNotEquals002, TestSize.Level1)
{
    Pen pen1 = Pen(128);
    Pen pen2 = Pen(0);
    EXPECT_TRUE(pen1 != pen2);
}

/**
 * @tc.name: SetLoop001
 * @tc.desc: Test for SetLooper
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PenTest, SetLoop001, TestSize.Level1)
{
    Pen pen;
    auto looper = BlurDrawLooper::CreateBlurDrawLooper(1.0, 1, 1, Color::COLOR_BLACK);
    pen.SetLooper(looper);
    EXPECT_TRUE(looper == pen.GetLooper());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
