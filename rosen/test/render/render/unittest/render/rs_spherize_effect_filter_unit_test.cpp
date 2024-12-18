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

class RSSpherizeEffectFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpherizeEffectFilterUnitTest::SetUpTestCase() {}
void RSSpherizeEffectFilterUnitTest::TearDownTestCase() {}
void RSSpherizeEffectFilterUnitTest::SetUp() {}
void RSSpherizeEffectFilterUnitTest::TearDown() {}

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
 * @tc.name: TestDrawImageRect01
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterUnitTest, TestDrawImageRect01, TestSize.Level1)
{
    RSSpherizeEffectFilter testFilter(0.f);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src;
    Drawing::Rect dst;
    testFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);

    testFilter.spherizeDegree_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    testFilter.DrawImageRect(canvas, image, src, dst);
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
    testFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: TesGetDescriptiont01
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterUnitTest, TesGetDescriptiont01, TestSize.Level1)
{
    RSSpherizeEffectFilter testFilter(0.f);
    std::string expectRes = "RSSpherizeEffectFilter " + std::to_string(0.f);
    EXPECT_EQ(testFilter.GetDescription(), expectRes);
}

/**
 * @tc.name: TestGetSpherizeDegree01
 * @tc.desc: Verify function GetSpherizeDegree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterUnitTest, TestGetSpherizeDegree01, TestSize.Level1)
{
    RSSpherizeEffectFilter testFilter(0.f);
    EXPECT_EQ(testFilter.GetSpherizeDegree(), 0.f);
}

/**
 * @tc.name: TestIsValid01
 * @tc.desc: Verify function IsValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterUnitTest, TestIsValid01, TestSize.Level1)
{
    RSSpherizeEffectFilter testFilter(0.f);
    EXPECT_FALSE(testFilter.IsValid());
}

/**
 * @tc.name: TestDrawImageRect02
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpherizeEffectFilterUnitTest, TestDrawImageRect02, TestSize.Level2)
{
    RSSpherizeEffectFilter testFilter(0.f);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src;
    Drawing::Rect dst;

    testFilter.spherizeDegree_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image);

    image = CreateDrawingImage(2, 1);
    EXPECT_TRUE(image->GetWidth() > image->GetHeight());
    testFilter.DrawImageRect(canvas, image, src, dst);
}
} // namespace Rosen
} // namespace OHOS
