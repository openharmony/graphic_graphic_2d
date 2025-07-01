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

#include <parameters.h>

#include "drawable/rs_logical_display_render_node_drawable.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "params/rs_logical_display_render_params.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "platform/drawing/rs_surface_converter.h"
#include "screen_manager/rs_screen.h"
#include "render/rs_pixel_map_util.h"
// xml parser
#include "graphic_feature_param_manager.h"
#include "feature/hpae/rs_hpae_manager.h"
#include "feature/anco_manager/rs_anco_manager.h"
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr NodeId DEFAULT_DISPLAY_NODE_ID = 9;
constexpr NodeId DEFAULT_SURFACE_NODE_ID = 10;
constexpr NodeId DEFAULT_RENDER_NODE_ID = 11;
}
class RSLogicalDisplayRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSLogicalDisplayRenderNode> renderNode_;
    std::shared_ptr<RSLogicalDisplayRenderNode> mirroredNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSRenderNodeDrawableAdapter* mirroredDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* mirroredDisplayDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirror_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSLogicalDisplayRenderNodeDrawableTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSLogicalDisplayRenderNodeDrawableTest::TearDownTestCase() {}
void RSLogicalDisplayRenderNodeDrawableTest::SetUp()
{
    RSDisplayNodeConfig config;
    renderNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID, config);
    mirroredNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID + 1, config);
    if (!renderNode_ || !mirroredNode_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to create display node.");
        return;
    }

    drawable_ = RSLogicalDisplayRenderNodeDrawable::OnGenerate(renderNode_);
    mirroredDrawable_ = RSLogicalDisplayRenderNodeDrawable::OnGenerate(mirroredNode_);
    if (!drawable_ || !mirroredDrawable_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to create RSRenderNodeDrawableAdapter.");
        return;
    }

    displayDrawable_ = static_cast<RSLogicalDisplayRenderNodeDrawable*>(drawable_);
    mirroredDisplayDrawable_ = static_cast<RSLogicalDisplayRenderNodeDrawable*>(mirroredDrawable_);
    if (!displayDrawable_ || !mirroredDisplayDrawable_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to create drawable.");
        return;
    }

    displayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id);
    mirroredDisplayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id + 1);
    if (!drawable_->renderParams_ || !mirroredDrawable_->renderParams_) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawableTest: failed to init render params.");
        return;
    }
}
void RSLogicalDisplayRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateDisplayRenderNodeDrawableTest
 * @tc.desc: Test If DisplayRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CreateDisplayRenderNodeDrawable, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 1;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    auto drawable = RSLogicalDisplayRenderNodeDrawable::OnGenerate(displayNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: UpdateSlrScale001
 * @tc.desc: Test UpdateSlrScale
 * @tc.type: FUNC
 * @tc.require: #IBIOQ4
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, UpdateSlrScale001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto param = system::GetParameter("rosen.SLRScale.enabled", "");
    const int32_t width = DEFAULT_CANVAS_SIZE * 2;
    const int32_t height = DEFAULT_CANVAS_SIZE * 2;
    ScreenInfo screenInfo = {
        .phyWidth = DEFAULT_CANVAS_SIZE,
        .phyHeight = DEFAULT_CANVAS_SIZE,
        .width = width,
        .height = height,
        .isSamplingOn = true,
    };
    system::SetParameter("rosen.SLRScale.enabled", "1");
    displayDrawable_->UpdateSlrScale(screenInfo);

    system::SetParameter("rosen.SLRScale.enabled", "0");
    displayDrawable_->UpdateSlrScale(screenInfo);
    system::SetParameter("rosen.SLRScale.enabled", param);
}

/**
 * @tc.name: ScaleCanvasIfNeeded
 * @tc.desc: Test ScaleCanvasIfNeeded
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleCanvasIfNeeded001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto param = system::GetParameter("rosen.SLRScale.enabled", "");
    ScreenInfo screenInfo = {
        .phyWidth = DEFAULT_CANVAS_SIZE,
        .phyHeight = DEFAULT_CANVAS_SIZE,
        .width = DEFAULT_CANVAS_SIZE,
        .height = DEFAULT_CANVAS_SIZE,
        .isSamplingOn = false,
    };
    system::SetParameter("rosen.SLRScale.enabled", "1");
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);

    screenInfo.isSamplingOn = true;
    system::SetParameter("rosen.SLRScale.enabled", "0");
    displayDrawable_->ScaleCanvasIfNeeded(screenInfo);
    system::SetParameter("rosen.SLRScale.enabled", param);
}

/**
 * @tc.name: PrepareOffscreenRender001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRender001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);

    auto params = static_cast<RSScreenRenderParams*>(displayDrawable_->GetRenderParams().get());
    params->frameRect_ = { 0.f, 0.f, 1.f, 0.f };
    ASSERT_TRUE(params->IsRotationChanged());

    params->frameRect_ = { 0.f, 0.f, 1.f, 1.f };
    ASSERT_FALSE(displayDrawable_->curCanvas_->GetSurface());

    auto surface = std::make_shared<Drawing::Surface>();
    displayDrawable_->curCanvas_->surface_ = surface.get();
    ASSERT_TRUE(displayDrawable_->curCanvas_->GetSurface());
}

/**
 * @tc.name: PrepareOffscreenRender002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, PrepareOffscreenRender002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, with two surface with/without param initialization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
    NodeId id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable1 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode1);
    id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable2 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode2);
    surfaceNode2->InitRenderParams();
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, 1, drawable1));
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, 1, drawable2));
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
}

/**
 * @tc.name: DrawCurtainScreen
 * @tc.desc: Test DrawCurtainScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawCurtainScreen, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_TRUE(!RSUniRenderThread::Instance().IsCurtainScreenOn());

    auto params = std::make_unique<RSRenderThreadParams>();
    params->isCurtainScreenOn_ = true;
    RSUniRenderThread::Instance().Sync(move(params));
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = true;
    ASSERT_TRUE(RSUniRenderThread::Instance().IsCurtainScreenOn());
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = false;
}

/**
 * @tc.name: DrawWatermarkIfNeed
 * @tc.desc: Test DrawWatermarkIfNeed
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWatermarkIfNeed, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_TRUE(params);

    ASSERT_TRUE(RSUniRenderThread::Instance().GetRSRenderThreadParams());

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = true;

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = std::make_shared<Drawing::Image>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = false;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = nullptr;
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen001
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without mirrorNode
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);

    auto params = std::make_shared<RSLogicalDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;

    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_DISPLAY_NODE_ID, config);
    params->mirrorSourceDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen002
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, isVirtualDirtyEnabled_ false
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen002, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSLogicalDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = false;
    }
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen003
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without syncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen003, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSLogicalDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen004
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, canvasMatrix not equals to lastMatrix_
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen004, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSLogicalDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    const Drawing::scalar scale = 100.0f;
    canvasMatrix.SetScale(scale, scale);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen005
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen005, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSLogicalDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreen006
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #IA76UC
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreen006, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSLogicalDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyDfxEnabled_ = true;
    }
}

/**
 * @tc.name: RenderOverDraw
 * @tc.desc: Test RenderOverDraw
 * @tc.type: FUNC
 * @tc.require: #IB1MHX
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, RenderOverDraw, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: SetScreenRotationForPointLight
 * @tc.desc: Test SetScreenRotationForPointLight
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SetScreenRotationForPointLight, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    displayDrawable_->SetScreenRotationForPointLight(*params);
}

/**
 * @tc.name: HardCursorCreateLayer
 * @tc.desc: Test HardCursorCreateLayer
 * @tc.type: FUNC
 * @tc.require: #IAX2SN
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, HardCursorCreateLayerTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    ASSERT_NE(processor, nullptr);;

    NodeId nodeId = 1;
    auto renderNode = std::make_shared<RSRenderNode>(nodeId);
    auto drawablePtr = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    EXPECT_NE(drawablePtr, nullptr);
}

/**
 * @tc.name: CheckDisplayNodeSkip
 * @tc.desc: Test CheckDisplayNodeSkip
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckDisplayNodeSkipTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->forceCommitReason_ = 1;

    RSMainThread::Instance()->SetDirtyFlag(true);

    RSUifirstManager::Instance().hasForceUpdateNode_ = true;

    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->forceCommitReason_ = 0;
    RSMainThread::Instance()->isDirty_ = false;
    RSUifirstManager::Instance().hasForceUpdateNode_ = false;
}

/**
 * @tc.name: CheckAndUpdateFilterCacheOcclusion
 * @tc.desc: Test CheckAndUpdateFilterCacheOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckAndUpdateFilterCacheOcclusionTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceNode, nullptr);

    auto surfaceDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    ASSERT_NE(surfaceDrawableAdapter, nullptr);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawableAdapter->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> allSurfaceDrawable{surfaceDrawableAdapter};

    surfaceParams->isMainWindowType_ = false;

    surfaceParams->isMainWindowType_ = true;
}

/**
 * @tc.name: CheckFilterCacheFullyCovered
 * @tc.desc: Test CheckFilterCacheFullyCovered
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CheckFilterCacheFullyCoveredTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto surfaceParams = std::make_unique<RSSurfaceRenderParams>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->visibleFilterChild_ = {DEFAULT_RENDER_NODE_ID};

    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderNode, nullptr);
    auto renderDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    ASSERT_NE(renderDrawableAdapter, nullptr);
    renderDrawableAdapter->renderParams_ = std::make_unique<RSRenderParams>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderDrawableAdapter->renderParams_, nullptr);
    RectI screenRect{0, 0, 0, 0};

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(false);

    surfaceParams->isTransparent_ = true;
    surfaceParams->isRotating_ = true;

    surfaceParams->isRotating_ = false;

    surfaceParams->isTransparent_ = false;
    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);

    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(false);

    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::EFFECT_NODE);

    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(true);

    renderDrawableAdapter->renderParams_->SetGlobalAlpha(0.f);

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);
    renderDrawableAdapter->renderParams_->SetGlobalAlpha(1.f);
    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(false);
    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::CANVAS_NODE);
    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(true);
}

/**
 * @tc.name: OnDraw001
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    Drawing::Canvas canvas;
    displayDrawable_->OnDraw(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    HardCursorInfo hardInfo;
    hardInfo.id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(hardInfo.id);
    hardInfo.drawablePtr = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    displayDrawable_->OnDraw(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: OnDraw002
 * @tc.desc: Test OnDraw can skip when virtual expand screen do not need refresh
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->compositeType_ = CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    Drawing::Canvas canvas;
    displayDrawable_->OnDraw(canvas);
    // restore
    params->compositeType_ = CompositeType::HARDWARE_COMPOSITE;
}

/**
 * @tc.name: OnDraw003
 * @tc.desc: Test OnDraw should not skip when virtual expand screen need refresh
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->compositeType_ = CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    Drawing::Canvas canvas;
    displayDrawable_->OnDraw(canvas);
    // restore
    params->compositeType_ = CompositeType::HARDWARE_COMPOSITE;
}

/**
 * @tc.name: OnDrawTest004
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: wz
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, OnDrawTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas canvas;
    HardCursorInfo hardInfo;
    hardInfo.id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(hardInfo.id);
    hardInfo.drawablePtr = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    displayDrawable_->OnDraw(canvas);

    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = false;
    displayDrawable_->OnDraw(canvas);

    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSAncoManager::Instance()->SetAncoHebcStatus(AncoHebcStatus::NOT_USE_HEBC);
    displayDrawable_->OnDraw(canvas);

    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSAncoManager::Instance()->SetAncoHebcStatus(AncoHebcStatus::USE_HEBC);
    displayDrawable_->OnDraw(canvas);
}
/**
 * @tc.name: DrawMirrorScreen
 * @tc.desc: Test DrawMirrorScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorScreenTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    CaptureParam param;
    param.isSingleSurface_ = true;
    rtThread.SetCaptureParam(param);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    displayDrawable_->DrawMirrorScreen(*params, processor);
    sleep(1);
}

/**
 * @tc.name: CalculateVirtualDirty
 * @tc.desc: Test CalculateVirtualDirty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    sleep(1);
}

/**
 * @tc.name: DrawMirror
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();

    displayDrawable_->DrawMirror(*params, virtualProcesser,
        &RSLogicalDisplayRenderNodeDrawable::OnCapture, *uniParam);
}

/**
 * @tc.name: DrawMirror001
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_NE(virtualProcesser->GetCanvas(), nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, psurface);
    params->screenId_ = id;
    ASSERT_EQ(params->GetScreenId(), id);
    int32_t virtualSecLayerOption = screenManager->GetVirtualScreenSecLayerOption(params->GetScreenId());
    ASSERT_EQ(virtualSecLayerOption, 0);

    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    ASSERT_NE(drawable, nullptr);
    params->mirrorSourceDrawable_ = drawable;
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::HAS_SECURITY;
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->specialLayerManager_ = slManager;
    bool hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    ASSERT_EQ(hasSecSurface, true);
    displayDrawable_->DrawMirror(*params, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawMirror002
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    RSRenderThreadParams uniParam;

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_NE(virtualProcesser->GetCanvas(), nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen("virtualScreen02", 480, 320, psurface, 0, 1);
    params->screenId_ = id;
    ASSERT_EQ(params->GetScreenId(), id);
    int32_t virtualSecLayerOption = screenManager->GetVirtualScreenSecLayerOption(params->GetScreenId());
    ASSERT_EQ(virtualSecLayerOption, 1);

    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    ASSERT_NE(drawable, nullptr);
    params->mirrorSourceDrawable_ = drawable;
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::HAS_SECURITY;
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
    ASSERT_NE(mirroredParams, nullptr);
    mirroredParams->specialLayerManager_ = slManager;
    bool hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    ASSERT_EQ(hasSecSurface, true);
    displayDrawable_->DrawMirror(*params, virtualProcesser, &RSLogicalDisplayRenderNodeDrawable::OnCapture, uniParam);
}

/**
 * @tc.name: DrawExpandScreen001
 * @tc.desc: Test DrawExpandScreen when canvas is null
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawExpandScreenTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    NodeId id = 1;
    auto displayRenderParams = std::make_shared<RSLogicalDisplayRenderParams>(id);
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    EXPECT_EQ(displayDrawable_->GetCacheImageByCapture(), nullptr);
}

/**
 * @tc.name: DrawExpandScreen002
 * @tc.desc: Test DrawExpandScreen 
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawExpandScreenTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    NodeId id = 1;
    auto displayRenderParams = std::make_shared<RSLogicalDisplayRenderParams>(id);
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    EXPECT_EQ(displayDrawable_->GetCacheImageByCapture(), nullptr);
}

/**
 * @tc.name: DrawExpandScreen003
 * @tc.desc: Test DrawExpandScreen when targetSurfaceRenderNodeDrawable not null
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawExpandScreenTest003, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    NodeId id = 1;
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    auto surface = std::make_shared<Drawing::Surface>();
    virtualProcesser->canvas_->surface_ = surface.get();
    auto renderNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(renderNode, nullptr);
    auto surfaceRenderNodeDrawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    ASSERT_NE(surfaceRenderNodeDrawable, nullptr);
    auto displayRenderParams = std::make_shared<RSLogicalDisplayRenderParams>(id);
    EXPECT_EQ(displayDrawable_->GetCacheImageByCapture(), nullptr);
}

/**
 * @tc.name: DrawExpandScreen004
 * @tc.desc: Test DrawExpandScreen when hasMirrorDisplay is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawExpandScreenTest004, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    NodeId id = 1;
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    virtualProcesser->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    auto displayRenderParams = std::make_shared<RSLogicalDisplayRenderParams>(id);
    EXPECT_EQ(displayDrawable_->GetCacheImageByCapture(), nullptr);
}

/**
 * @tc.name: ClearCanvasStencilTest001
 * @tc.desc: Test ClearCanvasStencilTest
 * @tc.type: FUNC
 * @tc.require: #IBO35Y
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ClearCanvasStencilTest001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    ASSERT_NE(uniParam, nullptr);

    ASSERT_NE(displayDrawable_, nullptr);

    auto dirtyManager = displayDrawable_->GetSyncDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    Occlusion::Region allDirtyRegion{ Occlusion::Rect{ dirtyManager->GetDirtyRegion() } };
    auto alignedRegion = allDirtyRegion.GetAlignedRegion(128);
    ASSERT_TRUE(alignedRegion.IsEmpty());
}

/**
 * @tc.name: WiredScreenProjection
 * @tc.desc: Test WiredScreenProjection
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, WiredScreenProjectionTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
}

/**
 * @tc.name: SkipFrameByIntervalTest001
 * @tc.desc: test SkipFrameByInterval for refreshRate 0 and skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest001, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByIntervalTest002
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest002, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByIntervalTest003
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 1
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest003, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByIntervalTest004
 * @tc.desc: test SkipFrameByInterval for time within skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest004, TestSize.Level1)
{
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SkipFrameByIntervalTest005
 * @tc.desc: test SkipFrameByInterval for time over skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest005, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByIntervalTest006
 * @tc.desc: test SkipFrameByInterval for time within skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest006, TestSize.Level1)
{
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SkipFrameByIntervalTest007
 * @tc.desc: test SkipFrameByInterval for time over skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest007, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByRefreshRateTest001
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest001, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByRefreshRateTest002
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate UINT32_MAX
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest002, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByRefreshRateTest003
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 60
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest003, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameByRefreshRateTest004
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 60
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByRefreshRateTest004, TestSize.Level1)
{
    ASSERT_FALSE(false);
}

/**
 * @tc.name: SkipFrameTest001
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_INTERVAL
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameTest001, TestSize.Level1)
{
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SkipFrameTest002
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameTest002, TestSize.Level1)
{
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest001
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 1hz
 *                  3. result: actual refresh rate is about 1hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest001, TestSize.Level1)
{
     

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 1; // expectedRefreshRate 1
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest002
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 5hz
 *                  3. result: actual refresh rate is about 5hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest002, TestSize.Level1)
{
     

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 5; // expectedRefreshRate 5
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest003
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 15hz
 *                  3. result: actual refresh rate is about 15hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest003, TestSize.Level1)
{
     

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 15; // expectedRefreshRate 15
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest004
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 20hz
 *                  3. result: actual refresh rate is about 20hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest004, TestSize.Level1)
{
     

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 20; // expectedRefreshRate 20
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest005
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 25hz
 *                  3. result: actual refresh rate is about 25hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest005, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 25; // expectedRefreshRate 25
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest006
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 30hz
 *                  3. result: actual refresh rate is about 30hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest006, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 30; // expectedRefreshRate 30
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest007
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 40hz
 *                  3. result: actual refresh rate is about 40hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest007, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 40; // expectedRefreshRate 40
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest008
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 45hz
 *                  3. result: actual refresh rate is about 45hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest008, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 45; // expectedRefreshRate 45
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest009
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 55hz
 *                  3. result: actual refresh rate is about 55hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest009, TestSize.Level1)
{
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 55; // expectedRefreshRate 55
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest010
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 0hz
 *                  3. result: 0hz is not allowed, the actual refresh rate will be 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest010, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 0; // expectedRefreshRate 0
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest011
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 60hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest011, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 60; // expectedRefreshRate 60
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest012
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 61hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest012, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 61; // expectedRefreshRate 61
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest013
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- UINT32_MAX hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest013, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = UINT32_MAX; // expectedRefreshRate UINT32_MAX
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest014
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- UINT32_MAX+1 hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest014, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    int actualRefreshRateCount = 0;
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: GetSpecialLayerType
 * @tc.desc: Test GetSpecialLayerType
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, GetSpecialLayerType, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    int32_t result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 0);
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 2);

    displayDrawable_->currentBlackList_.insert(0);
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);
    RSUniRenderThread::Instance().whiteList_.insert(0);
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);

    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    result = displayDrawable_->GetSpecialLayerType(*params);
    ASSERT_EQ(result, 1);
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    displayDrawable_->currentBlackList_.clear();
    RSUniRenderThread::Instance().whiteList_.clear();
}

/**
 * @tc.name: RotateMirrorCanvas
 * @tc.desc: Test RotateMirrorCanvas
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, RotateMirrorCanvas, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ScreenRotation rotation = ScreenRotation::ROTATION_0;
    displayDrawable_->RotateMirrorCanvas(rotation, 1.f, 1.f);

    rotation = ScreenRotation::ROTATION_90;
    displayDrawable_->RotateMirrorCanvas(rotation, 1.f, -1.f);

    rotation = ScreenRotation::ROTATION_180;
    displayDrawable_->RotateMirrorCanvas(rotation, 2.f, 2.f);

    rotation = ScreenRotation::ROTATION_270;
    displayDrawable_->RotateMirrorCanvas(rotation, -1.f, 1.f);
    ASSERT_TRUE(displayDrawable_->curCanvas_);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    std::shared_ptr<RSLogicalDisplayRenderNode> renderNode;
    RSRenderNodeDrawableAdapter* drawable = nullptr;
    RSLogicalDisplayRenderNodeDrawable* mirroredDrawable = nullptr;
    RSDisplayNodeConfig config;
    renderNode = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID + 2, config);
    drawable = RSLogicalDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (drawable) {
        mirroredDrawable = static_cast<RSLogicalDisplayRenderNodeDrawable*>(drawable);
    }
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(mirroredDrawable->GetRenderParams());

    if (drawable) {
        mirroredDrawable->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id);
    }
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_TRUE(mirroredDrawable->GetRenderParams());
    ASSERT_TRUE(displayDrawable_->GetRenderParams());

    auto screenManagerPtr = impl::RSScreenManager::GetInstance();
    auto* screenManager = static_cast<impl::RSScreenManager*>(screenManagerPtr.GetRefPtr());
    VirtualScreenConfigs configs;
    auto screen = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(screenManager->screens_.empty());

    screenManager->screens_.clear();
    auto screenPtr = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    screenPtr->screenRotation_ = ScreenRotation::ROTATION_90;
    screenManager->screens_.insert(std::make_pair(screenPtr->id_, std::move(screenPtr)));
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(screenManager->screens_.empty());

    screenManager->screens_.clear();
    auto managerPtr = std::make_unique<OHOS::Rosen::impl::RSScreen>(configs);
    managerPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    screenManager->screens_.insert(std::make_pair(managerPtr->id_, std::move(managerPtr)));
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    mirroredDrawable->renderParams_ = nullptr;
    displayDrawable_->ScaleAndRotateMirrorForWiredScreen(*mirroredDrawable);
    ASSERT_FALSE(mirroredDrawable->renderParams_);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen002
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while scale of width smaller
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen002, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);

    ASSERT_NE(mirroredDrawable_, nullptr);
    auto mirroredParams = mirroredDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mirroredParams, nullptr);
    
    // sacle mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen003
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while scale of height smaller
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen003, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);

    ASSERT_NE(mirroredDrawable_, nullptr);
    auto mirroredParams = mirroredDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mirroredParams, nullptr);
    
    // sacle mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
}

/**
 * @tc.name: ScaleAndRotateMirrorForWiredScreen004
 * @tc.desc: Test ScaleAndRotateMirrorForWiredScreen while mainHeight / mainWidth invalid
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, ScaleAndRotateMirrorForWiredScreen004, TestSize.Level2)
{
    // set drawable screenInfo
    ASSERT_NE(displayDrawable_, nullptr);
    auto mainParams = displayDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mainParams, nullptr);

    ASSERT_NE(mirroredDrawable_, nullptr);
    auto mirroredParams = mirroredDrawable_->GetRenderParams() ?
        static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable_->GetRenderParams().get()) : nullptr;
    ASSERT_NE(mirroredParams, nullptr);
    
    // sacle mirror screen
    mirroredDisplayDrawable_->ScaleAndRotateMirrorForWiredScreen(*displayDrawable_);
    ASSERT_NE(mirroredDisplayDrawable_->curCanvas_, nullptr);
    ASSERT_EQ(mirroredDisplayDrawable_->curCanvas_->GetTotalMatrix().Get(Drawing::Matrix::SCALE_X), 1);
}

/**
 * @tc.name: DrawMirrorCopy
 * @tc.desc: Test DrawMirrorCopy
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawMirrorCopy, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);
    std::shared_ptr<RSLogicalDisplayRenderNode> renderNode;
    RSRenderNodeDrawableAdapter* drawable = nullptr;
    RSLogicalDisplayRenderNodeDrawable* mirrorDrawable = nullptr;
    RSDisplayNodeConfig config;
    renderNode = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID + 2, config);
    drawable = RSLogicalDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (drawable) {
        mirrorDrawable = static_cast<RSLogicalDisplayRenderNodeDrawable*>(drawable);
        mirrorDrawable->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id);
    }
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    ASSERT_TRUE(uniParam->IsVirtualDirtyEnabled());

    uniParam->isVirtualDirtyEnabled_ = false;
    virtualProcesser->renderFrame_ = std::make_unique<RSRenderFrame>(nullptr, nullptr);

    Drawing::Canvas drawingCanvas;
    virtualProcesser->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    ASSERT_TRUE(virtualProcesser->GetCanvas());
    ASSERT_TRUE(virtualProcesser->GetCanvas());
    uniParam->isVirtualDirtyEnabled_ = true;
    sleep(1);
}

/**
 * @tc.name: DrawHardwareEnabledNodes001
 * @tc.desc: Test DrawHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawHardwareEnabledNodes001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    NodeId id = 1;
    auto rsSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(rsSurfaceNode);
    id = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::INVERT_COLOR_DISABLE_MODE;
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);

    RSUniRenderThread::Instance().uniRenderEngine_->colorFilterMode_ = ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE;
    displayDrawable_->DrawHardwareEnabledNodes(canvas, *params);
    ASSERT_TRUE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: DrawHardwareEnabledNodes002
 * @tc.desc: Test DrawHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawHardwareEnabledNodes002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: DrawHardwareEnabledNodesMissedInCacheImage
 * @tc.desc: Test DrawHardwareEnabledNodesMissedInCacheImage
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawHardwareEnabledNodesMissedInCacheImage, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: SwitchColorFilter
 * @tc.desc: Test SwitchColorFilter
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SwitchColorFilter, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    ASSERT_TRUE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: SwitchColorFilterWithP3
 * @tc.desc: Test SwitchColorFilterWithP3
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SwitchColorFilterWithP3, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    Drawing::Canvas drawingCanvas(100, 100);
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surface = std::make_shared<Drawing::Surface>();

    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, bitmapFormat);
    surface->Bind(bitmap);
    canvas.surface_ = surface.get();

    ASSERT_FALSE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();

    ASSERT_TRUE(RSUniRenderThread::Instance().GetRenderEngine());
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: FindHardwareEnabledNodes
 * @tc.desc: Test FindHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FindHardwareEnabledNodes, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->renderParams_.get());
    ASSERT_NE(params, nullptr);
    ASSERT_EQ(RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.size(), 2);

    NodeId nodeId = 1;
    auto renderNode = std::make_shared<RSRenderNode>(nodeId);
    auto drawablePtr = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    EXPECT_NE(drawablePtr, nullptr);

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
}

/**
 * @tc.name: MakeBrightnessAdjustmentShader
 * @tc.desc: Test MakeBrightnessAdjustmentShader
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, MakeBrightnessAdjustmentShader, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, FinishOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: CreateSurface
 * @tc.desc: Test CreateSurface
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, CreateSurface, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: SkipFrameByInterval
 * @tc.desc: Test SkipFrameByInterval
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SkipFrameByInterval, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: EnablescRGBForP3AndUiFirstTest
 * @tc.desc: Test ScRGB For P3 Controller
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, EnablescRGBForP3AndUiFirstTest, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
}

HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWiredMirrorOnDraw, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredDisplayDrawable_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *params);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    displayDrawable_->DrawWiredMirrorOnDraw(*mirroredDisplayDrawable_, *params);
}

/**
 * @tc.name: SetSecurityMaskTest
 * @tc.desc: Test SetSecurityMask
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, SetSecurityMaskTest, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->SetSecurityMask(*virtualProcesser);
}

/**
 * @tc.name: UpdateSurfaceDrawRegion
 * @tc.desc: Test UpdateSurfaceDrawRegion
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, UpdateSurfaceDrawRegion, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawWatermarkIfNeed001
 * @tc.desc: Test DrawWatermarkIfNeed001
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSLogicalDisplayRenderNodeDrawableTest, DrawWatermarkIfNeed001, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 400;
    uint32_t height = 400;
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto screenId = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, screenId);
    screenManager->SetDefaultScreenId(screenId);

    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_TRUE(params);
    // Create waterMask pixelMap
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.editable = true;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);

    ASSERT_TRUE(pixelMap != nullptr);
    auto img = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    ASSERT_TRUE(img != nullptr);

    // Create canvas
    auto rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->SetWatermark(true, img);
    RSUniRenderThread::Instance().Sync(std::move(rsRenderThreadParams));

    Drawing::Canvas drawingCanvas(400, 400);
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto screen = static_cast<impl::RSScreenManager*>(screenManager.GetRefPtr())->GetScreen(screenId);
    auto screenRawPtr = static_cast<impl::RSScreen*>(screen.get());
    ASSERT_TRUE(screenRawPtr != nullptr);

    // Test0 ScreenCorrection = INVALID_SCREEN_ROTATION && ScreenRotation = ROTATION_0
    params->screenRotation_ = ScreenRotation::ROTATION_0;
    screenRawPtr->screenRotation_ = ScreenRotation::INVALID_SCREEN_ROTATION;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Test1 ScreenCorrection = ROTATION_0 && ScreenRotation = ROTATION_180
    params->screenRotation_ = ScreenRotation::ROTATION_180;
    screenRawPtr->screenRotation_ = ScreenRotation::ROTATION_0;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Test2 ScreenCorrection = ROTATION_0 && ScreenRotation = ROTATION_90
    params->screenRotation_ = ScreenRotation::ROTATION_90;
    screenRawPtr->screenRotation_ = ScreenRotation::ROTATION_0;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Test3 ScreenCorrection = ROTATION_270 && ScreenRotation = ROTATION_180
    params->screenRotation_ = ScreenRotation::ROTATION_180;
    screenRawPtr->screenRotation_ = ScreenRotation::ROTATION_270;
    displayDrawable_->DrawWatermarkIfNeed(canvas);

    // Reset
    rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->SetWatermark(false, nullptr);
    RSUniRenderThread::Instance().Sync(std::move(rsRenderThreadParams));
}
}
