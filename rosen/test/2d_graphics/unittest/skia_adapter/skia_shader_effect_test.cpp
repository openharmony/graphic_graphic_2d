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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_shader_effect.h"
#include "draw/color.h"
#include "effect/shader_effect.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaShaderEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaShaderEffectTest::SetUpTestCase() {}
void SkiaShaderEffectTest::TearDownTestCase() {}
void SkiaShaderEffectTest::SetUp() {}
void SkiaShaderEffectTest::TearDown() {}

/**
 * @tc.name: InitWithColor001
 * @tc.desc: Test InitWithColor
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaShaderEffectTest, InitWithColor001, TestSize.Level1)
{
    ColorQuad colorQuad = 0xFF000000;
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithColor(colorQuad);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithBlend001
 * @tc.desc: Test InitWithBlend
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaShaderEffectTest, InitWithBlend001, TestSize.Level1)
{
    ShaderEffect shaderEffect1{ShaderEffect::ShaderEffectType::BLEND, 0xFF000000};
    ShaderEffect shaderEffect2{ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, 0xFF000000};
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithBlend(shaderEffect1, shaderEffect2, BlendMode::CLEAR);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithImage001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithImage001, TestSize.Level1)
{
    Image image;
    Matrix matrix;
    SamplingOptions samplingOptions;
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithImage(image, TileMode::MIRROR, TileMode::REPEAT, samplingOptions, matrix);
    EXPECT_TRUE(skiaShaderEffect.GetShader() == nullptr);
}

/**
 * @tc.name: InitWithPicture001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithPicture001, TestSize.Level1)
{
    Picture picture;
    Matrix matrix;
    Rect rect;
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithPicture(picture, TileMode::MIRROR, TileMode::CLAMP, FilterMode::LINEAR, matrix, rect);
    EXPECT_TRUE(skiaShaderEffect.GetShader() == nullptr);
}

/**
 * @tc.name: InitWithLinearGradient001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithLinearGradient001, TestSize.Level1)
{
    Point startPt;
    Point endPt;
    ColorQuad colorQuad = 20;
    std::vector<ColorQuad> color { colorQuad };
    std::vector<scalar> pos { 30.0f };
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithLinearGradient(startPt, endPt, color, pos, TileMode::MIRROR, nullptr);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithLinearGradient002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithLinearGradient002, TestSize.Level1)
{
    Point startPt;
    Point endPt;
    std::vector<Color4f> color;
    color.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> pos { 30.0f };
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithLinearGradient(startPt, endPt, color, colorSpace, pos, TileMode::MIRROR, nullptr);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
    SkiaShaderEffect skiaShaderEffect2;
    skiaShaderEffect2.InitWithLinearGradient(startPt, endPt, color, nullptr, pos, TileMode::MIRROR, nullptr);
    EXPECT_TRUE(skiaShaderEffect2.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithRadialGradient001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithRadialGradient001, TestSize.Level1)
{
    Point centerPt;
    scalar radius = 15.0f;
    ColorQuad colorQuad = 30;
    std::vector<ColorQuad> color { colorQuad };
    std::vector<scalar> pos { 16.0f };
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithRadialGradient(centerPt, radius, color, pos, TileMode::REPEAT, nullptr);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithRadialGradient002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithRadialGradient002, TestSize.Level1)
{
    Point centerPt;
    scalar radius = 15.0f;
    std::vector<Color4f> color;
    color.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> pos { 16.0f };
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithRadialGradient(centerPt, radius, color, colorSpace, pos, TileMode::REPEAT, nullptr);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
    SkiaShaderEffect skiaShaderEffect2;
    skiaShaderEffect2.InitWithRadialGradient(centerPt, radius, color, nullptr, pos, TileMode::REPEAT, nullptr);
    EXPECT_TRUE(skiaShaderEffect2.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithTwoPointConical001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithTwoPointConical001, TestSize.Level1)
{
    Point startPt;
    scalar startRadius = 10.0f;
    Point endPt;
    scalar endRadius = 25.0f;
    ColorQuad colorQuad = 15;
    std::vector<ColorQuad> color { colorQuad };
    std::vector<scalar> pos { 30.0f };
    SkiaShaderEffect skiaShaderEffect;
    Matrix matrix;
    skiaShaderEffect.InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, color, pos, TileMode::CLAMP,
        &matrix);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithTwoPointConical002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithTwoPointConical002, TestSize.Level1)
{
    Point startPt;
    scalar startRadius = 10.0f;
    Point endPt;
    scalar endRadius = 25.0f;
    std::vector<Color4f> color;
    color.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> pos { 30.0f };
    SkiaShaderEffect skiaShaderEffect;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    Matrix matrix;
    skiaShaderEffect.InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, color, colorSpace, pos, TileMode::CLAMP,
        &matrix);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
    SkiaShaderEffect skiaShaderEffect2;
    skiaShaderEffect2.InitWithTwoPointConical(startPt, startRadius, endPt, endRadius, color, nullptr, pos, TileMode::CLAMP,
        &matrix);
    EXPECT_TRUE(skiaShaderEffect2.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithSweepGradient001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithSweepGradient001, TestSize.Level1)
{
    Point centerPt;
    ColorQuad colorQuad = 10;
    std::vector<ColorQuad> color { colorQuad };
    std::vector<scalar> pos { 30.0f };
    scalar startAngle = 45.0f;
    scalar endAngle = 60.0f;
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithSweepGradient(centerPt, color, pos, TileMode::MIRROR, startAngle, endAngle, nullptr);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithSweepGradient002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithSweepGradient002, TestSize.Level1)
{
    Point centerPt;
    std::vector<Color4f> color;
    color.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> pos { 30.0f };
    scalar startAngle = 45.0f;
    scalar endAngle = 60.0f;
    SkiaShaderEffect skiaShaderEffect;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    skiaShaderEffect.InitWithSweepGradient(centerPt, color, colorSpace, pos, TileMode::MIRROR, startAngle, endAngle, nullptr);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
    SkiaShaderEffect skiaShaderEffect2;
    skiaShaderEffect2.InitWithSweepGradient(centerPt, color, nullptr, pos, TileMode::MIRROR, startAngle, endAngle, nullptr);
    EXPECT_TRUE(skiaShaderEffect2.GetShader() != nullptr);
}

/**
 * @tc.name: InitWithLightUp001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithLightUp001, TestSize.Level1)
{
    const std::shared_ptr<Drawing::ShaderEffect> imageShader = std::make_shared<Drawing::ShaderEffect>();
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithLightUp(1.0f, *imageShader);
    EXPECT_TRUE(skiaShaderEffect.GetShader() != nullptr);
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaShaderEffectTest, Serialize001, TestSize.Level1)
{
    SkiaShaderEffect skiaShaderEffect;
    auto shader1 = skiaShaderEffect.GetShader();
    EXPECT_TRUE(shader1 == nullptr);
    skiaShaderEffect.Serialize();
    auto shader2 = skiaShaderEffect.GetShader();
    EXPECT_TRUE(shader2 == nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS