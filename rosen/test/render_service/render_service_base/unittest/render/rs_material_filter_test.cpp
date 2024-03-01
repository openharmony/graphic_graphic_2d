/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "draw/surface.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaterialFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaterialFilterTest::SetUpTestCase() {}
void RSMaterialFilterTest::TearDownTestCase() {}
void RSMaterialFilterTest::SetUp() {}
void RSMaterialFilterTest::TearDown() {}

/**
 * @tc.name: CreateMaterialStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyle001, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialStyle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyle002, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: PostProcessTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PostProcessTest, TestSize.Level1)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    rsMaterialFilter.PostProcess(*canvas);
    EXPECT_NE(canvas, nullptr);
}

/**
 * @tc.name: CanSkipFrameTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CanSkipFrameTest, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    auto res = rsMaterialFilter.CanSkipFrame();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsNearEqual001, TestSize.Level1)
{
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    float radius = 0.5f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, color };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);

    float saturation1 = 1.2f;
    struct MaterialParam materialParam1 = { radius, saturation1, brightness, color };
    float threshold = 1.0f;
    std::shared_ptr<RSFilter> rsMaterialFilter1 = std::make_shared<RSMaterialFilter>(materialParam1, mode);
    EXPECT_TRUE(rsMaterialFilter->IsNearEqual(rsMaterialFilter1, threshold));
}

/**
 * @tc.name: IsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsNearZero001, TestSize.Level1)
{
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    float radius = 0.5f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, color };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);

    float threshold = 1.0f;
    EXPECT_TRUE(rsMaterialFilter->IsNearZero(threshold));
}
} // namespace OHOS::Rosen
