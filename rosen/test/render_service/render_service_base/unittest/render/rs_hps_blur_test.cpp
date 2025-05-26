/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "image/bitmap.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_hps_blur.h"
#include "utils/rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpsBlurTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void RSHpsBlurTest::SetUpTestCase() {}
void RSHpsBlurTest::TearDownTestCase() {}
void RSHpsBlurTest::SetUp()
{
    canvas_.Restore();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void RSHpsBlurTest::TearDown() {}

/**
 * @tc.name: GetShaderTransformTest
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, GetShaderTransformTest, TestSize.Level1)
{
    Drawing::Matrix matrix;
    Drawing::Rect blurRect(0, 0, 100, 100);
    float scaleW = 1.0;
    float scaleH = 1.0;

    HpsBlurFilter filter;
    EXPECT_EQ(filter.GetShaderTransform(blurRect, scaleW, scaleH), matrix);
}

/**
 * @tc.name: GetMixEffectTest
 * @tc.desc: Verify function GetMixEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, GetMixEffectTest, TestSize.Level1)
{
    auto mixEffect = HpsBlurFilter::GetHpsBlurFilter().GetMixEffect();
    EXPECT_NE(mixEffect, nullptr);
}

/**
 * @tc.name: SetShaderEffectTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, linear, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTileXIsREPEATTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTileXIsREPEATTest, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::REPEAT, Drawing::TileMode::CLAMP, linear, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTileYIsREPEATTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTileYIsREPEATTest, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::REPEAT, linear, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTileXIsMIRRORTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTileXIsMIRRORTest, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::MIRROR, Drawing::TileMode::CLAMP, linear, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTileYIsMIRRORTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTileYIsMIRRORTest, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::MIRROR, linear, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTileXIsDECALTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTileXIsDECALTest, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::DECAL, Drawing::TileMode::CLAMP, linear, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTileYIsCLAMPTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTileYIsCLAMPTest, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::DECAL, linear, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest001TileXIsCLAMP
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest001TileXIsCLAMP, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::DECAL, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest001TileXIsDECAL
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest001TileXIsDECAL, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::DECAL, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest001TileYIsMIRROR
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest001TileYIsMIRROR, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::MIRROR, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest001TileYIsCLAMP
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest001TileYIsCLAMP, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::MIRROR, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest001TileYIsREPEAT
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest001TileYIsREPEAT, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::REPEAT, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest001TileYIsCLAMP
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest005TileYIsCLAMP, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::REPEAT, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest002
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest002, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::REPEAT, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest003
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest003, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::REPEAT, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest004
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest004, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::REPEAT, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest005
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest005, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::MIRROR, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest006
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest006, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::MIRROR, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest007
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest007, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::DECAL, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest008
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest008, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::DECAL, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest009
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest009, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::REPEAT, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest010
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest010, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::REPEAT, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest011
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest011, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::REPEAT, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest012
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest012, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::MIRROR, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest013
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest013, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::MIRROR, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest014
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest014, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::DECAL, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest015
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest015, TestSize.Level1)
{
    Drawing::Brush brush;
    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::DECAL, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetShaderEffectTest001
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, SetShaderEffectTest001, TestSize.Level1)
{
    Drawing::Brush brush;

    Drawing::SamplingOptions nearest(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    Drawing::Matrix blurMatrix;
    auto imageCache = std::make_shared<Drawing::Image>();
    ASSERT_NE(imageCache, nullptr);
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(
        *imageCache, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, nearest, blurMatrix);

    bool res = HpsBlurFilter::GetHpsBlurFilter().SetShaderEffect(brush, blurShader, imageCache);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: ApplyHpsBlurTest001
 * @tc.desc: Verify function ApplyHpsBlur
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    float radius = 10;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(canvas_, image, param, alpha, colorFilter), false);

    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha1 = 13;
    RSColor maskColor(red, green, blue, alpha1);
    EXPECT_EQ(filter.ApplyHpsBlur(canvas_, image, param, alpha1, colorFilter, maskColor), false);
    EXPECT_EQ(filter.ApplyHpsBlur(canvas_, image, param, 0.0, colorFilter, maskColor), false);
}

/**
 * @tc.name: ApplyHpsBlurTest002
 * @tc.desc: radius is invalid
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest002, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    ASSERT_NE(canvas, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    float radius = -1;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);

    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha1 = 13;
    RSColor maskColor(red, green, blue, alpha1);
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha1, colorFilter, maskColor), false);
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, 0.0, colorFilter, maskColor), false);
}

/**
 * @tc.name: ApplyHpsBlurTest003
 * @tc.desc: radius is max
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest003, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    float radius = 100000;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);
}

/**
 * @tc.name: ApplyHpsBlurTest004
 * @tc.desc: saturationForHPS is invalid
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest004, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = -1.1;
    float brightnessForHPS = 1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);
}

/**
 * @tc.name: ApplyHpsBlurTest005
 * @tc.desc: saturationForHPS  is max
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest005, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = 100000;
    float brightnessForHPS = 1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);
}

/**
 * @tc.name: ApplyHpsBlurTest006
 * @tc.desc: brightnessForHPS is invalid
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest006, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = 1.1;
    float brightnessForHPS = -1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);
}

/**
 * @tc.name: ApplyHpsBlurTest007
 * @tc.desc: brightnessForHPS is max
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest007, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 100000;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 0.8;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);
}

/**
 * @tc.name: ApplyHpsBlurTest008
 * @tc.desc: alpha is invalid
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest008, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = -1;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);
}

/**
 * @tc.name: ApplyHpsBlurTest009
 * @tc.desc: alpha is max
 * @tc.type:FUNC
 */
HWTEST_F(RSHpsBlurTest, ApplyHpsBlurTest009, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());

    auto image = std::make_shared<Drawing::Image>();
    float radius = 10;
    float saturationForHPS = 1.1;
    float brightnessForHPS = 1.0;
    auto param = Drawing::HpsBlurParameter(src_, dst_, radius, saturationForHPS, brightnessForHPS);
    float alpha = 100000;
    Drawing::Brush brush;
    auto colorFilter = brush.GetFilter().GetColorFilter();

    HpsBlurFilter filter;
    EXPECT_EQ(filter.ApplyHpsBlur(*canvas, image, param, alpha, colorFilter), false);
}
} // namespace Rosen
} // namespace OHOS