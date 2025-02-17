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
#include "skia_image.h"
#include "skia_image_info.h"

#include "draw/color.h"
#include "draw/surface.h"
#include "image/image_info.h"
#include "render/rs_foreground_effect_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSForegroundEffectFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSForegroundEffectFilterUnitTest::SetUpTestCase() {}
void RSForegroundEffectFilterUnitTest::TearDownTestCase() {}
void RSForegroundEffectFilterUnitTest::SetUp() {}
void RSForegroundEffectFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestMakeForegroundEffect
 * @tc.desc: Verify function MakeForegroundEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestMakeForegroundEffect, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    auto groundEffect = testFilter->MakeForegroundEffect();
    EXPECT_NE(groundEffect, nullptr);
}

/**
 * @tc.name: TestAdjustRadiusAndScale
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestAdjustRadiusAndScale, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    testFilter->blurRadius_ = 401;
    testFilter->MakeForegroundEffect();
    testFilter->blurRadius_ = 151;
    testFilter->MakeForegroundEffect();
    testFilter->blurRadius_ = 51;
    testFilter->MakeForegroundEffect();
    testFilter->blurRadius_ = 50;
    testFilter->MakeForegroundEffect();
    EXPECT_EQ(testFilter->blurScale_, 0.5f);
}

/**
 * @tc.name: TestComputePassesAndUnit
 * @tc.desc: Verify function ComputePassesAndUnit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestComputePassesAndUnit, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    testFilter->blurRadius_ = 0.0f;
    testFilter->ComputePassesAndUnit();
    EXPECT_EQ(testFilter->numberOfPasses_, 1);
}

/**
 * @tc.name: TestGetDirtyExtension
 * @tc.desc: Verify function GetDirtyExtension
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestGetDirtyExtension, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    testFilter->blurScale_ = 0.0f;
    EXPECT_EQ(testFilter->GetDirtyExtension(), 0.0f);
    testFilter->blurScale_ = 1.0f;
    EXPECT_EQ(testFilter->GetDirtyExtension(), 8.0f);
}

/**
 * @tc.name: TestApplyForegroundEffect
 * @tc.desc: Verify function ApplyForegroundEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestApplyForegroundEffect, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    Drawing::Canvas testCanvas;
    testCanvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> testImage;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    testFilter->ApplyForegroundEffect(testCanvas, testImage, param);
    Drawing::Pixmap pixmap;
    testImage = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    testImage->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    testFilter->ApplyForegroundEffect(testCanvas, testImage, param);
    testFilter->blurScale_ = 0;
    testFilter->ApplyForegroundEffect(testCanvas, testImage, param);
    EXPECT_EQ(testImage->GetWidth(), 1);
}

/**
 * @tc.name: TestGetDescription
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestGetDescription, TestSize.Level1)
{
    float blurRadius = 401.f;
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(blurRadius);
    std::string expectResult = "ForegroundEffect radius: " + std::to_string(blurRadius) +
        ", scale: " + std::to_string(testFilter->blurScale_) + ", passNum: " +
        std::to_string(testFilter->numberOfPasses_) + ", dirtyExtension: " +
        std::to_string(testFilter->GetDirtyExtension());
    EXPECT_NE(testFilter->blurRadius_, blurRadius);

    blurRadius = 151.f;
    auto otherFilter = std::make_shared<testFilter>(blurRadius);
    EXPECT_NE(otherFilter->GetDescription(), expectResult);
}

/**
 * @tc.name: TestIsValid
 * @tc.desc: Verify function IsValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestIsValid, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    std::string expectResult;
    EXPECT_TRUE(testFilter->IsValid());
}

/**
 * @tc.name: TestDrawImageRect
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestDrawImageRect, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    std::shared_ptr<Drawing::Image> testImage = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Image> originImage = testImage;
    Drawing::Canvas testCanvas;
    Drawing::Rect rectSrc;
    Drawing::Rect rectDst;
    testFilter->DrawImageRect(testCanvas, testImage, rectSrc, rectDst);
    EXPECT_EQ(testImage, originImage);
}

/**
 * @tc.name: TestMakeImage
 * @tc.desc: Verify function MakeImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterUnitTest, TestMakeImage, TestSize.Level1)
{
    auto testFilter = std::make_shared<RSForegroundEffectFilter>(1.0f); // blur radius
    std::shared_ptr<Drawing::Image> testImage = std::make_shared<Drawing::Image>();
    auto blurBuilder = testFilter->MakeForegroundEffect();
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;
    blurBuilder->SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*testImage, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, sampling, matrix));
    blurBuilder->SetUniform("in_blurOffset", 2.f, 2.f); // offsetX and offsetY
    std::shared_ptr<Drawing::Surface> testSurface = std::make_shared<Drawing::Surface>();
    Drawing::Bitmap testBitmap;
    Drawing::BitmapFormat testBitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    testBitmap.Build(10, 10, testBitmapFormat); // width and height
    ASSERT_TRUE(testSurface->Bind(testBitmap));
    auto outputImage = testFilter->MakeImage(testSurface, &matrix, blurBuilder);

    ASSERT_TRUE(outputImage != nullptr);
}
} // namespace Rosen
} // namespace OHOS