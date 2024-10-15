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

#include "render/rs_distortion_shader_filter.h"
#include "draw/color.h"
#include "image/image_info.h"
#include "skia_image.h"
#include "skia_image_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSDistortionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDistortionFilterTest::SetUpTestCase() {}
void RSDistortionFilterTest::TearDownTestCase() {}
void RSDistortionFilterTest::SetUp() {}
void RSDistortionFilterTest::TearDown() {}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, DrawImageRect001, TestSize.Level1)
{
    RSDistortionFilter effectFilter(0.5f);
    Drawing::Canvas canvas;
    // if image == nullptr
    std::shared_ptr<Drawing::Image> image = nullptr;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);
}

/**
 * @tc.name: DrawImageRect002
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, DrawImageRect002, TestSize.Level1)
{
    // pincushion distortion
    RSDistortionFilter effectFilter(0.05f);
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

    // barrel distortion
    effectFilter.distortionK_ = -0.05f;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());

    // IsValid = false
    effectFilter.distortionK_ = -1.5f;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: DrawImageRect003
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, DrawImageRect003, TestSize.Level1)
{
    RSDistortionFilter effectFilter(0.f);
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
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, DrawImageRect004, TestSize.Level1)
{
    RSDistortionFilter effectFilter(0.f);
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
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, GetFunc001, TestSize.Level1)
{
    RSDistortionFilter effectFilter(0.5f);
    std::string description = effectFilter.GetDescription();
    std::string result = "DistortionEffect k: " + std::to_string(effectFilter.distortionK_) +
        ", scaleCoord: " + std::to_string(effectFilter.scaleCoord_) +
        ", offsetX: " + std::to_string(effectFilter.offsetX_) + ", offsetY: " + std::to_string(effectFilter.offsetY_);
    EXPECT_TRUE(description == result);
}

/**
 * @tc.name: MakeDistortionShader001
 * @tc.desc: test MakeDistortionShader
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, MakeDistortionShader001, TestSize.Level1)
{
    RSDistortionFilter effectFilter(0.7f);
    // if distortEffect == nullptr
    auto distortBuilder1 = effectFilter.MakeDistortionShader();
    EXPECT_TRUE(distortBuilder1);
    // if distortEffect != nullptr because distortEffect is static
    auto distortBuilder2 = effectFilter.MakeDistortionShader();
    EXPECT_TRUE(distortBuilder2);
}

/**
 * @tc.name: IsValid001
 * @tc.desc: test IsValid
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, IsValid001, TestSize.Level1)
{
    // if distortionK > 1.0f
    RSDistortionFilter effectFilter1(1.5f);
    bool isValid1 = effectFilter1.IsValid();
    EXPECT_FALSE(isValid1);

    // if distortionK < -1.0f
    RSDistortionFilter effectFilter2(-1.5f);
    bool isValid2 = effectFilter2.IsValid();
    EXPECT_FALSE(isValid2);

    // if distortionK = -1.0f
    RSDistortionFilter effectFilter3(-1.0f);
    bool isValid3 = effectFilter3.IsValid();
    EXPECT_TRUE(isValid3);

    // if distortionK = 1.0f
    RSDistortionFilter effectFilter4(1.0f);
    bool isValid4 = effectFilter4.IsValid();
    EXPECT_TRUE(isValid4);
}

/**
 * @tc.name: IsValid002
 * @tc.desc: test IsValid
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSDistortionFilterTest, IsValid002, TestSize.Level1)
{
    RSDistortionFilter effectFilter(-1.0f);
    // if scaleCoord_ = 1.0f
    effectFilter.scaleCoord_ = 1.0f;
    bool isValid1 = effectFilter.IsValid();
    EXPECT_TRUE(isValid1);

    // if scaleCoord_ = 4.0f
    effectFilter.scaleCoord_ = 4.0f;
    bool isValid2 = effectFilter.IsValid();
    EXPECT_FALSE(isValid2);

    // if scaleCoord_ = 0.0f
    effectFilter.scaleCoord_ = 0.0f;
    bool isValid3 = effectFilter.IsValid();
    EXPECT_FALSE(isValid3);
}
} // namespace Rosen
} // namespace OHOS
