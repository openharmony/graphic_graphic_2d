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

class RSForegroundEffectFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSForegroundEffectFilterRenderTest::SetUpTestCase() {}
void RSForegroundEffectFilterRenderTest::TearDownTestCase() {}
void RSForegroundEffectFilterRenderTest::SetUp() {}
void RSForegroundEffectFilterRenderTest::TearDown() {}

/**
 * @tc.name: TestMakeForegroundEffect
 * @tc.desc: Verify function MakeForegroundEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestMakeForegroundEffect, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    auto blurBuilder = rsForegroundEffectFilter->MakeForegroundEffect();
    EXPECT_NE(blurBuilder, nullptr);
}

/**
 * @tc.name: TestMakeForegroundEffect01
 * @tc.desc: radius is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestMakeForegroundEffect01, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(-1.0f);
    auto blurBuilder = rsForegroundEffectFilter->MakeForegroundEffect();
    EXPECT_NE(blurBuilder, nullptr);
}

/**
 * @tc.name: TestMakeForegroundEffect02
 * @tc.desc: radius is max
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestMakeForegroundEffect02, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1000000.0f);
    auto blurBuilder = rsForegroundEffectFilter->MakeForegroundEffect();
    EXPECT_NE(blurBuilder, nullptr);
}

/**
 * @tc.name: TestAdjustRadiusAndScale
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestAdjustRadiusAndScale, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    rsForegroundEffectFilter->blurRadius_ = 401;
    rsForegroundEffectFilter->MakeForegroundEffect();
    rsForegroundEffectFilter->blurRadius_ = 151;
    rsForegroundEffectFilter->MakeForegroundEffect();
    rsForegroundEffectFilter->blurRadius_ = 51;
    rsForegroundEffectFilter->MakeForegroundEffect();
    rsForegroundEffectFilter->blurRadius_ = 50;
    rsForegroundEffectFilter->MakeForegroundEffect();
    EXPECT_EQ(rsForegroundEffectFilter->blurScale_, 0.5f);
}

/**
 * @tc.name: TestComputePassesAndUnit
 * @tc.desc: Verify function ComputePassesAndUnit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestComputePassesAndUnit, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    rsForegroundEffectFilter->blurRadius_ = 0.0f;
    rsForegroundEffectFilter->ComputePassesAndUnit();
    EXPECT_EQ(rsForegroundEffectFilter->numberOfPasses_, 1);
}

/**
 * @tc.name: TestGetDirtyExtension
 * @tc.desc: Verify function GetDirtyExtension
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestGetDirtyExtension, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    rsForegroundEffectFilter->blurScale_ = 0.0f;
    EXPECT_EQ(rsForegroundEffectFilter->GetDirtyExtension(), 0.0f);
    rsForegroundEffectFilter->blurScale_ = 1.0f;
    EXPECT_EQ(rsForegroundEffectFilter->GetDirtyExtension(), 8.0f);
}

/**
 * @tc.name: TestApplyForegroundEffect
 * @tc.desc: Verify function ApplyForegroundEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 1);
}

/**
 * @tc.name: TestApplyForegroundEffect01
 * @tc.desc: radius is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect01, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(-1.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 1);
}

/**
 * @tc.name: TestApplyForegroundEffect02
 * @tc.desc: radius is max
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect02, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1000000.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 1);
}

/**
 * @tc.name: TestApplyForegroundEffect03
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect03, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}


/**
 * @tc.name: TestApplyForegroundEffect104
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_RGB_565
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect104, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect04
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_RGB_565
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect04, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect05
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_RGBA_8888
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect05, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect06
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_BGRA_8888
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect06, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect07
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_RGBA_F16
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect07, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect08
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_N32
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect08, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect09
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_RGBA_1010102
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect09, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect010
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_GRAY_8
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect010, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 1);
}

/**
 * @tc.name: TestApplyForegroundEffect011
 * @tc.desc: ImageInfo  ColorType is COLORTYPE_RGB_888X
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect011, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}


/**
 * @tc.name: TestApplyForegroundEffect012
 * @tc.desc: ImageInfo  AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect012, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect013
 * @tc.desc: ImageInfo  AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect013, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestApplyForegroundEffect014
 * @tc.desc: ImageInfo  AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestApplyForegroundEffect014, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(10.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: TestGetDescription
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestGetDescription, TestSize.Level1)
{
    float blurRadius = 401.f;
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(blurRadius);
    std::string expectRes = "ForegroundEffect radius: " + std::to_string(blurRadius) +
        ", scale: " + std::to_string(rsForegroundEffectFilter->blurScale_) + ", passNum: " +
        std::to_string(rsForegroundEffectFilter->numberOfPasses_) + ", dirtyExtension: " +
        std::to_string(rsForegroundEffectFilter->GetDirtyExtension());
    EXPECT_NE(rsForegroundEffectFilter->blurRadius_, blurRadius);

    blurRadius = 151.f;
    auto otherFilter = std::make_shared<RSForegroundEffectFilter>(blurRadius);
    EXPECT_NE(otherFilter->GetDescription(), expectRes);
}

/**
 * @tc.name: TestIsValid
 * @tc.desc: Verify function IsValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestIsValid, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    std::string expectRes;
    EXPECT_TRUE(rsForegroundEffectFilter->IsValid());
}

/**
 * @tc.name: TestDrawImageRect
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestDrawImageRect, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    rsForegroundEffectFilter->DrawImageRect(canvas, image, src, dst);
    EXPECT_NE(rsForegroundEffectFilter->blurEffect_, nullptr);
}

/**
 * @tc.name: TestMakeImage
 * @tc.desc: Verify function MakeImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSForegroundEffectFilterRenderTest, TestMakeImage, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f); // blur radius
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    auto blurBuilder = rsForegroundEffectFilter->MakeForegroundEffect();
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;
    blurBuilder->SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::DECAL,
        Drawing::TileMode::DECAL, sampling, matrix));
    blurBuilder->SetUniform("in_blurOffset", 2.f, 2.f); // offsetX and offsetY
    std::shared_ptr<Drawing::Surface> surface = std::make_shared<Drawing::Surface>();
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat); // width and height
    ASSERT_TRUE(surface->Bind(bitmap));
    auto outputImage = rsForegroundEffectFilter->MakeImage(surface, &matrix, blurBuilder);

    ASSERT_TRUE(outputImage != nullptr);
}
} // namespace Rosen
} // namespace OHOS