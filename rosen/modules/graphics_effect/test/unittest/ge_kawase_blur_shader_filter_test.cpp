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

#include <gtest/gtest.h>

#include "ge_kawase_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEKawaseBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEKawaseBlurShaderFilterTest::SetUpTestCase(void) {}
void GEKawaseBlurShaderFilterTest::TearDownTestCase(void) {}

void GEKawaseBlurShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEKawaseBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetRadius001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, GetRadius001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1}; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    EXPECT_EQ(geKawaseBlurShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1}; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{1}; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage003, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{0}; // 0 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ProcessImage004, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params{8001}; // 8001 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_EQ(geKawaseBlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ScaleAndAddRandomColor001
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    std::shared_ptr<Drawing::Image> image { nullptr };
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    EXPECT_NE(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);
}

/**
 * @tc.name: ScaleAndAddRandomColor002
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ScaleAndAddRandomColor002, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto filter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    filter->blurScale_ = 0;
    EXPECT_EQ(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), imageBlur);
}

/**
 * @tc.name: ComputeRadiusAndScale001
 * @tc.desc: Verify function ComputeRadiusAndScale
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, ComputeRadiusAndScale001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);

    geKawaseBlurShaderFilter->ComputeRadiusAndScale(200); // 200 blur radius
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(50); // 50 blur radius
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(20); // 20 blur radius
    geKawaseBlurShaderFilter->ComputeRadiusAndScale(9); // 9 blur radius
    std::string expect = "blur radius is " + std::to_string(float(36)); // 36 match result
    EXPECT_EQ(geKawaseBlurShaderFilter->GetDescription(), expect);
}

/**
 * @tc.name: InitSimpleFilter001
 * @tc.desc: Verify function InitSimpleFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitSimpleFilter001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitSimpleFilter());
}

/**
 * @tc.name: InitBlurEffectForAdvancedFilter001
 * @tc.desc: Verify function InitBlurEffectForAdvancedFilter
 * @tc.type:FUNC
 */
HWTEST_F(GEKawaseBlurShaderFilterTest, InitBlurEffectForAdvancedFilter001, TestSize.Level1)
{
    Drawing::GEKawaseBlurShaderFilterParams params { 1 }; // 1 blur radius
    auto geKawaseBlurShaderFilter = std::make_shared<GEKawaseBlurShaderFilter>(params);
    EXPECT_TRUE(geKawaseBlurShaderFilter->InitBlurEffectForAdvancedFilter());
}


} // namespace GraphicsEffectEngine
} // namespace OHOS
