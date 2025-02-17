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

#include "ge_mesa_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEMESABlurShaderFilterTest : public testing::Test {
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

void GEMESABlurShaderFilterTest::SetUpTestCase(void) {}
void GEMESABlurShaderFilterTest::TearDownTestCase(void) {}

void GEMESABlurShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
    src_ = image_->GetImageInfo().GetBound();
}

void GEMESABlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetRadius001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius001, TestSize.Level1)
{
    // 1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), 1);
}

/**
 * @tc.name: GetRadius002
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius002, TestSize.Level1)
{
    // 8000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{8000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), params.radius);
}

/**
 * @tc.name: GetRadius003
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius003, TestSize.Level1)
{
    // 10000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{10000, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), 8000);
}

/**
 * @tc.name: GetRadius004
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, GetRadius004, TestSize.Level1)
{
    // 0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter->GetRadius(), 0);
}


/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, ProcessImage001, TestSize.Level1)
{
    // 0, 1, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params{1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(geMESABlurShaderFilter->ProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    // 0, 0.f, 1.f: valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params1{0, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0, 0.f, 0.f};
    auto geMESABlurShaderFilter1 = std::make_shared<GEMESABlurShaderFilter>(params1);
    ASSERT_TRUE(geMESABlurShaderFilter1 != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter1->ProcessImage(canvas_, image_, src_, dst_), image_);

    // 8001: valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params2{8001, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0, 1.f, 1.f};
    auto geMESABlurShaderFilter2 = std::make_shared<GEMESABlurShaderFilter>(params2);
    ASSERT_TRUE(geMESABlurShaderFilter2 != nullptr);

    EXPECT_EQ(geMESABlurShaderFilter2->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, ProcessImage003, TestSize.Level1)
{
    // 1,7,12,18,21,55,81,120,240,360: valid blur radius
    int blurRadius[] = {1, 7, 12, 18, 21, 55, 81, 120, 240, 360};
    for (auto radius : blurRadius) {
        // 0, 0.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_EQ(geMESABlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
    }

    for (auto radius : blurRadius) {
        // 1, 1.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1, 1.f, 1.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_EQ(geMESABlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
    }

    for (auto radius : blurRadius) {
        // 2, 2.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 2.f, 2.f, 2.f, 2.f, 2.f, 2.f, 2, 2.f, 2.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_EQ(geMESABlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
    }
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, ProcessImage004, TestSize.Level1)
{
    // 1,7,12,18,21,55,81,120,240,360: valid blur radius
    int blurRadius[] = {1, 7, 12, 18, 21, 55, 81, 120, 240, 360};
    for (auto radius : blurRadius) {
        // 0, 0.f, 1.f: valid MESA blur params
        Drawing::GEMESABlurShaderFilterParams params{radius, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
        auto geMESABlurShaderFilter = std::make_shared<GEMESABlurShaderFilter>(params);
        ASSERT_TRUE(geMESABlurShaderFilter != nullptr);

        EXPECT_EQ(geMESABlurShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
    }
}

/**
 * @tc.name: ScaleAndAddRandomColor001
 * @tc.desc: Verify function ScaleAndAddRandomColor
 * @tc.type:FUNC
 */
HWTEST_F(GEMESABlurShaderFilterTest, ScaleAndAddRandomColor001, TestSize.Level1)
{
    // 1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params {1, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto filter = std::make_shared<GEMESABlurShaderFilter>(params);
    std::shared_ptr<Drawing::Image> image { nullptr };
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(100, 30, format); // 100, 30  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    std::shared_ptr<Drawing::Image> imageBlur = bmp.MakeImage();
    auto width = std::max(static_cast<int>(std::ceil(dst_.GetWidth())), imageBlur->GetWidth());
    auto height = std::max(static_cast<int>(std::ceil(dst_.GetHeight())), imageBlur->GetHeight());
    EXPECT_NE(filter->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);

    // 88: valid MESA blur params
    Drawing::GEMESABlurShaderFilterParams params2 {88, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0, 0.f, 0.f};
    auto filter2 = std::make_shared<GEMESABlurShaderFilter>(params2);
    EXPECT_NE(filter2->ScaleAndAddRandomColor(canvas_, image_, imageBlur, src_, dst_, width, height), image_);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
