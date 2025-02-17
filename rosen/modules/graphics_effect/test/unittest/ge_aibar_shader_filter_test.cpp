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

#include "ge_aibar_shader_filter.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class GEAIBarShaderFilterTest : public testing::Test {
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

void GEAIBarShaderFilterTest::SetUpTestCase(void) {}
void GEAIBarShaderFilterTest::TearDownTestCase(void) {}

void GEAIBarShaderFilterTest::SetUp()
{
    canvas_.Restore();

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}

void GEAIBarShaderFilterTest::TearDown() {}

/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify the ProcessImage: image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage002 start";

    // 0.0, 0.0, 0.0, 0.0, 0.0 valid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage002 end";
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage003 start";

    // -0.5, -0.5, -0.5, -0.5, -0.5 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { -0.5, -0.5, -0.5, -0.5, -0.5 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage003 end";
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage004 start";

    // 2.0, 2.0, 2.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage004 end";
}

/**
 * @tc.name: ProcessImage005
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage005 start";

    // 1.0, 2.0, 2.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 1.0, 2.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage005 end";
}

/**
 * @tc.name: ProcessImage006
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage006 start";

    // 2.0, 1.0, 2.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 1.0, 2.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage006 end";
}

/**
 * @tc.name: ProcessImage007
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage007 start";

    // 2.0, 2.0, 1.0, 2.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 1.0, 2.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage007 end";
}

/**
 * @tc.name: ProcessImage008
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage008 start";

    // 2.0, 2.0, 2.0, 1.0, 3.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 1.0, 3.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage008 end";
}

/**
 * @tc.name: ProcessImage009
 * @tc.desc: Verify the ProcessImage: filter param is invalid
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, ProcessImage009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage009 start";

    // 2.0, 2.0, 2.0, 2.0, 1.0 invalid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 2.0, 2.0, 2.0, 2.0, 1.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->ProcessImage(canvas_, image_, src_, dst_), image_);

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest ProcessImage009 end";
}

/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify the GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(GEAIBarShaderFilterTest, GetDescription001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest GetDescription001 start";

    // 0.0, 0.0, 0.0, 0.0, 0.0 valid AI bar blur params
    Drawing::GEAIBarShaderFilterParams geAIBarShaderFilterParams { 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::unique_ptr<GEAIBarShaderFilter> geAIBarShaderFilter =
        std::make_unique<GEAIBarShaderFilter>(geAIBarShaderFilterParams);
    EXPECT_EQ(geAIBarShaderFilter->GetDescription(), "GEAIBarShaderFilter");

    GTEST_LOG_(INFO) << "GEAIBarShaderFilterTest GetDescription001 end";
}

} // namespace Rosen
} // namespace OHOS
