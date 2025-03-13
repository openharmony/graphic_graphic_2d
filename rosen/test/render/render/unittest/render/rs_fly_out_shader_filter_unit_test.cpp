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

class RSFlyOutShaderFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFlyOutShaderFilterUnitTest::SetUpTestCase() {}
void RSFlyOutShaderFilterUnitTest::TearDownTestCase() {}
void RSFlyOutShaderFilterUnitTest::SetUp() {}
void RSFlyOutShaderFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestDrawImageRect01
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterUnitTest, TestDrawImageRect01, TestSize.Level1)
{
    RSFlyOutShaderFilter shaderFilter(0.f, 0);
    Drawing::Canvas testCanvas;
    // if image == nullptr
    std::shared_ptr<Drawing::Image> testImage = nullptr;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rectSrc { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect rectDst { 1.0f, 1.0f, 2.0f, 2.0f };
    shaderFilter.DrawImageRect(testCanvas, testImage, rectSrc, rectDst);
    EXPECT_FALSE(testImage);

    shaderFilter.degree_ = 1.0f;
    testImage = std::make_shared<Drawing::Image>();
    shaderFilter.DrawImageRect(testCanvas, testImage, rectSrc, rectDst);
    EXPECT_TRUE(testImage);
}

/**
 * @tc.name: TestDrawImageRect02
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterUnitTest, TestDrawImageRect02, TestSize.Level1)
{
    // if FLY_OUT_MODE
    RSFlyOutShaderFilter shaderFilter(0.f, 0);
    Drawing::Canvas testCanvas;
    std::shared_ptr<Drawing::Image> testImage;
    testImage = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rectSrc { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect rectDst { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    testImage->imageImplPtr = skiaImage;
    shaderFilter.DrawImageRect(canvtestCanvasas, testImage, rectSrc, rectDst);
    EXPECT_TRUE(testImage->GetWidth());
    EXPECT_TRUE(testImage->GetHeight());

    // if FLY_IN_MODE
    shaderFilter.flyMode_ = 1;
    shaderFilter.DrawImageRect(testCanvas, testImage, rectSrc, rectDst);
    EXPECT_TRUE(testImage->GetWidth());
    EXPECT_TRUE(testImage->GetHeight());
}

/**
 * @tc.name: TestDrawImageRect03
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterUnitTest, TestDrawImageRect03, TestSize.Level1)
{
    RSFlyOutShaderFilter shaderFilter(0.f, 0);
    Drawing::Canvas testCanvas;
    std::shared_ptr<Drawing::Image> testImage;
    testImage = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rectSrc { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect rectDst { 1.0f, 1.0f, 2.0f, 2.0f };
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
    testImage->imageImplPtr = skiaImage;
    shaderFilter.DrawImageRect(testCanvas, testImage, rectSrc, rectDst);
    EXPECT_TRUE(testImage->GetWidth() == 0);
    EXPECT_TRUE(testImage->GetHeight() == 0);
}

/**
 * @tc.name: TestDrawImageRect04
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterUnitTest, TestDrawImageRect04, TestSize.Level1)
{
    RSFlyOutShaderFilter shaderFilter(0.f, 0);
    Drawing::Canvas testCanvas;
    std::shared_ptr<Drawing::Image> testImage;
    testImage = std::make_shared<Drawing::Image>();
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rectSrc { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect rectDst { 1.0f, 1.0f, 2.0f, 2.0f };
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
    testImage->imageImplPtr = skiaImage;
    shaderFilter.DrawImageRect(testCanvas, testImage, rectSrc, rectDst);
    EXPECT_TRUE(testImage->GetWidth() == 0);
    EXPECT_TRUE(testImage->GetHeight() == 0);
}

/**
 * @tc.name: TestGetFunc01
 * @tc.desc: Verify function GetFunc
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterUnitTest, TestGetFunc01, TestSize.Level1)
{
    RSFlyOutShaderFilter shaderFilter(0.5f, 1);
    uint32_t flyModeValue = shaderFilter.GetFlyMode();
    EXPECT_TRUE(flyModeValue == 1);

    float degreeValue = shaderFilter.GetDegree();
    EXPECT_FLOAT_EQ(degreeValue, 0.5f);

    std::string filterDescription = shaderFilter.GetDescription();
    std::string result = "RSFlyOutShaderFilter " + std::to_string(shaderFilter.degree_);
    EXPECT_TRUE(filterDescription == result);
}

/**
 * @tc.name: TestGetBrush01
 * @tc.desc: Verify function GetFunc
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFlyOutShaderFilterUnitTest, TestGetBrush01, TestSize.Level1)
{
    RSFlyOutShaderFilter shaderFilter(0.f, 0);
    // if image == nullptr
    std::shared_ptr<Drawing::Image> testImage = nullptr;
    auto filterBrush = effectFilter.GetBrush(testImage);
    EXPECT_FALSE(filterBrush.GetShaderEffect());

    testImage = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 0, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    testImage->imageImplPtr = skiaImage;
    filterBrush = shaderFilter.GetBrush(testImage);
    EXPECT_TRUE(filterBrush.GetShaderEffect());
}
} // namespace Rosen
} // namespace OHOS
