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
class RSHpsBlurRenderTest : public testing::Test {
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

void RSHpsBlurRenderTest::SetUpTestCase() {}
void RSHpsBlurRenderTest::TearDownTestCase() {}
void RSHpsBlurRenderTest::SetUp()
{
    canvas_.Restore();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void RSHpsBlurRenderTest::TearDown() {}

/**
 * @tc.name: GetShaderTransformTest
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, GetShaderTransformTest, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, GetMixEffectTest, TestSize.Level2)
{
    auto mixEffect = HpsBlurFilter::GetHpsBlurFilter().GetMixEffect();
    EXPECT_NE(mixEffect, nullptr);
}

/**
 * @tc.name: SetShaderEffectTest
 * @tc.desc: Verify function SetShaderEffect
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTileXIsREPEATTest, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTileYIsREPEATTest, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTileXIsMIRRORTest, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTileYIsMIRRORTest, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTileXIsDECALTest, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTileYIsCLAMPTest, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest01TileXIsCLAMP
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest01TileXIsCLAMP, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest01TileXIsDECAL
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest01TileXIsDECAL, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest01TileYIsMIRROR
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest01TileYIsMIRROR, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest01TileYIsCLAMP
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest01TileYIsCLAMP, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest01TileYIsREPEAT
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest01TileYIsREPEAT, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest01TileYIsCLAMP
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest05TileYIsCLAMP, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest02
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest02, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest03
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest03, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest04
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest04, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest05
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest05, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest06
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest06, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest07
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest07, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest08
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest08, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest09
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest09, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest010, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest011, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest012, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest013, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest014, TestSize.Level2)
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
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest015, TestSize.Level2)
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
 * @tc.name: SetShaderEffectTest01
 * @tc.desc: FilterMode is NEAREST
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, SetShaderEffectTest01, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest01
 * @tc.desc: Verify function ApplyHpsBlur
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest01, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest02
 * @tc.desc: radius is invalid
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest02, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest03
 * @tc.desc: radius is max
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest03, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest04
 * @tc.desc: saturationForHPS is invalid
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest04, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest05
 * @tc.desc: saturationForHPS  is max
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest05, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest06
 * @tc.desc: brightnessForHPS is invalid
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest06, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest07
 * @tc.desc: brightnessForHPS is max
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest07, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest08
 * @tc.desc: alpha is invalid
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest08, TestSize.Level2)
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
 * @tc.name: ApplyHpsBlurTest09
 * @tc.desc: alpha is max
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplyHpsBlurTest09, TestSize.Level2)
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

/**
 * @tc.name: ApplymaskColorFilterTest01
 * @tc.desc: Verify function ApplymaskColorFilter
 * @tc.type:FUNC
 * @tc.require: issuesICH80N
 */
HWTEST_F(RSHpsBlurRenderTest, ApplymaskColorFilterTest01, TestSize.Level2)
{
    Drawing::Canvas canvas;
    HpsBlurFilter filter;
    float alpha1 = 0.0f;
    RSColor maskColor1 = RSColor();
    float newAlpha1 = filter.ApplyMaskColorFilter(canvas, alpha1, maskColor1);
    EXPECT_EQ(alpha1, newAlpha1);

    float alpha2 = 0.0f;
    RSColor maskColor2 = RSColor(255, 0, 0, 255); // maskColor1 r 255, g 0, b 0, a 255
    float newAlpha2 = filter.ApplyMaskColorFilter(canvas, alpha2, maskColor2);
    EXPECT_NE(alpha2, newAlpha2);

    float alpha3 = 0.0f;
    RSColor maskColor3 = RSColor(255, 255, 255, 0); // maskColor1 r 0, g 255, b 255, a 255
    float newAlpha3 = filter.ApplyMaskColorFilter(canvas, alpha3, maskColor3);
    EXPECT_EQ(alpha3, newAlpha3);

    float alpha4 = 0.5f;
    RSColor maskColor4 = RSColor(255, 255, 0, 255); // maskColor1 r 255, g 255, b 0, a 255
    float newAlpha4 = filter.ApplyMaskColorFilter(canvas, alpha4, maskColor4);
    EXPECT_NE(alpha4, newAlpha4);

    float alpha5 = 1.0f;
    RSColor maskColor5 = RSColor(255, 255, 255, 255); // maskColor1 r 255, g 255, b 255, a 255
    float newAlpha5 = filter.ApplyMaskColorFilter(canvas, alpha5, maskColor5);
    EXPECT_EQ(alpha5, newAlpha5);
}
} // namespace Rosen
} // namespace OHOS