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

#include "render/rs_spherize_effect_filter.h"
#include "draw/color.h"
#include "draw/canvas.h"
#include "draw/surface.h"
#include "image/image_info.h"
#include "skia_image.h"
#include "skia_image_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSpherizeEffectFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpherizeEffectFilterTest::SetUpTestCase() {}
void RSSpherizeEffectFilterTest::TearDownTestCase() {}
void RSSpherizeEffectFilterTest::SetUp() {}
void RSSpherizeEffectFilterTest::TearDown() {}

static std::shared_ptr<Drawing::Image> CreateDrawingImage(int width, int height)
{
    const Drawing::ImageInfo info =
        Drawing::ImageInfo(width, height, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE);
    auto surface(Drawing::Surface::MakeRaster(info));
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(0, 0, width, height));
    canvas->DetachBrush();
    return surface->GetImageSnapshot();
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSSpherizeEffectFilterTest, DrawImageRect001, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src;
    Drawing::Rect dst;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);

    effectFilter.spherizeDegree_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image);

    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
#ifdef USE_M133_SKIA
    sk_sp<SkImage> skImage = SkImages::RasterFromPixmap(skiaPixmap, rasterReleaseProc, releaseContext);
#else
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
#endif
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: test results of GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSSpherizeEffectFilterTest, GetDescriptionTest, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    std::string expectRes = "RSSpherizeEffectFilter " + std::to_string(0.f);
    EXPECT_EQ(effectFilter.GetDescription(), expectRes);
}

/**
 * @tc.name: GetSpherizeDegreeTest
 * @tc.desc: test results of GetSpherizeDegree
 * @tc.type: FUNC
 */
HWTEST_F(RSSpherizeEffectFilterTest, GetSpherizeDegreeTest, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    EXPECT_EQ(effectFilter.GetSpherizeDegree(), 0.f);
}

/**
 * @tc.name: IsValidTest
 * @tc.desc: test results of IsValid
 * @tc.type: FUNC
 */
HWTEST_F(RSSpherizeEffectFilterTest, IsValidTest, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    EXPECT_FALSE(effectFilter.IsValid());
}

/**
 * @tc.name: IsValidTest001
 * @tc.desc: test results of IsValid
 * @tc.type: FUNC
 */
HWTEST_F(RSSpherizeEffectFilterTest, IsValidTest001, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(1.f);
    EXPECT_TRUE(effectFilter.IsValid());
}

/**
 * @tc.name: DrawImageRect_002
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require:issueIB262Y
 */
HWTEST_F(RSSpherizeEffectFilterTest, DrawImageRect_002, TestSize.Level2)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src;
    Drawing::Rect dst;

    effectFilter.spherizeDegree_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image);

    image = CreateDrawingImage(2, 1);
    EXPECT_TRUE(image->GetWidth() > image->GetHeight());
    effectFilter.DrawImageRect(canvas, image, src, dst);
}
} // namespace Rosen
} // namespace OHOS
