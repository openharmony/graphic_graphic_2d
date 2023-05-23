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

#include "recording/recording_shader_effect.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingShaderEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingShaderEffectTest::SetUpTestCase() {}
void RecordingShaderEffectTest::TearDownTestCase() {}
void RecordingShaderEffectTest::SetUp() {}
void RecordingShaderEffectTest::TearDown() {}

/**
 * @tc.name: CreateColorShader001
 * @tc.desc: Test for CreateColorShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateColorShader001, TestSize.Level1)
{
    ColorQuad color = 13;
    auto newRecordingShaderEffect = RecordingShaderEffect::CreateColorShader(color);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
}

/**
 * @tc.name: CreateColorShader002
 * @tc.desc: Test the playback of the CreateColorShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateColorShader002, TestSize.Level1)
{
    ColorQuad color = 24;
    auto newRecordingShaderEffect = RecordingShaderEffect::CreateColorShader(color);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::COLOR);
}

/**
 * @tc.name: CreateBlendShader001
 * @tc.desc: Test for CreateBlendShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateBlendShader001, TestSize.Level1)
{
    RecordingShaderEffect shaderEffect1;
    RecordingShaderEffect shaderEffect2;
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateBlendShader(shaderEffect1, shaderEffect2, BlendMode::CLEAR);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
}

/**
 * @tc.name: CreateBlendShader002
 * @tc.desc: Test the playback of the CreateBlendShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateBlendShader002, TestSize.Level1)
{
    ColorQuad color3 = 24;
    ColorQuad color4 = 13;
    auto shaderEffect3 = RecordingShaderEffect::CreateColorShader(color3);
    auto shaderEffect4 = RecordingShaderEffect::CreateColorShader(color4);
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateBlendShader(*shaderEffect3.get(), *shaderEffect4.get(), BlendMode::DST);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::BLEND);
}

/**
 * @tc.name: CreateImageShader001
 * @tc.desc: Test the playback of the CreateImageShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateImageShader001, TestSize.Level1)
{
    Image image;
    Bitmap bitmap;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(100, 200, bitmapFormat);
    image.BuildFromBitmap(bitmap);
    SamplingOptions sampling;
    Matrix matrix;
    matrix.SetMatrix(200, 150, 800, 60, 200, 150, 800, 60, 90);
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateImageShader(image, TileMode::CLAMP, TileMode::REPEAT, sampling, matrix);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::IMAGE);
}

/**
 * @tc.name: CreateImageShader002
 * @tc.desc: Test the playback of the CreateImageShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateImageShader002, TestSize.Level1)
{
    Image image;
    Bitmap bitmap;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(100, 200, bitmapFormat);
    image.BuildFromBitmap(bitmap);
    SamplingOptions sampling;
    Matrix matrix;
    matrix.SetMatrix(200, 150, 800, 60, 200, 150, 800, 60, 90);
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateImageShader(image, TileMode::REPEAT, TileMode::MIRROR, sampling, matrix);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::IMAGE);
}

/**
 * @tc.name: CreatePictureShader001
 * @tc.desc: Test the playback of the CreatePictureShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreatePictureShader001, TestSize.Level1)
{
    Picture picture;
    Matrix matrix;
    Rect rect;
    auto newRecordingShaderEffect = RecordingShaderEffect::CreatePictureShader(
        picture, TileMode::MIRROR, TileMode::REPEAT, FilterMode::LINEAR, matrix, rect);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::PICTURE);
}

/**
 * @tc.name: CreatePictureShader002
 * @tc.desc: Test the playback of the CreatePictureShader function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreatePictureShader002, TestSize.Level1)
{
    Picture picture;
    auto data = std::make_shared<Data>();
    ASSERT_TRUE(data != nullptr);
    auto intData = new int();
    EXPECT_TRUE(data->BuildFromMalloc(intData, sizeof(intData)));
    picture.Deserialize(data);
    Matrix matrix;
    Rect rect;
    auto newRecordingShaderEffect = RecordingShaderEffect::CreatePictureShader(
        picture, TileMode::CLAMP, TileMode::REPEAT, FilterMode::LINEAR, matrix, rect);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::PICTURE);
}

/**
 * @tc.name: CreateLinearGradient001
 * @tc.desc: Test the playback of the CreateLinearGradient function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateLinearGradient001, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateLinearGradient(startPoint, endPoint, colors, position, TileMode::CLAMP);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::LINEAR_GRADIENT);
}

/**
 * @tc.name: CreateLinearGradient002
 * @tc.desc: Test the playback of the CreateLinearGradient function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateLinearGradient002, TestSize.Level1)
{
    Point startPoint;
    Point endPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateLinearGradient(startPoint, endPoint, colors, position, TileMode::REPEAT);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::LINEAR_GRADIENT);
}

/**
 * @tc.name: CreateRadialGradient001
 * @tc.desc: Test the playback of the CreateRadialGradient function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateRadialGradient001, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 0.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::MIRROR;
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateRadialGradient(centerPoint, radius, colors, position, tileMode);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
}

/**
 * @tc.name: CreateRadialGradient002
 * @tc.desc: Test the playback of the CreateRadialGradient function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateRadialGradient002, TestSize.Level1)
{
    Point centerPoint;
    scalar radius = 2.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateRadialGradient(centerPoint, radius, colors, position, tileMode);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::RADIAL_GRADIENT);
}

/**
 * @tc.name: CreateTwoPointConical001
 * @tc.desc: Test the playback of the CreateTwoPointConical function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateTwoPointConical001, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 0.2f;
    Point endPoint;
    scalar endRadius = 0.5f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::MIRROR;
    auto newRecordingShaderEffect = RecordingShaderEffect::CreateTwoPointConical(
        startPoint, startRadius, endPoint, endRadius, colors, position, tileMode);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
}

/**
 * @tc.name: CreateTwoPointConical002
 * @tc.desc: Test the playback of the CreateTwoPointConical function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateTwoPointConical002, TestSize.Level1)
{
    Point startPoint;
    scalar startRadius = 0.1f;
    Point endPoint;
    scalar endRadius = 0.7f;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    auto newRecordingShaderEffect = RecordingShaderEffect::CreateTwoPointConical(
        startPoint, startRadius, endPoint, endRadius, colors, position, tileMode);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::CONICAL_GRADIENT);
}

/**
 * @tc.name: CreateSweepGradient001
 * @tc.desc: Test the playback of the CreateSweepGradient function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateSweepGradient001, TestSize.Level1)
{
    Point centerPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::MIRROR;
    scalar startAngle = 0.2f;
    scalar endAngle = 0.5f;
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateSweepGradient(centerPoint, colors, position, tileMode, startAngle, endAngle);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::SWEEP_GRADIENT);
}

/**
 * @tc.name: CreateSweepGradient002
 * @tc.desc: Test the playback of the CreateSweepGradient function.
 * @tc.type: FUNC
 * @tc.require:I77IX8
 */
HWTEST_F(RecordingShaderEffectTest, CreateSweepGradient002, TestSize.Level1)
{
    Point centerPoint;
    std::vector<ColorQuad> colors;
    std::vector<scalar> position;
    TileMode tileMode = TileMode::REPEAT;
    scalar startAngle = 10.2f;
    scalar endAngle = 10.5f;
    auto newRecordingShaderEffect =
        RecordingShaderEffect::CreateSweepGradient(centerPoint, colors, position, tileMode, startAngle, endAngle);
    EXPECT_TRUE(newRecordingShaderEffect != nullptr);
    auto shaderEffectCmdList = newRecordingShaderEffect->GetCmdList();
    EXPECT_TRUE(shaderEffectCmdList != nullptr);
    auto shaderEffect = shaderEffectCmdList->Playback();
    EXPECT_TRUE(shaderEffect != nullptr);
    EXPECT_EQ(shaderEffect->GetType(), ShaderEffect::ShaderEffectType::SWEEP_GRADIENT);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
