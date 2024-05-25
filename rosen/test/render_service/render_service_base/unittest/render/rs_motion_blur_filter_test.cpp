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
 * @tc.name: SetGeometryTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(MotionBlurFilterTest, SetGeometryTest, TestSize.Level1)
{
    Vector2f anchor = {0.f, 0.f};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(10.f, anchor); // 10.f radius
    auto filter = std::make_shared<RSMotionBlurFilter>(para);

    Drawing::Canvas canvas;
    filter->SetGeometry(canvas, 0.f, 0.f);
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(MotionBlurFilterTest, DrawImageRect001, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter motionBlurFilter(para);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = nullptr;
    Drawing::Rect src;
    Drawing::Rect dst;
    motionBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image == nullptr);

    image = std::make_shared<Drawing::Image>();
    motionBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

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
    motionBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

    motionBlurFilter.motionBlurPara_->radius = 1.f;
    motionBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() != 0);

    motionBlurFilter.lastRect_.right_ = 3.f;
    motionBlurFilter.lastRect_.bottom_ = 3.f;
    motionBlurFilter.curRect_.right_ = 3.f;
    motionBlurFilter.curRect_.bottom_ = 4.f;
    motionBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

    motionBlurFilter.motionBlurPara_ = nullptr;
    motionBlurFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(motionBlurFilter.motionBlurPara_ == nullptr);
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
} // namespace Rosen
} // namespace OHOS