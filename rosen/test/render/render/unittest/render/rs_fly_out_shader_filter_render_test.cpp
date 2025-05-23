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

#include "render/rs_fly_out_shader_filter.h"
#include "draw/color.h"
#include "image/image_info.h"
#include "skia_image.h"
#include "skia_image_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSFlyOutShaderFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFlyOutShaderFilterRenderTest::SetUpTestCase() {}
void RSFlyOutShaderFilterRenderTest::TearDownTestCase() {}
void RSFlyOutShaderFilterRenderTest::SetUp() {}
void RSFlyOutShaderFilterRenderTest::TearDown() {}

/**
 * @tc.name: TestDrawImageRect01
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect01, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    // if image == nullptr
    std::shared_ptr<Drawing::Image> image = nullptr;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);

    effectFilter.degree_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image);
}

/**
 * @tc.name: TestDrawImageRect101
 * @tc.desc: test results of DrawImageRect, src Rect is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect101, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    // if image == nullptr
    std::shared_ptr<Drawing::Image> image = nullptr;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { -1.0f, -1.0f, -2.0f, -2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);

    effectFilter.degree_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image);
}

/**
 * @tc.name: TestDrawImageRect102
 * @tc.desc: test results of DrawImageRect, dst Rect is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect102, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    // if image == nullptr
    std::shared_ptr<Drawing::Image> image = nullptr;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { -1.0f, -1.0f, -2.0f, -2.0f };
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);

    effectFilter.degree_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image);
}

/**
 * @tc.name: TestDrawImageRect103
 * @tc.desc: test results of DrawImageRect, degree_ is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect103, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    // if image == nullptr
    std::shared_ptr<Drawing::Image> image = nullptr;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { -1.0f, -1.0f, -2.0f, -2.0f };
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);

    effectFilter.degree_ = -1.0f;
    image = std::make_shared<Drawing::Image>();
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image);
}

/**
 * @tc.name: TestDrawImageRect02
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect02, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect201
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect201, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2011
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_UNKNOWN,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2011, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2012
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_UNKNOWN,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2012, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2013
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_UNKNOWN,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2013, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect202
 * @tc.desc: test results of DrawImageRect, ColorType is COLORTYPE_RGB_565
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect202, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2021
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_565,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2021, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2022
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_565,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2022, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2023
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_565,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2023, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect203
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_ARGB_4444
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect203, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2031
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_ARGB_4444,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2031, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2032
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_ARGB_4444,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2032, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2033
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_ARGB_4444,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2033, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect204
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_8888
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect204, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2041
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_8888,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2041, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2042
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_8888,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2042, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2043
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_8888,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2043, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect205
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_BGRA_8888
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect205, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2051
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_BGRA_8888,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2051, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2052
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_BGRA_8888,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2052, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2053
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_BGRA_8888,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2053, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect206
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect206, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2061
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2061, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2062
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2062, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2063
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2063, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect207
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect207, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2071
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2071, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2072
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2072, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2073
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2073, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect208
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect208, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2081
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2081, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2082
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2082, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2083
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2083, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect209
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect209, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2091
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2091, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2092
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2092, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2093
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2093, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect210
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect210, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2101
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X,AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2101, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2102
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X,AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2102, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect2103
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X,AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect2103, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::Rect dst{1.0f, 1.0f, 2.0f, 2.0f};
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int *addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());

    // if FLY_IN_MODE
    effectFilter.flyMode_ = 1;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth());
    EXPECT_FALSE(image->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect03
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect03, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect301
 * @tc.desc: test results of DrawImageRect, ColorType is COLORTYPE_UNKNOWN,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect301, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3011
 * @tc.desc: test results of DrawImageRect, ColorType is COLORTYPE_UNKNOWN,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3011, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3012
 * @tc.desc: test results of DrawImageRect, ColorType is ALPHATYPE_PREMUL,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3012, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3013
 * @tc.desc: test results of DrawImageRect, ColorType is ALPHATYPE_UNPREMUL,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3013, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect302
 * @tc.desc: test results of DrawImageRect, ColorType is COLORTYPE_RGB_565,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect302, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3021
 * @tc.desc: test results of DrawImageRect, ColorType is COLORTYPE_RGB_565,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3021, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3022
 * @tc.desc: test results of DrawImageRect, ColorType is ALPHATYPE_PREMUL,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3022, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3023
 * @tc.desc: test results of DrawImageRect, ColorType is ALPHATYPE_UNPREMUL,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3023, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect303
 * @tc.desc: test results of DrawImageRect, ColorType is COLORTYPE_ARGB_4444,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect303, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3031
 * @tc.desc: test results of DrawImageRect, ColorType is COLORTYPE_RGB_565,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3031, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3032
 * @tc.desc: test results of DrawImageRect, ColorType is ALPHATYPE_PREMUL,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3032, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect3033
 * @tc.desc: test results of DrawImageRect, ColorType is ALPHATYPE_UNPREMUL,
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect3033, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetWidth() <= 0
    Drawing::ImageInfo imageInfo(0, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect004
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect004, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect406
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16, AlphaType is ALPHATYPE_OPAQUE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect406, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4061
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16, AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4061, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4062
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16, AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4062, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4063
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_F16, AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4063, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect407
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32, AlphaType is ALPHATYPE_OPAQUE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect407, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4071
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32, AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4071, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4072
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32, AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4072, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4073
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_N32, AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4073, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect408
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102, AlphaType is ALPHATYPE_OPAQUE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect408, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(
        1, 0, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4081
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102, AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4081, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(
        1, 0, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4082
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102, AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4082, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(
        1, 0, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4083
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGBA_1010102, AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4083, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(
        1, 0, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect409
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8, AlphaType is ALPHATYPE_OPAQUE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect409, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4091
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8, AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4091, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4092
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8, AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4092, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4093
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_GRAY_8, AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4093, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect410
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X, AlphaType is ALPHATYPE_OPAQUE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect410, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4101
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X, AlphaType is ALPHATYPE_UNKNOWN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4101, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4102
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X, AlphaType is ALPHATYPE_PREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4102, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect4103
 * @tc.desc: test results of DrawImageRect,ColorType is COLORTYPE_RGB_888X, AlphaType is ALPHATYPE_UNPREMUL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestDrawImageRect4103, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    image = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    // if image->GetHeight() <= 0
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() == 0);
    EXPECT_TRUE(image->GetHeight() == 0);
}

/**
 * @tc.name: TestGetFunc01
 * @tc.desc: test getFunc
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestGetFunc01, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.5f, 1);
    uint32_t flyMode = effectFilter.GetFlyMode();
    EXPECT_TRUE(flyMode == 1);

    float degree = effectFilter.GetDegree();
    EXPECT_FLOAT_EQ(degree, 0.5f);

    std::string description = effectFilter.GetDescription();
    std::string result = "RSFlyOutShaderFilter " + std::to_string(effectFilter.degree_);
    EXPECT_TRUE(description == result);
}

/**
 * @tc.name: TestGetBrush01
 * @tc.desc: test getFunc
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterRenderTest, TestGetBrush01, TestSize.Level1)
{
    RSFlyOutShaderFilter effectFilter(0.f, 0);
    // if image == nullptr
    std::shared_ptr<Drawing::Image> image = nullptr;
    auto brush = effectFilter.GetBrush(image);
    EXPECT_FALSE(brush.GetShaderEffect());

    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    brush = effectFilter.GetBrush(image);
    EXPECT_TRUE(brush.GetShaderEffect());
}
} // namespace Rosen
} // namespace OHOS
