/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSDirtyRectsDFXTest : public testing::Test {
public:
    std::shared_ptr<RSDirtyRectsDfx> rsDirtyRectsDfx_;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSDisplayRenderNodeDrawable> displayDrawable_ = nullptr;
    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    std::shared_ptr<RSDisplayRenderNode> displayRenderNode_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDirtyRectsDFXTest::SetUpTestCase() {}
void RSDirtyRectsDFXTest::TearDownTestCase() {}
void RSDirtyRectsDFXTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    RSDisplayNodeConfig config;
    displayRenderNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID, config);
    if (!renderNode_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create surface node.");
        return;
    }
    displayDrawable_ = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(displayRenderNode_));
    if (!displayDrawable_->renderParams_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init displayDrawable_.");
        return;
    }
    auto displayRenderParams = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (!displayRenderParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init displayRenderParams.");
        return;
    }
    rsDirtyRectsDfx_ = std::make_shared<RSDirtyRectsDfx>(*displayDrawable_);
    if (!rsDirtyRectsDfx_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create RSDirtyRectsDfx.");
        return;
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
}
void RSDirtyRectsDFXTest::TearDown() {}

/**
 * @tc.name: DrawDirtyRegionInVirtual
 * @tc.desc: Test If DrawDirtyRegionInVirtual Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawDirtyRegionInVirtual, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawDirtyRegionInVirtual(*canvas_);
}

/**
 * @tc.name: OnDrawVirtualTest
 * @tc.desc: Test If OnDrawVirtual Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, OnDrawVirtual, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    rsDirtyRectsDfx_->OnDraw(*canvas);
    ASSERT_NE(canvas_, nullptr);
    rsDirtyRectsDfx_->OnDrawVirtual(*canvas_);
    ASSERT_FALSE(RSUniRenderThread::Instance().GetRSRenderThreadParams()->isVirtualDirtyDfxEnabled_);

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isVirtualDirtyDfxEnabled_ = true;
    rsDirtyRectsDfx_->OnDrawVirtual(*canvas_);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isVirtualDirtyDfxEnabled_ = false;
}

/**
 * @tc.name: OnDrawest
 * @tc.desc: Test If OnDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, OnDraw, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    rsDirtyRectsDfx_->OnDraw(*canvas_);
    ASSERT_NE(canvas_, nullptr);
    rsDirtyRectsDfx_->OnDraw(*canvas_);

    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_TRUE(renderThreadParams);
    renderThreadParams->isPartialRenderEnabled_ = true;
    renderThreadParams->isOpaqueRegionDfxEnabled_ = true;
    renderThreadParams->isVisibleRegionDfxEnabled_ = true;
    RSRealtimeRefreshRateManager::Instance().showEnabled_ = true;
    rsDirtyRectsDfx_->OnDraw(*canvas_);

    renderThreadParams->isDirtyRegionDfxEnabled_ = true;
    renderThreadParams->isTargetDirtyRegionDfxEnabled_ = true;
    renderThreadParams->isDisplayDirtyDfxEnabled_ = true;
    rsDirtyRectsDfx_->OnDraw(*canvas_);
    renderThreadParams->isPartialRenderEnabled_ = false;
    renderThreadParams->isOpaqueRegionDfxEnabled_ = false;
    renderThreadParams->isVisibleRegionDfxEnabled_ = false;
    RSRealtimeRefreshRateManager::Instance().showEnabled_ = false;
    renderThreadParams->isDirtyRegionDfxEnabled_ = false;
    renderThreadParams->isTargetDirtyRegionDfxEnabled_ = false;
    renderThreadParams->isDisplayDirtyDfxEnabled_ = false;
}

/**
 * @tc.name: DrawDirtyRegionForDFX
 * @tc.desc: Test If DrawDirtyRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawDirtyRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    const auto& visibleDirtyRects = rsDirtyRectsDfx_->dirtyRegion_.GetRegionRects();
    std::vector<RectI> rects;
    for (auto& rect : visibleDirtyRects) {
        rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
    }
    rsDirtyRectsDfx_->DrawDirtyRegionForDFX(*canvas_, rects);

    auto& targetDrawable = rsDirtyRectsDfx_->targetDrawable_;
    auto dirtyManager = targetDrawable.GetSyncDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    rects = dirtyManager->GetMergedDirtyRegions();
    rsDirtyRectsDfx_->DrawDirtyRegionForDFX(*canvas_, rects);
}

/**
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX
 * @tc.desc: Test If DrawTargetSurfaceDirtyRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawTargetSurfaceDirtyRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawTargetSurfaceDirtyRegionForDFX(*canvas_);
    ASSERT_NE(rsDirtyRectsDfx_->targetDrawable_.GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawCurrentRefreshRate
 * @tc.desc: Test If DrawCurrentRefreshRate Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawCurrentRefreshRate, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    
    auto& rSRealtimeRefreshRateManager = RSRealtimeRefreshRateManager::Instance();
    rSRealtimeRefreshRateManager.currRealtimeRefreshRateMap_[0] = 200; // 200: value greater than currRefreshRate

    auto paramPtr = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());

    paramPtr->screenId_ = 0;
    paramPtr->screenRotation_ = ScreenRotation::ROTATION_0;
    rsDirtyRectsDfx_->DrawCurrentRefreshRate(*canvas);
    paramPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    rsDirtyRectsDfx_->DrawCurrentRefreshRate(*canvas);

    paramPtr->screenId_ = -1;
    paramPtr->screenRotation_ = ScreenRotation::ROTATION_0;
    rsDirtyRectsDfx_->DrawCurrentRefreshRate(*canvas);
    paramPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    rsDirtyRectsDfx_->DrawCurrentRefreshRate(*canvas);

    paramPtr->screenId_ = 0;
    paramPtr->screenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    rsDirtyRectsDfx_->DrawCurrentRefreshRate(*canvas);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawAllSurfaceOpaqueRegionForDFX
 * @tc.desc: Test If DrawAllSurfaceOpaqueRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawAllSurfaceOpaqueRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawAllSurfaceOpaqueRegionForDFX(*canvas_);
    auto& targetDrawable = rsDirtyRectsDfx_->targetDrawable_;
    ASSERT_NE(targetDrawable.GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawHwcRegionForDFXTest
 * @tc.desc: Test DrawHwcRegionForDFX with different type drawables
 * @tc.type: FUNC
 * @tc.require: issueIB278U
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawHwcRegionForDFXTest, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    auto params = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(params, nullptr);

    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    drawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetHardwareEnabled(true);
    params->hardwareEnabledTypeDrawables_.emplace_back(drawable);

    std::shared_ptr<RSSurfaceRenderNode> renderNodeHardwareDisabled = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(renderNodeHardwareDisabled, nullptr);
    auto drawableHardwareDisabled = RSSurfaceRenderNodeDrawable::OnGenerate(renderNodeHardwareDisabled);
    drawableHardwareDisabled->renderParams_ = std::make_unique<RSSurfaceRenderParams>(1);
    auto surfaceParamsHardwareDisabled =
        static_cast<RSSurfaceRenderParams*>(drawableHardwareDisabled->GetRenderParams().get());
    ASSERT_NE(surfaceParamsHardwareDisabled, nullptr);
    surfaceParamsHardwareDisabled->SetHardwareEnabled(false);
    params->hardwareEnabledTypeDrawables_.emplace_back(drawableHardwareDisabled);

    params->hardwareEnabledTypeDrawables_.emplace_back(nullptr);

    std::shared_ptr<RSSurfaceRenderNode> renderNodeParamsNull = std::make_shared<RSSurfaceRenderNode>(2);
    ASSERT_NE(renderNodeParamsNull, nullptr);
    auto drawableParamsNull = RSSurfaceRenderNodeDrawable::OnGenerate(renderNodeParamsNull);
    drawableParamsNull->renderParams_ = nullptr;
    params->hardwareEnabledTypeDrawables_.emplace_back(drawableParamsNull);

    RSUniRenderThread::Instance().Sync(move(params));
    ASSERT_NE(canvas_, nullptr);
    rsDirtyRectsDfx_->DrawHwcRegionForDFX(*canvas_);
}

/**
 * @tc.name: DrawTargetSurfaceVisibleRegionForDFX
 * @tc.desc: Test If DrawTargetSurfaceVisibleRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawTargetSurfaceVisibleRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    rsDirtyRectsDfx_->DrawTargetSurfaceVisibleRegionForDFX(*canvas_);
}

/**
 * @tc.name: DrawDirtyRectForDFX
 * @tc.desc: Test If DrawDirtyRectForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawDirtyRectForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    RectI dirtyRect(1, 1, 1, 1);
    Drawing::Color color;
    RSDirtyRectsDfx::RSPaintStyle fillType = RSDirtyRectsDfx::RSPaintStyle::STROKE;
    int edgeWidth = 1;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsDirtyRectsDfx_->DrawDirtyRectForDFX(*canvas_, dirtyRect, color, fillType, edgeWidth);
    ASSERT_TRUE(rsDirtyRectsDfx_->displayParams_);

    fillType = RSDirtyRectsDfx::RSPaintStyle::FILL;
    rsDirtyRectsDfx_->DrawDirtyRectForDFX(*canvas_, dirtyRect, color, fillType, edgeWidth);
    dirtyRect.height_ = 0;
    rsDirtyRectsDfx_->DrawDirtyRectForDFX(*canvas_, dirtyRect, color, fillType, edgeWidth);
    dirtyRect.width_ = 0;
    rsDirtyRectsDfx_->DrawDirtyRectForDFX(*canvas_, dirtyRect, color, fillType, edgeWidth);
    ASSERT_TRUE(canvas_);
}

/**
 * @tc.name: DrawDetailedTypesOfDirtyRegionForDFX
 * @tc.desc: Test If DrawDetailedTypesOfDirtyRegionForDFX Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDirtyRectsDFXTest, DrawDetailedTypesOfDirtyRegionForDFX, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    std::shared_ptr<RSSurfaceRenderNode> renderNode;
    RSRenderNodeDrawableAdapter* drawable = nullptr;
    RSSurfaceRenderNodeDrawable* surfaceDrawable = nullptr;
    renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    drawable = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    if (drawable) {
        drawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
        surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable);
    }
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    auto params = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(move(params));
    bool res = rsDirtyRectsDfx_->DrawDetailedTypesOfDirtyRegionForDFX(*canvas_, *surfaceDrawable);
    ASSERT_FALSE(res);

    params = std::make_unique<RSRenderThreadParams>();
    params->dirtyRegionDebugType_ = DirtyRegionDebugType::SUBTREE_SKIP_RECT;
    RSUniRenderThread::Instance().Sync(move(params));
    res = rsDirtyRectsDfx_->DrawDetailedTypesOfDirtyRegionForDFX(*canvas_, *surfaceDrawable);
    ASSERT_TRUE(res);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->dirtyRegionDebugType_ = DirtyRegionDebugType::DISABLED;
}

/**
 * @tc.name: RefreshRateRotationProcess
 * @tc.desc: Test If RefreshRateRotationProcess Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSDirtyRectsDFXTest, RefreshRateRotationProcess, TestSize.Level1)
{
    ASSERT_NE(rsDirtyRectsDfx_, nullptr);
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    uint64_t screenId = 0;
    bool res = rsDirtyRectsDfx_->RefreshRateRotationProcess(*canvas_, rotation, screenId);
    ASSERT_TRUE(res);

    rotation = ScreenRotation::ROTATION_90;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    res = rsDirtyRectsDfx_->RefreshRateRotationProcess(*canvas_, rotation, screenId);
    ASSERT_TRUE(res);

    rotation = ScreenRotation::ROTATION_180;
    res = rsDirtyRectsDfx_->RefreshRateRotationProcess(*canvas_, rotation, screenId);
    ASSERT_TRUE(res);

    rotation = ScreenRotation::ROTATION_270;
    res = rsDirtyRectsDfx_->RefreshRateRotationProcess(*canvas_, rotation, screenId);
    ASSERT_TRUE(res);

    rotation = ScreenRotation::INVALID_SCREEN_ROTATION;
    res = rsDirtyRectsDfx_->RefreshRateRotationProcess(*canvas_, rotation, screenId);
    ASSERT_FALSE(res);
}
}
