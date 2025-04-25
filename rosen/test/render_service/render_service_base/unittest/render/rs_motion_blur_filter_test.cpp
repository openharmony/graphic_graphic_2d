/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "render/rs_motion_blur_filter.h"
#include "draw/color.h"
#include "image/image_info.h"
#include "skia_image.h"
#include "skia_image_info.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class MotionBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MotionBlurFilterTest::SetUpTestCase() {}
void MotionBlurFilterTest::TearDownTestCase() {}
void MotionBlurFilterTest::SetUp() {}
void MotionBlurFilterTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(MotionBlurFilterTest, testInterface, TestSize.Level1)
{
    Vector2f anchor = {0.f, 0.f};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(10.f, anchor);
    auto filter = std::make_shared<RSMotionBlurFilter>(para);
    EXPECT_TRUE(filter != nullptr);

    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image;
    filter->GetDescription();
    filter->DrawImageRect(canvas, image, src, dst);
}

/**
 * @tc.name: ComposeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(MotionBlurFilterTest, ComposeTest, TestSize.Level1)
{
    Vector2f anchor = {0.f, 0.f};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(10.f, anchor); // 10.f radius
    auto filter = std::make_shared<RSMotionBlurFilter>(para);
    auto filter_ = std::make_shared<RSMotionBlurFilter>(para);

    EXPECT_TRUE(filter->Compose(filter_) == nullptr);
}

/**
 * @tc.name: DisableMotionBlurTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(MotionBlurFilterTest, DisableMotionBlurTest, TestSize.Level1)
{
    Vector2f anchor = {0.f, 0.f};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(10.f, anchor); // 10.f radius
    auto filter = std::make_shared<RSMotionBlurFilter>(para);
    ASSERT_EQ(filter->motionBlurPara_, para);
    ASSERT_EQ(filter->type_, RSDrawingFilterOriginal::FilterType::MOTION_BLUR);

    bool isDisableMotionBlur = true;
    filter->DisableMotionBlur(isDisableMotionBlur);
    ASSERT_TRUE(filter->disableMotionBlur_);
}

/**
 * @tc.name: DrawMotionBlur001
 * @tc.desc: test results of DrawMotionBlur
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, DrawMotionBlur001, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    std::shared_ptr<MotionBlurParam> para = nullptr;
    RSMotionBlurFilter motionBlurFilter(para);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    motionBlurFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

    para = std::make_shared<MotionBlurParam>(1.f, s);
    motionBlurFilter.motionBlurPara_ = para;
    motionBlurFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(motionBlurFilter.motionBlurPara_ != nullptr);
}

/**
 * @tc.name: DrawMotionBlur002
 * @tc.desc: test results of DrawMotionBlur
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, DrawMotionBlur002, TestSize.Level1)
{
    Vector2f s = { 2.f, 2.f };
    std::shared_ptr<MotionBlurParam> para = nullptr;
    RSMotionBlurFilter motionBlurFilter(para);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    motionBlurFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

    para = std::make_shared<MotionBlurParam>(2.f, s);
    motionBlurFilter.motionBlurPara_ = para;
    motionBlurFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(motionBlurFilter.motionBlurPara_ != nullptr);
}

/**
 * @tc.name: DrawMotionBlur003
 * @tc.desc: test results of DrawMotionBlur
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, DrawMotionBlur003, TestSize.Level1)
{
    Vector2f s = { 0.f, 0.f };
    std::shared_ptr<MotionBlurParam> para = nullptr;
    RSMotionBlurFilter motionBlurFilter(para);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    motionBlurFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

    para = std::make_shared<MotionBlurParam>(0.f, s);
    motionBlurFilter.motionBlurPara_ = para;
    motionBlurFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(motionBlurFilter.motionBlurPara_ != nullptr);
}

/**
 * @tc.name: MakeMotionBlurShader001
 * @tc.desc: test results of MakeMotionBlurShader
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, MakeMotionBlurShader001, TestSize.Level1)
{
    auto srcImageShader = std::make_shared<Drawing::ShaderEffect>();
    Vector2f scaleAnchor = { 1.f, 1.f };
    Vector2f scaleSize = { 1.f, 1.f };
    Vector2f rectOffset = { 1.f, 1.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.MakeMotionBlurShader(srcImageShader, scaleAnchor, scaleSize, rectOffset, 1.f);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: MakeMotionBlurShader002
 * @tc.desc: test results of MakeMotionBlurShader
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, MakeMotionBlurShader002, TestSize.Level1)
{
    auto srcImageShader = std::make_shared<Drawing::ShaderEffect>();
    Vector2f scaleAnchor = { 2.f, 2.f };
    Vector2f scaleSize = { 1.f, 1.f };
    Vector2f rectOffset = { 1.f, 1.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.MakeMotionBlurShader(srcImageShader, scaleAnchor, scaleSize, rectOffset, 1.f);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: MakeMotionBlurShader003
 * @tc.desc: test results of MakeMotionBlurShader
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, MakeMotionBlurShader003, TestSize.Level1)
{
    auto srcImageShader = std::make_shared<Drawing::ShaderEffect>();
    Vector2f scaleAnchor = { 2.f, 2.f };
    Vector2f scaleSize = { 3.f, 3.f };
    Vector2f rectOffset = { 1.f, 1.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.MakeMotionBlurShader(srcImageShader, scaleAnchor, scaleSize, rectOffset, 1.f);
    EXPECT_TRUE(para != nullptr);
}


/**
 * @tc.name: MakeMotionBlurShader004
 * @tc.desc: test results of MakeMotionBlurShader
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, MakeMotionBlurShader004, TestSize.Level1)
{
    auto srcImageShader = std::make_shared<Drawing::ShaderEffect>();
    Vector2f scaleAnchor = { 2.f, 2.f };
    Vector2f scaleSize = { 3.f, 3.f };
    Vector2f rectOffset = { 10.f, 10.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.MakeMotionBlurShader(srcImageShader, scaleAnchor, scaleSize, rectOffset, 1.f);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: CaculateRect001
 * @tc.desc: test results of CaculateRect
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, CaculateRect001, TestSize.Level1)
{
    Vector2f rectOffset = { 1.f, 1.f };
    Vector2f scaleSize = { 1.f, 1.f };
    Vector2f scaleAnchorCoord = { 1.f, 1.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, rectOffset);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    motionBlurFilter.lastRect_.right_ = 2.f;
    motionBlurFilter.lastRect_.bottom_ = 2.f; // for test
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    motionBlurFilter.lastRect_.right_ = 0.f;
    motionBlurFilter.lastRect_.bottom_ = 0.f;
    motionBlurFilter.curRect_.left_ = 3.f;
    motionBlurFilter.curRect_.top_ = 3.f; // for test
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: CaculateRect002
 * @tc.desc: test results of CaculateRect
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, CaculateRect002, TestSize.Level1)
{
    Vector2f rectOffset = { 2.f, 2.f };
    Vector2f scaleSize = { 2.f, 2.f };
    Vector2f scaleAnchorCoord = { 2.f, 2.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, rectOffset);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    motionBlurFilter.lastRect_.right_ = 2.f;
    motionBlurFilter.lastRect_.bottom_ = 2.f; // for test
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    motionBlurFilter.lastRect_.right_ = 0.f;
    motionBlurFilter.lastRect_.bottom_ = 0.f;
    motionBlurFilter.curRect_.left_ = 3.f;
    motionBlurFilter.curRect_.top_ = 3.f; // for test
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: CaculateRect003
 * @tc.desc: test results of CaculateRect
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, CaculateRect003, TestSize.Level1)
{
    Vector2f rectOffset = { 2.f, 2.f };
    Vector2f scaleSize = { 2.f, 2.f };
    Vector2f scaleAnchorCoord = { 2.f, 2.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, rectOffset);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    motionBlurFilter.lastRect_.right_ = 20.f;
    motionBlurFilter.lastRect_.bottom_ = 20.f; // for test
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    motionBlurFilter.lastRect_.right_ = 0.f;
    motionBlurFilter.lastRect_.bottom_ = 0.f;
    motionBlurFilter.curRect_.left_ = 30.f;
    motionBlurFilter.curRect_.top_ = 30.f; // for test
    motionBlurFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: RectValid001
 * @tc.desc: test results of RectValid
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, RectValid001, TestSize.Level1)
{
    Vector2f rectOffset = { 1.f, 1.f };
    Drawing::Rect rect1;
    Drawing::Rect rect2;
    auto para = std::make_shared<MotionBlurParam>(1.f, rectOffset);
    RSMotionBlurFilter motionBlurFilter(para);
    bool res = motionBlurFilter.RectValid(rect1, rect2);
    EXPECT_TRUE(res == false);

    rect1.right_ = 3.f;
    rect1.bottom_ = 3.f;
    rect2.right_ = 3.f;
    rect2.bottom_ = 3.f;
    res = motionBlurFilter.RectValid(rect1, rect2);
    EXPECT_TRUE(res == false);

    rect2.bottom_ = 4.f;
    res = motionBlurFilter.RectValid(rect1, rect2);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: OutputOriginalImage001
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImage001, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() != 0);
    EXPECT_TRUE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_UNKNOWN
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_UNKNOWN, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGB_565
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGB_565, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_ARGB_4444
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_ARGB_4444, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_8888
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_8888, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_BGRA_8888
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_BGRA_8888, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_F16
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_F16, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_N32
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_N32, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_1010102
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_1010102, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_GRAY_8
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_GRAY_8, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() != 0);
    EXPECT_TRUE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGB_888X
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGB_888X, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
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
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_UNKNOWNALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_UNKNOWNALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGB_565ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGB_565ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_ARGB_4444ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_ARGB_4444ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_8888ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_8888ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_BGRA_8888ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_BGRA_8888ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_F16ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_F16ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_N32ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_N32ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_1010102ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_1010102ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_GRAY_8ALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_GRAY_8ALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() != 0);
    EXPECT_TRUE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGB_888XALPHATYPE_PREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGB_888XALPHATYPE_PREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_PREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_UNKNOWNALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_UNKNOWNALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGB_565ALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGB_565ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_ARGB_4444ALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_ARGB_4444ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ARGB_4444, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_8888ALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_8888ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_BGRA_8888ALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_BGRA_8888ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_BGRA_8888, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_F16ALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_F16ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_F16, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_N32ALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_N32ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_N32, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGBA_1010102AALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGBA_1010102ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_RGBA_1010102, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_GRAY_8ALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_GRAY_8ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_GRAY_8, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() != 0);
    EXPECT_TRUE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImageCOLORTYPE_RGB_888XALPHATYPE_UNPREMUL
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImageCOLORTYPE_RGB_888XALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGB_888X, Drawing::AlphaType::ALPHATYPE_UNPREMUL);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImage002
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImage002, TestSize.Level1)
{
    Vector2f s = { 2.f, 2.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(2.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(2, 2, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}

/**
 * @tc.name: OutputOriginalImage003
 * @tc.desc: test results of OutputOriginalImage
 * @tc.type: FUNC
 */
HWTEST_F(MotionBlurFilterTest, OutputOriginalImage003, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(para != nullptr);

    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    motionBlurFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_FALSE(image->GetWidth() != 0);
    EXPECT_FALSE(image->GetHeight() != 0);
}
} // namespace Rosen
} // namespace OHOS