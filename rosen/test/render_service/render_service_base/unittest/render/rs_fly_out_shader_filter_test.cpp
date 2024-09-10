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

class RSFlyOutShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFlyOutShaderFilterTest::SetUpTestCase() {}
void RSFlyOutShaderFilterTest::TearDownTestCase() {}
void RSFlyOutShaderFilterTest::SetUp() {}
void RSFlyOutShaderFilterTest::TearDown() {}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(RSFlyOutShaderFilterTest, DrawImageRect001, TestSize.Level1)
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
 * @tc.name: DrawImageRect002
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(RSFlyOutShaderFilterTest, DrawImageRect002, TestSize.Level1)
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
 * @tc.name: DrawImageRect003
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(RSFlyOutShaderFilterTest, DrawImageRect003, TestSize.Level1)
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
 * @tc.name: DrawImageRect004
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(RSFlyOutShaderFilterTest, DrawImageRect004, TestSize.Level1)
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
 * @tc.name: GetFunc001
 * @tc.desc: test getFunc
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(RSFlyOutShaderFilterTest, GetFunc001, TestSize.Level1)
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
 * @tc.name: GetBrush001
 * @tc.desc: test getFunc
 * @tc.type: FUNC
 * @tc.require: issueIAH2TY
 */
HWTEST_F(RSFlyOutShaderFilterTest, GetBrush001, TestSize.Level1)
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
