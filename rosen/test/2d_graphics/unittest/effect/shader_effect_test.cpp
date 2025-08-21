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
#include "effect/shader_effect_lazy.h"
#include "draw/blend_mode.h"
#ifdef RS_ENABLE_GPU
#include "image/gpu_context.h"
#endif
#include "image/image.h"
#include "utils/object_helper.h"
#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#endif
#include "transaction/rs_marshalling_helper.h"

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

private:
#ifdef ROSEN_OHOS
    static std::function<bool(Parcel&, std::shared_ptr<Data>)> originalMarshallingCallback_;
    static std::function<std::shared_ptr<Data>(Parcel&)> originalUnmarshallingCallback_;
#endif
};

#ifdef ROSEN_OHOS
std::function<bool(Parcel&, std::shared_ptr<Data>)> ShaderEffectTest::originalMarshallingCallback_;
std::function<std::shared_ptr<Data>(Parcel&)> ShaderEffectTest::originalUnmarshallingCallback_;
#endif

void ShaderEffectTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    // Save original callbacks
    originalMarshallingCallback_ = ObjectHelper::Instance().GetDataMarshallingCallback();
    originalUnmarshallingCallback_ = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Register Data marshalling/unmarshalling callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Data> data) -> bool {
            return OHOS::Rosen::RSMarshallingHelper::Marshalling(parcel, data);
        }
    );
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Data> {
            std::shared_ptr<Data> data;
            return OHOS::Rosen::RSMarshallingHelper::Unmarshalling(parcel, data) ? data : nullptr;
        }
    );
#endif
}

void ShaderEffectTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    // Restore original callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(originalMarshallingCallback_);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalUnmarshallingCallback_);
#endif
}
void ShaderEffectTest::SetUp() {}
void ShaderEffectTest::TearDown() {}

// Create a mock shader that will return null from Serialize
class MockShaderEffect : public ShaderEffect {
public:
    MockShaderEffect() : ShaderEffect(ShaderEffectType::COLOR_SHADER) {}

    std::shared_ptr<Data> Serialize() const override
    {
        return nullptr;
    }
};

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
 * @tc.name: CreateBlendShader003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateBlendShader003, TestSize.Level1)
{
    ShaderEffect shaderEffect3(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, 60);
    ShaderEffect shaderEffect4(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, 4);
    auto newShaderEffect = ShaderEffect::CreateBlendShader(shaderEffect3, shaderEffect4, BlendMode::DST);
    EXPECT_TRUE(newShaderEffect != nullptr);
    EXPECT_FALSE(newShaderEffect->IsLazy());
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
 * @tc.name: CreateLinearGradient003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateLinearGradient003, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> position;
    auto newShaderEffect = ShaderEffect::CreateLinearGradient(startPoint, endPoint, colors, colorSpace,
        position,TileMode::REPEAT);
    EXPECT_TRUE(newShaderEffect != nullptr);
    colors.clear();
    auto newShaderEffect2 = ShaderEffect::CreateLinearGradient(startPoint, endPoint, colors, colorSpace,
        position,TileMode::REPEAT);
    EXPECT_TRUE(newShaderEffect2 != nullptr);
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
 * @tc.name: CreateRadialGradient003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateRadialGradient003, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 2.5f;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    Matrix matrix;
    auto newShaderEffect = ShaderEffect::CreateRadialGradient(centerPoint, radius, colors, colorSpace, position,
        tileMode, &matrix);
    EXPECT_TRUE(newShaderEffect != nullptr);
    colors.clear();
    auto newShaderEffect2 = ShaderEffect::CreateRadialGradient(centerPoint, radius, colors, colorSpace, position,
        tileMode, &matrix);
    EXPECT_TRUE(newShaderEffect2 != nullptr);
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
 * @tc.name: CreateTwoPointConical003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateTwoPointConical003, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 0.1f;
    Point endPoint;
    scalar endRadius = 0.7f;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    Matrix matrix;
    auto newShaderEffect = ShaderEffect::CreateTwoPointConical(startPoint, startRadius, endPoint, endRadius, colors,
        colorSpace, position, tileMode, &matrix);
    EXPECT_TRUE(newShaderEffect != nullptr);
    colors.clear();
    auto newShaderEffect2 = ShaderEffect::CreateTwoPointConical(startPoint, startRadius, endPoint, endRadius, colors,
        colorSpace, position, tileMode, &matrix);
    EXPECT_TRUE(newShaderEffect2 != nullptr);
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
 * @tc.name: CreateSweepGradient003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateSweepGradient003, TestSize.Level1)
{
    Point centerPoint;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    scalar startAngle = 10.2f;
    scalar endAngle = 10.5f;
    auto newShaderEffect =
        ShaderEffect::CreateSweepGradient(centerPoint, colors, colorSpace, position, tileMode, startAngle,
        endAngle, nullptr);
    EXPECT_TRUE(newShaderEffect != nullptr);
    colors.clear();
    auto newShaderEffect2 =
        ShaderEffect::CreateSweepGradient(centerPoint, colors, colorSpace, position, tileMode, startAngle,
        endAngle, nullptr);
    EXPECT_TRUE(newShaderEffect2 != nullptr);
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

/*
 * @tc.name: ArgsContructor018
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor019, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> position;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT, startPoint,
        endPoint, colors, colorSpace, position, TileMode::REPEAT, &matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor019
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor020, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 55.1f;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> position;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    TileMode mode = TileMode::REPEAT;
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT, centerPoint,
        radius, colors, colorSpace, position, mode, &matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor020
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor021, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 55.1f;
    Point endPoint;
    scalar endRadius = 10.7f;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> position;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(ShaderEffect::ShaderEffectType::CONICAL_GRADIENT, startPoint,
        startRadius, endPoint, endRadius, colors, colorSpace, position, TileMode::REPEAT, &matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
}

/*
 * @tc.name: ArgsContructor021
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, ArgsContructor022, TestSize.Level1)
{
    Point centerPoint;
    scalar startAngle = 0.0f;
    scalar endAngle = 180.0f;
    std::vector<Color4f> colors;
    colors.push_back({0.1, 0.1, 0.1, 1.0f});
    std::vector<scalar> position;
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    Matrix matrix;
    auto newShaderEffect = std::make_unique<ShaderEffect>(
        ShaderEffect::ShaderEffectType::SWEEP_GRADIENT, centerPoint, colors, colorSpace, position, TileMode::REPEAT,
        startAngle, endAngle, &matrix);
    ASSERT_TRUE(newShaderEffect != nullptr);
    auto type = newShaderEffect->GetType();
    EXPECT_EQ(type, ShaderEffect::ShaderEffectType::SWEEP_GRADIENT);
}

/*
 * @tc.name: SetGPUContext001
 * @tc.desc: test ShaderEffect
 * @tc.type: FUNC
 * @tc.require: issue#ICHPKE
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, SetGPUContext001, TestSize.Level1)
{
    auto shaderEffect = ShaderEffect::CreateColorShader(Color::COLOR_TRANSPARENT);
    ASSERT_TRUE(shaderEffect != nullptr);
#ifdef RS_ENABLE_GPU
    auto gpuContext = std::make_shared<GPUContext>();
    shaderEffect->SetGPUContext(gpuContext);
#endif
}

/*
 * @tc.name: CreateBlendShader004
 * @tc.desc: Test CreateBlendShader with Lazy dst shader (should fail)
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateBlendShader004, TestSize.Level1)
{
    auto baseShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto lazyDstShader = ShaderEffectLazy::CreateBlendShader(baseShader, baseShader, BlendMode::SRC_OVER);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    ASSERT_TRUE(lazyDstShader != nullptr);
    ASSERT_TRUE(srcShader != nullptr);
    // Check shader types
    EXPECT_TRUE(lazyDstShader->IsLazy());
    EXPECT_FALSE(srcShader->IsLazy());

    // CreateBlendShader should fail with Lazy dst shader
    auto blendShader = ShaderEffect::CreateBlendShader(*lazyDstShader, *srcShader, BlendMode::MULTIPLY);
    EXPECT_TRUE(blendShader == nullptr); // Should return nullptr due to Lazy dst shader
}

/*
 * @tc.name: CreateBlendShader005
 * @tc.desc: Test CreateBlendShader with Lazy src shader (should fail)
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, CreateBlendShader005, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto baseShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazySrcShader = ShaderEffectLazy::CreateBlendShader(baseShader, baseShader, BlendMode::SRC_OVER);
    ASSERT_TRUE(dstShader != nullptr);
    ASSERT_TRUE(lazySrcShader != nullptr);
    // Check shader types
    EXPECT_FALSE(dstShader->IsLazy());
    EXPECT_TRUE(lazySrcShader->IsLazy());

    // CreateBlendShader should fail with Lazy src shader
    auto blendShader = ShaderEffect::CreateBlendShader(*dstShader, *lazySrcShader, BlendMode::SCREEN);
    EXPECT_TRUE(blendShader == nullptr); // Should return nullptr due to Lazy src shader
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: UnmarshallingCompleteRoundTrip001
 * @tc.desc: Test complete round-trip marshalling and serialization data consistency
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, UnmarshallingCompleteRoundTrip001, TestSize.Level1)
{
    // Test 1: Complete round-trip with multiple shader types
    std::vector<std::shared_ptr<ShaderEffect>> testShaders;
    // ColorShader
    testShaders.push_back(ShaderEffect::CreateColorShader(Color::COLOR_BLUE));

    // LinearGradient
    Point startPt(0, 0);
    Point endPt(100, 100);
    std::vector<ColorQuad> colors = {Color::COLOR_RED, Color::COLOR_GREEN, Color::COLOR_BLUE};
    std::vector<scalar> pos = {0.0f, 0.5f, 1.0f};
    testShaders.push_back(ShaderEffect::CreateLinearGradient(startPt, endPt, colors, pos, TileMode::CLAMP));

    // RadialGradient
    Point center(50, 50);
    scalar radius = 25.0f;
    std::vector<ColorQuad> radialColors = {Color::COLOR_BLACK, Color::COLOR_WHITE};
    std::vector<scalar> radialPos = {0.0f, 1.0f};
    testShaders.push_back(ShaderEffect::CreateRadialGradient(center,
        radius, radialColors, radialPos, TileMode::REPEAT));

    for (auto& originalShader : testShaders) {
        ASSERT_NE(originalShader, nullptr);
        auto originalType = originalShader->GetType();

        // Marshal and unmarshal
        Parcel parcel;
        bool marshalResult = originalShader->Marshalling(parcel);
        EXPECT_TRUE(marshalResult);

        bool isValid = true;
        auto unmarshaledShader = ShaderEffect::Unmarshalling(parcel, isValid);
        EXPECT_NE(unmarshaledShader, nullptr);
        EXPECT_TRUE(isValid);
        EXPECT_EQ(unmarshaledShader->GetType(), originalType);

        // Verify data serialization consistency
        auto originalData = originalShader->Serialize();
        auto unmarshaledData = unmarshaledShader->Serialize();
        if (originalData && unmarshaledData) {
            EXPECT_EQ(originalData->GetSize(), unmarshaledData->GetSize());
            const uint8_t* originalBytes = static_cast<const uint8_t*>(originalData->GetData());
            const uint8_t* unmarshaledBytes = static_cast<const uint8_t*>(unmarshaledData->GetData());
            int memResult = memcmp(originalBytes, unmarshaledBytes, originalData->GetSize());
            EXPECT_EQ(memResult, 0);
        } else {
            // If either serialization fails, both should fail consistently
            EXPECT_EQ(originalData, unmarshaledData);
        }
    }
}

/*
 * @tc.name: UnmarshallingErrorHandling001
 * @tc.desc: Test ShaderEffect::Unmarshalling error handling scenarios and boundary conditions
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectTest, UnmarshallingErrorHandling001, TestSize.Level1)
{
    // Test 1: Empty parcel
    {
        Parcel emptyParcel;
        bool isValid = true;
        auto result = ShaderEffect::Unmarshalling(emptyParcel, isValid);
        EXPECT_EQ(result, nullptr);
    }

    // Test 2: NO_TYPE - 1 (negative boundary)
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::NO_TYPE) - 1);
        bool isValid = true;
        auto result = ShaderEffect::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected due to invalid type
    }

    // Test 3: NO_TYPE (lower boundary) - can construct empty ShaderEffect
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::NO_TYPE));
        parcel.WriteInt32(false);
        bool isValid = true;
        auto result = ShaderEffect::Unmarshalling(parcel, isValid);
        EXPECT_NE(result, nullptr); // Should succeed, creating empty ShaderEffect
        EXPECT_TRUE(isValid);
        if (result) {
            EXPECT_EQ(result->GetType(), ShaderEffect::ShaderEffectType::NO_TYPE);
        }
    }

    // Test 4: LAZY_SHADER (should be rejected in normal ShaderEffect unmarshalling)
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::LAZY_SHADER));
        bool isValid = true;
        auto result = ShaderEffect::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected
    }

    // Test 5: Beyond LAZY_SHADER (upper boundary + 1)
    {
        Parcel parcel;
        parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::LAZY_SHADER) + 1);
        bool isValid = true;
        auto result = ShaderEffect::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected due to invalid type
    }

    // Test 6: Large invalid value
    {
        Parcel parcel;
        parcel.WriteInt32(999);
        bool isValid = true;
        auto result = ShaderEffect::Unmarshalling(parcel, isValid);
        EXPECT_EQ(result, nullptr); // Should be rejected
    }
}

/*
 * @tc.name: MarshallingEmptyData001
 * @tc.desc: Test ShaderEffect::Marshalling with empty Serialize data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, MarshallingEmptyData001, TestSize.Level1)
{
    auto mockShader = std::make_shared<MockShaderEffect>();
    Parcel parcel;
    // Should succeed even with null Serialize data
    bool result = mockShader->Marshalling(parcel);
    EXPECT_TRUE(result);

    // Verify the parcel contains the expected structure
    // Read type
    int32_t type;
    EXPECT_TRUE(parcel.ReadInt32(type));
    EXPECT_EQ(type, static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER));

    // Read hasData flag - should be false
    bool hasData;
    EXPECT_TRUE(parcel.ReadBool(hasData));
    EXPECT_FALSE(hasData);

    // No more data should be available since hasData was false
    EXPECT_EQ(parcel.GetDataSize() - parcel.GetReadPosition(), 0);
}

/*
 * @tc.name: UnmarshallingEmptyData001
 * @tc.desc: Test ShaderEffect::Unmarshalling with empty data marker
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, UnmarshallingEmptyData001, TestSize.Level1)
{
    Parcel parcel;
    // Write type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER)));
    // Write hasData as false to simulate empty data scenario
    EXPECT_TRUE(parcel.WriteBool(false));

    // Should successfully create empty shader
    bool isValid = true;
    auto shader = ShaderEffect::Unmarshalling(parcel, isValid);
    EXPECT_NE(shader, nullptr);
    EXPECT_TRUE(isValid);
    EXPECT_EQ(shader->GetType(), ShaderEffect::ShaderEffectType::COLOR_SHADER);
}

/*
 * @tc.name: MarshallingUnmarshallingEmptyData001
 * @tc.desc: Test round-trip Marshalling and Unmarshalling with empty data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, MarshallingUnmarshallingEmptyData001, TestSize.Level1)
{
    auto originalShader = std::make_shared<MockShaderEffect>();
    Parcel parcel;
    // Marshal - should succeed with empty data
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal - should create empty shader with correct type
    bool isValid = true;
    auto unmarshaledShader = ShaderEffect::Unmarshalling(parcel, isValid);
    EXPECT_NE(unmarshaledShader, nullptr);
    EXPECT_TRUE(isValid);
    EXPECT_EQ(unmarshaledShader->GetType(), ShaderEffect::ShaderEffectType::COLOR_SHADER);

    // Serialize validation - both should return null consistently
    auto originalData = originalShader->Serialize();
    auto unmarshaledData = unmarshaledShader->Serialize();
    EXPECT_EQ(originalData, nullptr);
    EXPECT_EQ(unmarshaledData, nullptr);
}

/*
 * @tc.name: MarshallingCallbackFailure001
 * @tc.desc: Test ShaderEffect::Marshalling with callback failure to cover (!callback(parcel, data)) branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, MarshallingCallbackFailure001, TestSize.Level1)
{
    // Create a valid shader with non-null Serialize data
    ColorQuad color = 0xFF0000FF;
    auto shader = ShaderEffect::CreateColorShader(color);
    ASSERT_NE(shader, nullptr);

    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataMarshallingCallback();

    // Set a failing callback to trigger the (!callback(parcel, data)) branch
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return false; // Always fail
        }
    );

    Parcel parcel;
    bool result = shader->Marshalling(parcel);
    // Should fail due to callback failure
    EXPECT_FALSE(result);

    // Restore original callback
    ObjectHelper::Instance().SetDataMarshallingCallback(originalCallback);
}

/*
 * @tc.name: UnmarshallingCallbackNull001
 * @tc.desc: Test ShaderEffect::Unmarshalling with null callback to cover (if (!callback)) branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, UnmarshallingCallbackNull001, TestSize.Level1)
{
    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Set null callback to trigger the (if (!callback)) branch
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);

    Parcel parcel;
    // Write valid type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER)));
    // Write hasData as true to reach the callback check
    EXPECT_TRUE(parcel.WriteBool(true));

    bool isValid = true;
    auto result = ShaderEffect::Unmarshalling(parcel, isValid);
    // Should fail due to null callback
    EXPECT_EQ(result, nullptr);

    // Restore original callback
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalCallback);
}

/*
 * @tc.name: MarshallingWriteTypeFailure001
 * @tc.desc: Test ShaderEffect::Marshalling with WriteInt32(type) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, MarshallingWriteTypeFailure001, TestSize.Level1)
{
    // Create a valid shader
    auto shader = ShaderEffect::CreateColorShader(Color::COLOR_BLUE);
    ASSERT_NE(shader, nullptr);

    // Create buffer to fill parcel capacity (200K minimum)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Fill parcel completely, then try Marshalling (should fail on WriteInt32(type))
    Parcel parcel;
    parcel.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult = parcel.WriteBuffer(fillBuffer.data(), BUFFER_SIZE);
    EXPECT_TRUE(fillResult);

    bool result = shader->Marshalling(parcel);
    EXPECT_FALSE(result); // Should fail due to WriteInt32 failure
}

/*
 * @tc.name: MarshallingWriteHasDataFailure001
 * @tc.desc: Test ShaderEffect::Marshalling with WriteBool(hasValidData) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, MarshallingWriteHasDataFailure001, TestSize.Level1)
{
    // Create a valid shader
    auto shader = ShaderEffect::CreateColorShader(Color::COLOR_RED);
    ASSERT_NE(shader, nullptr);

    // Create buffer to fill parcel capacity (200K minimum)
    const size_t BUFFER_SIZE = 200 * 1024; // 200K
    std::vector<uint8_t> fillBuffer(BUFFER_SIZE, 0xFF);

    // Fill parcel leaving space for int32 only (4 bytes), should fail on WriteBool
    Parcel parcel;
    parcel.SetMaxCapacity(BUFFER_SIZE);
    bool fillResult = parcel.WriteBuffer(fillBuffer.data(), BUFFER_SIZE - 4);
    EXPECT_TRUE(fillResult);
    
    bool result = shader->Marshalling(parcel);
    EXPECT_FALSE(result); // Should fail due to WriteBool failure
}

/*
 * @tc.name: MarshallingCallbackNull001
 * @tc.desc: Test ShaderEffect::Marshalling with null DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, MarshallingCallbackNull001, TestSize.Level1)
{
    // Create a valid shader
    auto shader = ShaderEffect::CreateColorShader(Color::COLOR_GREEN);
    ASSERT_NE(shader, nullptr);

    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataMarshallingCallback();

    // Set null callback to trigger the (if (!callback)) branch
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);

    Parcel parcel;
    bool result = shader->Marshalling(parcel);
    EXPECT_FALSE(result); // Should fail due to null callback

    // Restore original callback
    ObjectHelper::Instance().SetDataMarshallingCallback(originalCallback);
}

/*
 * @tc.name: UnmarshallingReadHasDataFailure001
 * @tc.desc: Test ShaderEffect::Unmarshalling with ReadBool(hasData) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, UnmarshallingReadHasDataFailure001, TestSize.Level1)
{
    Parcel parcel;
    // Write valid type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER)));
    // Don't write hasData bool, leaving parcel incomplete

    bool isValid = true;
    auto result = ShaderEffect::Unmarshalling(parcel, isValid);
    EXPECT_EQ(result, nullptr); // Should fail due to ReadBool failure
}

/*
 * @tc.name: UnmarshallingCallbackReturnNull001
 * @tc.desc: Test ShaderEffect::Unmarshalling with callback returning null data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectTest, UnmarshallingCallbackReturnNull001, TestSize.Level1)
{
    // Backup original callback
    auto originalCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Set callback that returns null data to trigger the (if (!data)) branch
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            return nullptr; // Always return null
        }
    );

    Parcel parcel;
    // Write valid type
    EXPECT_TRUE(parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER)));
    // Write hasData as true to reach the callback
    EXPECT_TRUE(parcel.WriteBool(true));

    bool isValid = true;
    auto result = ShaderEffect::Unmarshalling(parcel, isValid);
    EXPECT_EQ(result, nullptr); // Should fail due to null data from callback

    // Restore original callback
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalCallback);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS