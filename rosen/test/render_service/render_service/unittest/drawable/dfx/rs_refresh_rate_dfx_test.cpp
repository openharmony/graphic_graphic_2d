/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "drawable/dfx/rs_refresh_rate_dfx.h"
#include "hgm_core.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "pipeline/rs_logical_display_render_node.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "params/rs_logical_display_render_params.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr ScreenId SCREEN_ID = 0xFF;

class RSRefreshRateDfxTest : public testing:Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<RSLogicalDisplayRenderNodeDrawable> displayDrawable_;
    std::shared_ptr<RSLogicalDisplayRenderNodeDrawable> mirroredDisplayDrawable_;
    std::shared_ptr<RSScreenRenderNodeDrawable> screenDrawable_;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayNode_;
    std::shared_ptr<RSScreenRenderNode> screenRenderNode_;
};

void RSRefreshRateDfxTest::SetUpTestCase() {}
void RSRefreshRateDfxTest::TearDownTestCase() {}
void RSRefreshRateDfxTest::SetUp()
{
    RSDisplayNodeConfig config;
    displayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID, config);
    displayNode_->InitRenderParams();
    displayDrawable_ = std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(displayNode_->GetRenderDrawable());

    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    screenRenderNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, SCREEN_ID, context);
    screenDrawable_ = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(screenRenderNode_));
    if (!screenDrawable_->renderParams_) {
        RS_LOGE("RSRefreshRateDfxTest: failed to init screenDrawable_.");
        return;
    }

    ASSERT_NE(displayDrawable_->GetRenderParams(), nullptr);
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayParams->SetAncestorScreenDrawable(screenDrawable_);
}
void RSRefreshRateDfxTest::TearDown() {}

/**
 * @tc.name: OnDrawTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, OnDrawTest001, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();
    instance.SetShowRefreshRateEnabled(true, 1);
    bool ret = instance.GetShowRefreshRateEnabled();
    EXPECT_EQ(ret, true);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsRefreshRateDfx.OnDraw(*canvas);
}

/**
 * @tc.name: OnDrawTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, OnDrawTest002, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();
    instance.SetShowRefreshRateEnabled(false, 1);
    bool ret = instance.GetShowRefreshRateEnabled();
    EXPECT_EQ(ret, false);

    EXPECT_NE(displayDrawable_, nullptr);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsRefreshRateDfx.OnDraw(*canvas);
}

/**
 * @tc.name: RefreshRateRotationProcessTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, RefreshRateRotationProcessTest001, TestSize.Level1)
{
    EXPECT_NE(displayDrawable_, nullptr);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    uint32_t translateWidth = 0;
    uint32_t translateHeight = 0;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    EXPECT_TRUE(rsRefreshRateDfx.RefreshRateRotationProcess(*canvas, rotation, translateWidth, translateHeight));
}

/**
 * @tc.name: RefreshRateRotationProcessTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, RefreshRateRotationProcessTest002, TestSize.Level1)
{
    EXPECT_NE(displayDrawable_, nullptr);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ScreenRotation rotation = ScreenRotation::ROTATION_90;
    uint32_t translateWidth = 90;
    uint32_t translateHeight = 90;
    EXPECT_TRUE(rsRefreshRateDfx.RefreshRateRotationProcess(*canvas, rotation, translateWidth, translateHeight));
}

/**
 * @tc.name: RefreshRateRotationProcessTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, RefreshRateRotationProcessTest003, TestSize.Level1)
{
    EXPECT_NE(displayDrawable_, nullptr);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ScreenRotation rotation = ScreenRotation::ROTATION_180;
    uint32_t translateWidth = 90;
    uint32_t translateHeight = 90;
    EXPECT_TRUE(rsRefreshRateDfx.RefreshRateRotationProcess(*canvas, rotation, translateWidth, translateHeight));
}

/**
 * @tc.name: RefreshRateRotationProcessTest004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, RefreshRateRotationProcessTest004, TestSize.Level1)
{
    EXPECT_NE(displayDrawable_, nullptr);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ScreenRotation rotation = ScreenRotation::ROTATION_270;
    uint32_t translateWidth = 90;
    uint32_t translateHeight = 90;
    EXPECT_TRUE(rsRefreshRateDfx.RefreshRateRotationProcess(*canvas, rotation, translateWidth, translateHeight));
}

/**
 * @tc.name: RefreshRateRotationProcessTest005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, RefreshRateRotationProcessTest005, TestSize.Level1)
{
    EXPECT_NE(displayDrawable_, nullptr);

    displayDrawable_->renderParams_ = nullptr;
    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    uint32_t translateWidth = 0;
    uint32_t translateHeight = 0;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    EXPECT_TRUE(rsRefreshRateDfx.RefreshRateRotationProcess(*canvas, rotation, translateWidth, translateHeight));
}

/**
 * @tc.name: RefreshRateRotationProcessTest006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, RefreshRateRotationProcessTest006, TestSize.Level1)
{
    EXPECT_NE(displayDrawable_, nullptr);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    uint32_t translateWidth = 0;
    uint32_t translateHeight = 0;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayParams->SetAncestorScreenDrawable(nullptr);
    EXPECT_TRUE(rsRefreshRateDfx.RefreshRateRotationProcess(*canvas, rotation, translateWidth, translateHeight));
}

/**
 * @tc.name: RefreshRateRotationProcessTest007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRefreshRateDfxTest, RefreshRateRotationProcessTest007, TestSize.Level1)
{
    EXPECT_NE(displayDrawable_, nullptr);

    RSRefreshRateDfx rsRefreshRateDfx(*displayDrawable_);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    uint32_t translateWidth = 0;
    uint32_t translateHeight = 0;
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    screenDrawable_->renderParams_ = nullptr;
    EXPECT_TRUE(rsRefreshRateDfx.RefreshRateRotationProcess(*canvas, rotation, translateWidth, translateHeight));
}
}