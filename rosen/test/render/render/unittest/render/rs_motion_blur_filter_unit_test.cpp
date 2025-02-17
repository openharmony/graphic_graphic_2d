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

class MotionBlurFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MotionBlurFilterUnitTest::SetUpTestCase() {}
void MotionBlurFilterUnitTest::TearDownTestCase() {}
void MotionBlurFilterUnitTest::SetUp() {}
void MotionBlurFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestInterface
 * @tc.desc: Verify function Interface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestInterface01, TestSize.Level1)
{
    Vector2f anchor = {0.f, 0.f};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(10.f, anchor);
    auto testFilter = std::make_shared<RSMotionBlurFilter>(para);
    EXPECT_TRUE(testFilter != nullptr);

    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image;
    testFilter->GetDescription();
    testFilter->DrawImageRect(canvas, image, src, dst);
}

/**
 * @tc.name: TestCompose01
 * @tc.desc: Verify function Compose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestCompose01, TestSize.Level1)
{
    Vector2f anchor = {0.f, 0.f};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(10.f, anchor); // 10.f radius
    auto testFilter = std::make_shared<RSMotionBlurFilter>(para);
    auto testFilter2 = std::make_shared<RSMotionBlurFilter>(para);

    EXPECT_TRUE(testFilter->Compose(testFilter2) == nullptr);
}

/**
 * @tc.name: TestDisableMotionBlur01
 * @tc.desc: Verify function DisableMotionBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestDisableMotionBlur01, TestSize.Level1)
{
    Vector2f anchor = {0.f, 0.f};
    std::shared_ptr<MotionBlurParam> para = std::make_shared<MotionBlurParam>(10.f, anchor); // 10.f radius
    auto testFilter = std::make_shared<RSMotionBlurFilter>(para);
    ASSERT_EQ(testFilter->motionBlurPara_, para);
    ASSERT_EQ(testFilter->type_, RSDrawingFilterOriginal::FilterType::MOTION_BLUR);

    bool isDisableMotionBlur = true;
    testFilter->DisableMotionBlur(isDisableMotionBlur);
    ASSERT_TRUE(testFilter->disableMotionBlur_);
}

/**
 * @tc.name: TestDrawMotionBlur01
 * @tc.desc: Verify function DrawMotionBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestDrawMotionBlur01, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    std::shared_ptr<MotionBlurParam> para = nullptr;
    RSMotionBlurFilter motionBlurFilter(para);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    testFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(image != nullptr);

    para = std::make_shared<MotionBlurParam>(1.f, s);
    testFilter.motionBlurPara_ = para;
    testFilter.DrawMotionBlur(canvas, image, src, dst);
    EXPECT_TRUE(testFilter.motionBlurPara_ != nullptr);
}

/**
 * @tc.name: TestMakeMotionBlurShader01
 * @tc.desc: Verify function MakeMotionBlurShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestMakeMotionBlurShader01, TestSize.Level1)
{
    auto srcImageShader = std::make_shared<Drawing::ShaderEffect>();
    Vector2f scaleAnchor = { 1.f, 1.f };
    Vector2f scaleSize = { 1.f, 1.f };
    Vector2f rectOffset = { 1.f, 1.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, scaleAnchor);
    RSMotionBlurFilter testFilter(para);
    testFilter.MakeMotionBlurShader(srcImageShader, scaleAnchor, scaleSize, rectOffset, 1.f);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: TestCaculateRect01
 * @tc.desc: Verify function CaculateRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestCaculateRect01, TestSize.Level1)
{
    Vector2f rectOffset = { 1.f, 1.f };
    Vector2f scaleSize = { 1.f, 1.f };
    Vector2f scaleAnchorCoord = { 1.f, 1.f }; // for test
    auto para = std::make_shared<MotionBlurParam>(1.f, rectOffset);
    RSMotionBlurFilter testFilter(para);
    testFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    testFilter.lastRect_.right_ = 2.f;
    testFilter.lastRect_.bottom_ = 2.f; // for test
    testFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);

    testFilter.lastRect_.right_ = 0.f;
    testFilter.lastRect_.bottom_ = 0.f;
    testFilter.curRect_.left_ = 3.f;
    testFilter.curRect_.top_ = 3.f; // for test
    testFilter.CaculateRect(rectOffset, scaleSize, scaleAnchorCoord);
    EXPECT_TRUE(para != nullptr);
}

/**
 * @tc.name: TestRectValid01
 * @tc.desc: Verify function RectValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestRectValid01, TestSize.Level1)
{
    Vector2f rectOffset = { 1.f, 1.f };
    Drawing::Rect rect1;
    Drawing::Rect rect2;
    auto para = std::make_shared<MotionBlurParam>(1.f, rectOffset);
    RSMotionBlurFilter testFilter(para);
    bool res = testFilter.RectValid(rect1, rect2);
    EXPECT_TRUE(res == false);

    rect1.right_ = 3.f;
    rect1.bottom_ = 3.f;
    rect2.right_ = 3.f;
    rect2.bottom_ = 3.f;
    res = testFilter.RectValid(rect1, rect2);
    EXPECT_TRUE(res == false);

    rect2.bottom_ = 4.f;
    res = testFilter.RectValid(rect1, rect2);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: TestOutputOriginalImage01
 * @tc.desc: Verify function OutputOriginalImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MotionBlurFilterUnitTest, TestOutputOriginalImage01, TestSize.Level1)
{
    Vector2f s = { 1.f, 1.f };
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    auto para = std::make_shared<MotionBlurParam>(1.f, s);
    RSMotionBlurFilter testFilter(para);
    testFilter.OutputOriginalImage(canvas, image, src, dst);
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
    testFilter.OutputOriginalImage(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth() != 0);
    EXPECT_TRUE(image->GetHeight() != 0);
}
} // namespace Rosen
} // namespace OHOS