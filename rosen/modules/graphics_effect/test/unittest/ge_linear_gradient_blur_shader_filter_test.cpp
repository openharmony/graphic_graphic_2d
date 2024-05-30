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

#include "ge_linear_gradient_blur_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GELinearGradientBlurShaderFilterTest : public testing::Test {
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

void GELinearGradientBlurShaderFilterTest::SetUpTestCase(void) {}
void GELinearGradientBlurShaderFilterTest::TearDownTestCase(void) {}

void GELinearGradientBlurShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GELinearGradientBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDescription001, TestSize.Level1)
{
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilter";
    EXPECT_EQ(filter->GetDescription(), expectStr);
}

/**
 * @tc.name: GetDetailedDescription001
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, GetDetailedDescription001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::string expectStr = "GELinearGradientBlurShaderFilterBlur, radius: " +std::to_string(params.blurRadius);
    EXPECT_EQ(filter->GetDetailedDescription(), expectStr);
}

/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage001, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    std::shared_ptr<Drawing::Image> image = nullptr;
    EXPECT_EQ(filter->ProcessImage(canvas_, image, src_, dst_), image);
}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    EXPECT_NE(filter->ProcessImage(canvas_, image_, src_, dst_), image_);
}


/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage004, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 1 direction, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 1, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter != nullptr);

    // 1.0f, 1.0f, 200.0f, 200.0f is left top right bottom
    Drawing::Rect src { 1.0f, 1.0f, 200.0f, 200.0f };
    Drawing::Rect dst { 1.0f, 1.0f, 2.0f, 2.0f };
    EXPECT_NE(filter->ProcessImage(canvas_, image_, src, dst), image_);
}

/**
 * @tc.name: ProcessImage005
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GELinearGradientBlurShaderFilterTest, ProcessImage005, TestSize.Level1)
{
    // blur params: 1.f blurRadius, {0.1f, 0.1f} fractionStops, 0 direction LEFT, 1.f geoWidth, geoHeight, tranX, tranY
    Drawing::GELinearGradientBlurShaderFilterParams params{1.f, {{0.1f, 0.1f}}, 0, 1.f, 1.f,
        Drawing::Matrix(), 1.f, 1.f, true};
    auto filter0 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter0 != nullptr);
    EXPECT_NE(filter0->ProcessImage(canvas_, image_, src_, dst_), image_);

    params.direction = 2; // RIGHT
    auto filter2 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter2 != nullptr);
    EXPECT_NE(filter2->ProcessImage(canvas_, image_, src_, dst_), image_);

    params.direction = 3; // BOTTOM
    auto filter3 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter3 != nullptr);
    EXPECT_NE(filter3->ProcessImage(canvas_, image_, src_, dst_), image_);

    params.direction = 4; // LEFT_TOP
    auto filter4 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter4 != nullptr);
    EXPECT_NE(filter4->ProcessImage(canvas_, image_, src_, dst_), image_);

    params.direction = 5; // LEFT_BOTTOM
    auto filter5 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter5 != nullptr);
    EXPECT_NE(filter5->ProcessImage(canvas_, image_, src_, dst_), image_);

    params.direction = 6; // RIGHT_TOP
    auto filter6 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter6 != nullptr);
    EXPECT_NE(filter6->ProcessImage(canvas_, image_, src_, dst_), image_);

    params.direction = 7; // RIGHT_BOTTOM
    auto filter7 = std::make_shared<GELinearGradientBlurShaderFilter>(params);
    ASSERT_TRUE(filter7 != nullptr);
    EXPECT_NE(filter7->ProcessImage(canvas_, image_, src_, dst_), image_);
}


} // namespace GraphicsEffectEngine
} // namespace OHOS
