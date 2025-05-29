/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

class RSSpherizeEffectFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpherizeEffectFilterRenderTest::SetUpTestCase() {}
void RSSpherizeEffectFilterRenderTest::TearDownTestCase() {}
void RSSpherizeEffectFilterRenderTest::SetUp() {}
void RSSpherizeEffectFilterRenderTest::TearDown() {}

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
 * @tc.name: TestDrawImageRect001
 * @tc.desc: test results of TestDrawImageRect001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterRenderTest, TestDrawImageRect001, TestSize.Level1)
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
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: TestGetDescription
 * @tc.desc: test results of TestGetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterRenderTest, TestGetDescription, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    std::string expectRes = "RSSpherizeEffectFilter " + std::to_string(0.f);
    EXPECT_EQ(effectFilter.GetDescription(), expectRes);
}

/**
 * @tc.name: TestGetSpherizeDegree
 * @tc.desc: test results of TestGetSpherizeDegree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterRenderTest, TestGetSpherizeDegree, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    EXPECT_EQ(effectFilter.GetSpherizeDegree(), 0.f);
}

/**
 * @tc.name: TestIsValid
 * @tc.desc: test results of TestIsValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterRenderTest, TestIsValid, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(0.f);
    EXPECT_FALSE(effectFilter.IsValid());
}

/**
 * @tc.name: TestIsValid001
 * @tc.desc: test results of TestIsValid001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterRenderTest, TestIsValid001, TestSize.Level1)
{
    RSSpherizeEffectFilter effectFilter(1.f);
    EXPECT_TRUE(effectFilter.IsValid());
}

/**
 * @tc.name: TestDrawImageRect_002
 * @tc.desc: Verify function TestDrawImageRect_002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterRenderTest, TestDrawImageRect_002, TestSize.Level2)
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
