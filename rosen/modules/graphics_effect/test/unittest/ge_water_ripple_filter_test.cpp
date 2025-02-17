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
 
#include "ge_water_ripple_filter.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace GraphicsEffectEngine {
 
using namespace Rosen;
 
class GEWaterRippleFilterTest : public testing::Test {
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
 
void GEWaterRippleFilterTest::SetUpTestCase(void) {}
void GEWaterRippleFilterTest::TearDownTestCase(void) {}
 
void GEWaterRippleFilterTest::SetUp()
{
    canvas_.Restore();
 
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(50, 50, format); // 50, 50  bitmap size
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    image_ = bmp.MakeImage();
}
 
void GEWaterRippleFilterTest::TearDown() {}
 
/**
 * @tc.name: ProcessImage001
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, ProcessImage001, TestSize.Level1)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.7f, 1.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_NE(geWaterRippleFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}
 
/**
 * @tc.name: ProcessImage002
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, ProcessImage002, TestSize.Level1)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.7f, 1.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_EQ(geWaterRippleFilter->ProcessImage(canvas_, nullptr, src_, dst_), nullptr);
}

/**
 * @tc.name: ProcessImage003
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, ProcessImage003, TestSize.Level1)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.0f, 2.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_NE(geWaterRippleFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage004
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, ProcessImage004, TestSize.Level1)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2.0f, 0.5f, 0.0f, 0.0f };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_NE(geWaterRippleFilter->ProcessImage(canvas_, image_, src_, dst_), image_);
}

/**
 * @tc.name: ProcessImage005
 * @tc.desc: Verify function ProcessImage
 * @tc.type:FUNC
 */
HWTEST_F(GEWaterRippleFilterTest, ProcessImage005, TestSize.Level1)
{
    Drawing::GEWaterRippleFilterParams geWaterRippleFilterParams { 0.5f, 2, 0.5f, 0.0f, 3 };
    auto geWaterRippleFilter = std::make_shared<GEWaterRippleFilter>(geWaterRippleFilterParams);
    EXPECT_EQ(geWaterRippleFilter->ProcessImage(canvas_, image_, src_, dst_), nullptr);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS