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

#include "ge_grey_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEGreyShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Image> MakeImage(Drawing::Canvas& canvas);

    static inline Drawing::Canvas canvas_;
    std::shared_ptr<Drawing::Image> image_ { nullptr };

    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect src_ { 1.0f, 1.0f, 2.0f, 2.0f };
    Drawing::Rect dst_ { 1.0f, 1.0f, 2.0f, 2.0f };
};

void GEGreyShaderFilterTest::SetUpTestCase(void) {}
void GEGreyShaderFilterTest::TearDownTestCase(void) {}

void GEGreyShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEGreyShaderFilterTest::TearDown() {}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify the DrawImageEffect: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEGreyShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage002 start";

    // 0.0, 0.0   invalid Grey blur params
    Drawing::GEGreyShaderFilterParams geGreyShaderFilterParams { 0.0, 0.0 };
    std::unique_ptr<GEGreyShaderFilter> geGreyShaderFilter =
        std::make_unique<GEGreyShaderFilter>(geGreyShaderFilterParams);
    EXPECT_EQ(geGreyShaderFilter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage002 end";
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify the DrawImageEffect: filter params is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEGreyShaderFilterTest, ProcessImage003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage003 start";

    // -1.0, -1.0   invalid Grey blur params
    Drawing::GEGreyShaderFilterParams geGreyShaderFilterParams { -1.0, -1.0 };
    std::unique_ptr<GEGreyShaderFilter> geGreyShaderFilter =
        std::make_unique<GEGreyShaderFilter>(geGreyShaderFilterParams);
    EXPECT_EQ(geGreyShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage003 end";
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify the DrawImageEffect: filter params is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEGreyShaderFilterTest, ProcessImage004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage004 start";

    // 128.0, 128.0   invalid Grey blur params
    Drawing::GEGreyShaderFilterParams geGreyShaderFilterParams { 128.0, 128.0 };
    std::unique_ptr<GEGreyShaderFilter> geGreyShaderFilter =
        std::make_unique<GEGreyShaderFilter>(geGreyShaderFilterParams);
    EXPECT_EQ(geGreyShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage004 end";
}

/**
 * @tc.name: ProcessImage005
 * @tc.desc: Verify the DrawImageEffect: filter params is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEGreyShaderFilterTest, ProcessImage005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage005 start";

    // 0.0, 0.0   invalid Grey blur params
    Drawing::GEGreyShaderFilterParams geGreyShaderFilterParams { 0.0, 0.0 };
    std::unique_ptr<GEGreyShaderFilter> geGreyShaderFilter =
        std::make_unique<GEGreyShaderFilter>(geGreyShaderFilterParams);
    EXPECT_EQ(geGreyShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage005 end";
}

/**
 * @tc.name: ProcessImage006
 * @tc.desc: Verify the DrawImageEffect: filter params is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEGreyShaderFilterTest, ProcessImage006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage006 start";

    // 1.0, 1.0   valid Grey blur params
    Drawing::GEGreyShaderFilterParams geGreyShaderFilterParams { 1.0, 1.0 };
    std::unique_ptr<GEGreyShaderFilter> geGreyShaderFilter =
        std::make_unique<GEGreyShaderFilter>(geGreyShaderFilterParams);
    EXPECT_EQ(geGreyShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage006 end";
}

/**
 * @tc.name: ProcessImage007
 * @tc.desc: Verify the DrawImageEffect: filter params is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEGreyShaderFilterTest, ProcessImage007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage007 start";

    // 0.5, 0.5   invalid Grey blur params
    Drawing::GEGreyShaderFilterParams geGreyShaderFilterParams { 0.5, 0.5 };
    std::unique_ptr<GEGreyShaderFilter> geGreyShaderFilter =
        std::make_unique<GEGreyShaderFilter>(geGreyShaderFilterParams);
    EXPECT_EQ(geGreyShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEGreyShaderFilterTest ProcessImage007 end";
}

} // namespace Rosen
} // namespace OHOS
