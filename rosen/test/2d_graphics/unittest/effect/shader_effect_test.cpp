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

#include "effect/shader_effect.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShaderEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ShaderEffectTest::SetUpTestCase() {}
void ShaderEffectTest::TearDownTestCase() {}
void ShaderEffectTest::SetUp() {}
void ShaderEffectTest::TearDown() {}

/*
 * @tc.name: CreateColorShader001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateColorShader001, TestSize.Level1)
{
    ColorQuad color = 13;
    auto newShaderEffect = ShaderEffect::CreateColorShader(color);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateColorShader002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateColorShader002, TestSize.Level1)
{
    ColorQuad color = 24;
    auto newShaderEffect = ShaderEffect::CreateColorShader(color);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateBlendShader001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateBlendShader001, TestSize.Level1)
{
    ShaderEffect shaderEffect1(ShaderEffect::ShaderEffectType::BLEND, 20);
    ShaderEffect shaderEffect2(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, 10);
    auto newShaderEffect = ShaderEffect::CreateBlendShader(shaderEffect1, shaderEffect2, BlendMode::CLEAR);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateBlendShader002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateBlendShader002, TestSize.Level1)
{
    ShaderEffect shaderEffect3(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, 60);
    ShaderEffect shaderEffect4(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, 4);
    auto newShaderEffect = ShaderEffect::CreateBlendShader(shaderEffect3, shaderEffect4, BlendMode::DST);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateImageShader001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateImageShader001, TestSize.Level1)
{
    Image image;
    SamplingOptions sampling;
    Matrix matrix;
    auto newShaderEffect = ShaderEffect::CreateImageShader(image, TileMode::CLAMP, TileMode::REPEAT, sampling, matrix);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateImageShader002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateImageShader002, TestSize.Level1)
{
    Image image;
    SamplingOptions sampling;
    Matrix matrix;
    auto newShaderEffect = ShaderEffect::CreateImageShader(image, TileMode::REPEAT, TileMode::MIRROR, sampling, matrix);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreatePictureShader001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreatePictureShader001, TestSize.Level1)
{
    Picture picture;
    Matrix matrix;
    Rect rect;
    auto newShaderEffect = ShaderEffect::CreatePictureShader(
        picture, TileMode::MIRROR, TileMode::REPEAT, FilterMode::LINEAR, matrix, rect);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreatePictureShader002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreatePictureShader002, TestSize.Level1)
{
    Picture picture;
    Matrix matrix;
    Rect rect;
    auto newShaderEffect =
        ShaderEffect::CreatePictureShader(picture, TileMode::CLAMP, TileMode::REPEAT, FilterMode::LINEAR, matrix, rect);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateLinearGradient001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateLinearGradient001, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = ShaderEffect::CreateLinearGradient(startPoint, endPoint, colors, position, TileMode::CLAMP);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateLinearGradient002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateLinearGradient002, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = ShaderEffect::CreateLinearGradient(startPoint, endPoint, colors, position, TileMode::REPEAT);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateRadialGradient001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateRadialGradient001, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 0.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::MIRROR;
    auto newShaderEffect = ShaderEffect::CreateRadialGradient(centerPoint, radius, colors, position, tileMode);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateRadialGradient002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateRadialGradient002, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 2.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    auto newShaderEffect = ShaderEffect::CreateRadialGradient(centerPoint, radius, colors, position, tileMode);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateTwoPointConical001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateTwoPointConical001, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 0.2f;
    Point endPoint;
    scalar endRadius = 0.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::MIRROR;
    Matrix matrix;
    auto newShaderEffect = ShaderEffect::CreateTwoPointConical(startPoint, startRadius, endPoint, endRadius, colors,
        position, tileMode, &matrix);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateTwoPointConical002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateTwoPointConical002, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 0.1f;
    Point endPoint;
    scalar endRadius = 0.7f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    Matrix matrix;
    auto newShaderEffect = ShaderEffect::CreateTwoPointConical(startPoint, startRadius, endPoint, endRadius, colors,
        position, tileMode, &matrix);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateSweepGradient001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateSweepGradient001, TestSize.Level1)
{
    Point centerPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::MIRROR;
    scalar startAngle = 0.2f;
    scalar endAngle = 0.5f;
    auto newShaderEffect =
        ShaderEffect::CreateSweepGradient(centerPoint, colors, position, tileMode, startAngle, endAngle, nullptr);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: CreateSweepGradient002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateSweepGradient002, TestSize.Level1)
{
    Point centerPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    scalar startAngle = 10.2f;
    scalar endAngle = 10.5f;
    auto newShaderEffect =
        ShaderEffect::CreateSweepGradient(centerPoint, colors, position, tileMode, startAngle, endAngle, nullptr);
    EXPECT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor001, TestSize.Level1)
{
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_SHADER, 230);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor002, TestSize.Level1)
{
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::PICTURE, 100);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor003, TestSize.Level1)
{
    ShaderEffect shaderEffect3(ShaderEffect::ShaderEffectType::IMAGE, 55);
    ShaderEffect shaderEffect4(ShaderEffect::ShaderEffectType::IMAGE, 100);
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::RADIAL_GRADIENT, shaderEffect3, shaderEffect4, BlendMode::SRC);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor004, TestSize.Level1)
{
    ShaderEffect shaderEffect3(ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, 60);
    ShaderEffect shaderEffect4(ShaderEffect::ShaderEffectType::IMAGE, 10);
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, shaderEffect3, shaderEffect4, BlendMode::SRC);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor005, TestSize.Level1)
{
    Image image;
    TileMode tileX = TileMode::REPEAT;
    TileMode tileY = TileMode::MIRROR;
    SamplingOptions sampling;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, image, tileX, tileY, sampling, matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor006, TestSize.Level1)
{
    Image image;
    TileMode tileX = TileMode::REPEAT;
    TileMode tileY = TileMode::CLAMP;
    SamplingOptions sampling;
    Matrix matrix;
    auto newShaderEffect =
        std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::PICTURE, image, tileX, tileY, sampling, matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor007, TestSize.Level1)
{
    Image image;
    SamplingOptions sampling;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::NO_TYPE, image, TileMode::CLAMP, TileMode::REPEAT, sampling, matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor008, TestSize.Level1)
{
    Image image;
    SamplingOptions sampling;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, image, TileMode::REPEAT, TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor009
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor009, TestSize.Level1)
{
    Picture picture;
    Rect rect;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::PICTURE, picture,
        TileMode::REPEAT, TileMode::MIRROR, FilterMode::LINEAR, matrix, rect);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor010
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor010, TestSize.Level1)
{
    Picture picture;
    Rect rect;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, picture,
        TileMode::MIRROR, TileMode::REPEAT, FilterMode::LINEAR, matrix, rect);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor011
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor011, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::NO_TYPE, startPoint, endPoint, colors, position, TileMode::REPEAT);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor012
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor012, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::COLOR_SHADER, startPoint, endPoint, colors, position, TileMode::REPEAT);
    ASSERT_TRUE(newShaderEffect != nullptr);
    auto type = newShaderEffect->GetType();
    EXPECT_EQ(type, ShaderEffect::ShaderEffectType::COLOR_SHADER);
}

/*
 * @tc.name: ArgsContructor013
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor013, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 0.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, centerPoint, radius, colors, position, TileMode::REPEAT);
    ASSERT_TRUE(newShaderEffect != nullptr);
    auto type = newShaderEffect->GetType();
    EXPECT_EQ(type, ShaderEffect::ShaderEffectType::SWEEP_GRADIENT);
}

/*
 * @tc.name: ArgsContructor014
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor014, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 0.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::IMAGE, centerPoint, radius, colors, position, TileMode::REPEAT);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor015
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor015, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 0.1f;
    Point endPoint;
    scalar endRadius = 0.7f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_SHADER, startPoint,
        startRadius, endPoint, endRadius, colors, position, TileMode::REPEAT, &matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor016
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor016, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 55.1f;
    Point endPoint;
    scalar endRadius = 10.7f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, startPoint,
        startRadius, endPoint, endRadius, colors, position, TileMode::REPEAT, &matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor017
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor017, TestSize.Level1)
{
    Point centerPoint;
    scalar startAngle = 42.2f;
    scalar endAngle = 55.7f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::BLEND, centerPoint, colors, position, TileMode::REPEAT, startAngle, endAngle,
        nullptr);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor017
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor018, TestSize.Level1)
{
    Point centerPoint;
    scalar startAngle = 3.55f;
    scalar endAngle = 4.65f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::COLOR_SHADER, centerPoint,
        colors, position, TileMode::REPEAT, startAngle, endAngle, nullptr);
    ASSERT_TRUE(newShaderEffect != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS