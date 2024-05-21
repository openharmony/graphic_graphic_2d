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
#include "skia_image.h"
#include "skia_image_info.h"

#include "draw/color.h"
#include "image/image_info.h"
#include "render/rs_foreground_effect_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSForegroundEffectFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSForegroundEffectFilterTest::SetUpTestCase() {}
void RSForegroundEffectFilterTest::TearDownTestCase() {}
void RSForegroundEffectFilterTest::SetUp() {}
void RSForegroundEffectFilterTest::TearDown() {}

/**
 * @tc.name: MakeForegroundEffectTest
 * @tc.desc: Verify function MakeForegroundEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSForegroundEffectFilterTest, MakeForegroundEffectTest, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    rsForegroundEffectFilter->MakeForegroundEffect();
    EXPECT_NE(rsForegroundEffectFilter->blurEffect_, nullptr);
}

/**
 * @tc.name: AdjustRadiusAndScaleTest
 * @tc.desc: Verify function AdjustRadiusAndScale
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSForegroundEffectFilterTest, AdjustRadiusAndScaleTest, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    rsForegroundEffectFilter->blurRadius_ = 401;
    rsForegroundEffectFilter->MakeForegroundEffect();
    rsForegroundEffectFilter->blurRadius_ = 151;
    rsForegroundEffectFilter->MakeForegroundEffect();
    rsForegroundEffectFilter->blurRadius_ = 51;
    rsForegroundEffectFilter->MakeForegroundEffect();
    rsForegroundEffectFilter->blurRadius_ = 50;
    rsForegroundEffectFilter->MakeForegroundEffect();
    EXPECT_EQ(rsForegroundEffectFilter->blurScale_, 0.5f);
}

/**
 * @tc.name: ComputePassesAndUnitTest
 * @tc.desc: Verify function ComputePassesAndUnit
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSForegroundEffectFilterTest, ComputePassesAndUnitTest, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    rsForegroundEffectFilter->blurRadius_ = 0.0f;
    rsForegroundEffectFilter->ComputePassesAndUnit();
    EXPECT_EQ(rsForegroundEffectFilter->numberOfPasses_, 1);
}

/**
 * @tc.name: GetDirtyExtensionTest
 * @tc.desc: Verify function GetDirtyExtension
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSForegroundEffectFilterTest, GetDirtyExtensionTest, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    // fot test
    rsForegroundEffectFilter->blurScale_ = 0.0f;
    EXPECT_EQ(rsForegroundEffectFilter->GetDirtyExtension(), 0.0f);
    rsForegroundEffectFilter->blurScale_ = 1.0f;
    EXPECT_EQ(rsForegroundEffectFilter->GetDirtyExtension(), 8.0f);
}

/**
 * @tc.name: ApplyForegroundEffectTest
 * @tc.desc: Verify function ApplyForegroundEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSForegroundEffectFilterTest, ApplyForegroundEffectTest, TestSize.Level1)
{
    auto rsForegroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(1.0f);
    Drawing::Canvas canvas;
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src1;
    Drawing::Rect dst2;
    ForegroundEffectParam param(src1, dst2);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    Drawing::Pixmap pixmap;
    image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>(skImage);
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    rsForegroundEffectFilter->blurScale_ = 0;
    rsForegroundEffectFilter->ApplyForegroundEffect(canvas, image, param);
    EXPECT_EQ(image->GetWidth(), 1);
}
} // namespace Rosen
} // namespace OHOS