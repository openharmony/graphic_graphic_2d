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

#include <gtest/gtest.h>
#include "pipeline/slr_scale/rs_slr_scale.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSLRScaleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline float mainWidth = 3120.0f;
    static inline float mainHeight = 2080.0f;
    static inline float width1K = 1920.0f;
    static inline float height1K = 1080.0f;
    static inline float width2K = 2560.0f;
    static inline float height2K = 1440.0f;
    static inline std::shared_ptr<RSSLRScaleFunction> scaleManager_;
};

void RSSLRScaleTest::SetUpTestCase()
{
    scaleManager_ = std::make_shared<RSSLRScaleFunction>(
            width1K, height1K, mainWidth, mainHeight);
}

void RSSLRScaleTest::TearDownTestCase() {}
void RSSLRScaleTest::SetUp() {}
void RSSLRScaleTest::TearDown() {}

/**
 * @tc.name: CheckOrRefreshScreenTest
 * @tc.desc: test CheckOrRefreshScreen
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSSLRScaleTest, CheckOrRefreshScreenTest, TestSize.Level1)
{
    scaleManager_->CheckOrRefreshScreen(0.0f, height1K, mainWidth, mainHeight);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);
    scaleManager_->CheckOrRefreshScreen(width1K, 0.0f, mainWidth, mainHeight);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);
    scaleManager_->CheckOrRefreshScreen(width1K, height1K, 0.0f, mainHeight);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);
    scaleManager_->CheckOrRefreshScreen(width1K, height1K, mainWidth, 0.0f);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);

    scaleManager_->CheckOrRefreshScreen(width2K, height2K, mainWidth, mainHeight);
    scaleManager_->CheckOrRefreshScreen(width1K, height1K, mainWidth, mainHeight);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);
}

/**
 * @tc.name: RefreshColorSpaceTest
 * @tc.desc: test RefreshColorSpace
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSSLRScaleTest, RefreshColorSpaceTest, TestSize.Level1)
{
    scaleManager_->RefreshColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);
    scaleManager_->RefreshColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);
    scaleManager_->RefreshColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    ASSERT_EQ(scaleManager_->isSLRCopy_, false);
}

/**
 * @tc.name: ProcessCacheImageTest
 * @tc.desc: test ProcessCacheImage
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSSLRScaleTest, ProcessCacheImageTest, TestSize.Level1)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(mainWidth, mainHeight);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    scaleManager_->ProcessCacheImage(*canvas, *image);
    scaleManager_->ProcessOffscreenImage(*canvas, *image);
    ASSERT_NE(scaleManager_->slrFilterShader_, nullptr);
    ASSERT_EQ(scaleManager_->laplaceFilterShader_, nullptr);
    ASSERT_NE(scaleManager_->slrShaderBuilder_, nullptr);
    ASSERT_EQ(scaleManager_->laplaceShaderBuilder_, nullptr);
}
} // OHOS::Rosen