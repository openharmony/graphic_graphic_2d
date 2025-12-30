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
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature_param/performance_feature/rotateoffscreen_param.h"
#include "params/rs_effect_render_params.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSSurfaceRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSSurfaceRenderNodeDrawable* surfaceDrawable_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    static inline Occlusion::Rect DEFAULT_RECT{0, 80, 500, 500};
};

void RSSurfaceRenderNodeDrawableTest::SetUpTestCase() {}
void RSSurfaceRenderNodeDrawableTest::TearDownTestCase() {}
void RSSurfaceRenderNodeDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    if (!renderNode_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create surface node.");
    }
    drawable_ = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode_);
    if (drawable_) {
        drawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID);
        surfaceDrawable_ = static_cast<RSSurfaceRenderNodeDrawable*>(drawable_);
        if (!drawable_->renderParams_) {
            RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init render params.");
        }
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSSurfaceRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateSurfaceRenderNodeDrawableTest
 * @tc.desc: Test If SurfaceRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CreateSurfaceRenderNodeDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);

    const RSSurfaceRenderNodeConfig config = {.id = id, .surfaceWindowType = SurfaceWindowType::SCB_SCREEN_LOCK};
    renderNode = std::make_shared<RSSurfaceRenderNode>(config);
    drawable = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: GetWhiteListPersistentId001
 * @tc.desc: Test GetWhiteListPersistentId  while node in whitelist
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetWhiteListPersistentId001, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->leashPersistentId_ = surfaceDrawable_->nodeId_;

    std::unordered_set<NodeId> whiteList = {surfaceParams->leashPersistentId_};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);
    ASSERT_NE(surfaceDrawable_->GetWhiteListPersistentId(*surfaceParams, *canvas_), INVALID_LEASH_PERSISTENTID);

    // restore
    RSUniRenderThread::Instance().SetWhiteList({});
}

/**
 * @tc.name: OnDraw001
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    // default case, shouldpaint == false
    ASSERT_NE(drawable_->renderParams_, nullptr);
    surfaceDrawable_->OnDraw(*drawingCanvas_);
    // if should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnDraw(*drawingCanvas_);
}

/**
 * @tc.name: OnDraw002
 * @tc.desc: Test OnDraw when CheckIfSurfaceSkipInMirrorOrScreenshot return false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(false);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
    surfaceDrawable_->OnDraw(*drawingCanvas_);
}

/**
 * @tc.name: OnDraw003
 * @tc.desc: Test OnDraw when CheckIfSurfaceSkipInMirrorOrScreenshot return true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    std::unordered_set<NodeId> blackList = {surfaceParams->GetId()};
    RSUniRenderThread::Instance().SetBlackList(blackList);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
    surfaceDrawable_->OnDraw(*drawingCanvas_);
    RSUniRenderThread::Instance().SetBlackList({});
}

/**
 * @tc.name: OnCapture001
 * @tc.desc: Test OnCapture
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnCapture001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    // default, should not paint
    surfaceDrawable_->OnCapture(*canvas_);
    // should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnCapture(*canvas_);
}

/**
 * @tc.name: OnCapture002
 * @tc.desc: Test OnCapture  when CheckIfSurfaceSkipInMirrorOrScreenshot return false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
 HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnCapture002, TestSize.Level1)
 {
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(false);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
    surfaceDrawable_->OnCapture(*canvas_);
}

/**
 * @tc.name: OnCapture003
 * @tc.desc: Test OnCapture when CheckIfSurfaceSkipInMirrorOrScreenshot return true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnCapture003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    std::unordered_set<NodeId> blackList = {surfaceParams->GetId()};
    RSUniRenderThread::Instance().SetBlackList(blackList);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
    surfaceDrawable_->OnCapture(*canvas_);
    RSUniRenderThread::Instance().SetBlackList({});
}

/**
 * @tc.name: CaptureSurface001
 * @tc.desc: Test CaptureSurface, default case
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    surfaceParams->isSpherizeValid_ = false;
    surfaceParams->isAttractionValid_ = false;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceDrawable_->GetRsSubThreadCache().hasHdrPresent_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    ASSERT_TRUE(!surfaceParams->IsAttractionValid());

    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::PROTECTED, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::SKIP, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::SNAPSHOT_SKIP, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::SECURITY, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SNAPSHOT_SKIP));
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * @tc.name: DealWithHdr
 * @tc.desc: Test DealWithHdr
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithHdr, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->DealWithHdr(*renderNode_, *surfaceParams);
}
#endif

/**
 * @tc.name: CaptureSurface002
 * @tc.desc: Test CaptureSurface, special case: security/protected layer.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
}

/**
 * @tc.name: CaptureSurface003
 * @tc.desc: Test CaptureSurface, special layer cases for screen mirrorring.
 * @tc.type: FUNC
 * @tc.require: #IBNN3I
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED));

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

/**
 * @tc.name: CaptureSurface004
 * @tc.desc: Test CaptureSurface, special layer cases for capture single surface.
 * @tc.type: FUNC
 * @tc.require: #IBNN3I
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    CaptureParam captureParam;
    captureParam.isSingleSurface_ = true;
    RSUniRenderThread::SetCaptureParam(captureParam);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));

    captureParam.isNeedBlur_ = true;
    RSUniRenderThread::SetCaptureParam(captureParam);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}


/**
 * @tc.name: CaptureSurface005
 * @tc.desc: Test CaptureSurface, special layer cases for screenshot.
 * @tc.type: FUNC
 * @tc.require: #IBNN3I
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface005, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    CaptureParam captureParam;
    captureParam.isSnapshot_ = true;
    captureParam.isSingleSurface_ = false;
    RSUniRenderThread::SetCaptureParam(captureParam);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

/**
 * @tc.name: CaptureSurface006
 * @tc.desc: Test CaptureSurface
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface006, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    RSSpecialLayerManager slManager;
    surfaceParams->specialLayerManager_ = slManager;
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);

    CaptureParam captureParam;
    captureParam.isMirror_ = true;
    RSUniRenderThread::SetCaptureParam(captureParam);

    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);
    uniParams->isSecurityExemption_ = false;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    captureParam.isMirror_ = false;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
}

/**
 * @tc.name: CaptureSurface008
 * @tc.desc: Test sub window has a blacklist in UIFirst scenario
 * @tc.type: FUNC
 * @tc.require: issueICHZO3
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface008, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);
    RSSpecialLayerManager slManager;
    surfaceParams->specialLayerManager_ = slManager;

    auto virtualScreenId = 1;
    CaptureParam captureParam;
    captureParam.isMirror_ = true;
    captureParam.virtualScreenId_ = virtualScreenId;
    RSUniRenderThread::SetCaptureParam(captureParam);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->GetMultableSpecialLayerMgr().SetWithScreen(virtualScreenId, SpecialLayerType::HAS_BLACK_LIST, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().FindWithScreen(virtualScreenId, SpecialLayerType::HAS_BLACK_LIST));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->uiFirstFlag_ = MultiThreadCacheType::LEASH_WINDOW;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    RSUniRenderThread::GetCaptureParam().isMirror_ = false;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->GetMultableSpecialLayerMgr().SetWithScreen(virtualScreenId, SpecialLayerType::HAS_BLACK_LIST, false);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->uiFirstFlag_ = MultiThreadCacheType::NONE;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->GetMultableSpecialLayerMgr().SetWithScreen(virtualScreenId, SpecialLayerType::HAS_BLACK_LIST, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().FindWithScreen(virtualScreenId, SpecialLayerType::HAS_BLACK_LIST));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->GetMultableSpecialLayerMgr().ClearScreenSpecialLayer();
}

/**
 * @tc.name: CaptureSurface009
 * @tc.desc: Test CaptureSurface for ignore special layer
 * @tc.type: FUNC
 * @tc.require: issueICUQ08
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface009, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);
    uniParams->SetSecExemption(true);
    RSSpecialLayerManager slManager;
    surfaceParams->specialLayerManager_ = slManager;
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);

    CaptureParam captureParam1;
    captureParam1.isSingleSurface_ = false;
    captureParam1.needCaptureSpecialLayer_ = false;
    RSUniRenderThread::SetCaptureParam(captureParam1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    CaptureParam captureParam2;
    captureParam2.isSingleSurface_ = false;
    captureParam2.needCaptureSpecialLayer_ = true;
    RSUniRenderThread::SetCaptureParam(captureParam2);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    EXPECT_TRUE(uniParams->GetSecExemption());
}

/**
 * @tc.name: CrossDisplaySurfaceDirtyRegionConversion
 * @tc.desc: Test CrossDisplaySurfaceDirtyRegionConversion, if node is cross-display, the surface dirty will be offset.
 * @tc.type: FUNC
 * @tc.require: issueIB670G
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CrossDisplaySurfaceDirtyRegionConversion_001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(surfaceDrawable_->GetRenderParams(), nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);

    NodeId curDisplayId = 1;
    Drawing::Matrix matrix;
    matrix.PreTranslate(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    surfaceParams->crossNodeSkipDisplayConversionMatrices_.insert({curDisplayId, matrix});
    uniParams->SetCurrentVisitDisplayDrawableId(curDisplayId);
    RectI surfaceDirtyRect = { DEFAULT_RECT.left_, DEFAULT_RECT.top_,
        DEFAULT_RECT.right_ - DEFAULT_RECT.left_, DEFAULT_RECT.bottom_ - DEFAULT_RECT.top_ };;

    // if surface node is not cross-display node, nothing will happen.
    surfaceDrawable_->GetRenderParams()->SetFirstLevelCrossNode(false);
    surfaceDrawable_->CrossDisplaySurfaceDirtyRegionConversion(*uniParams, *surfaceParams, surfaceDirtyRect);
    ASSERT_EQ(surfaceDirtyRect.GetTop(), DEFAULT_RECT.top_);
    // if surface node is not cross-display node, the surface dirty region will be offset.
    surfaceDrawable_->GetRenderParams()->SetFirstLevelCrossNode(true);
    surfaceDrawable_->CrossDisplaySurfaceDirtyRegionConversion(*uniParams, *surfaceParams, surfaceDirtyRect);
    ASSERT_NE(surfaceDirtyRect.GetTop(), DEFAULT_RECT.top_);
}

/**
 * @tc.name: CalculateVisibleDirtyRegion
 * @tc.desc: Test CalculateVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CalculateVisibleDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    surfaceParams->SetWindowInfo(false, true, false);
    Drawing::Region result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, true);
    ASSERT_FALSE(result.IsEmpty());

    surfaceParams->windowInfo_.isMainWindowType_ = true;
    surfaceParams->windowInfo_.isLeashWindow_ = true;
    surfaceParams->windowInfo_.isAppWindow_ = false;
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, true);
    ASSERT_FALSE(result.IsEmpty());

    surfaceParams->windowInfo_.isMainWindowType_ = false;
    surfaceParams->windowInfo_.isLeashWindow_ = false;
    surfaceParams->windowInfo_.isAppWindow_ = false;
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, true);
    ASSERT_TRUE(result.IsEmpty());

    surfaceParams->SetWindowInfo(true, false, false);
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, true);
    ASSERT_FALSE(result.IsEmpty());

    Occlusion::Region region;
    surfaceParams->SetVisibleRegion(region);
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());

    Occlusion::Region region1(DEFAULT_RECT);
    surfaceParams->SetVisibleRegion(region1);
    surfaceDrawable_->globalDirtyRegion_ = region1;
    surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, false);
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, PrepareOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    surfaceDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());

    surfaceDrawable_->curCanvas_->surface_ = nullptr;
    ASSERT_FALSE(surfaceDrawable_->PrepareOffscreenRender());
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, PrepareOffscreenRenderTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    surfaceDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());
    ASSERT_TRUE(surfaceDrawable_->curCanvas_->GetSurface());
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, PrepareOffscreenRenderTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    surfaceDrawable_->maxRenderSize_ = 200; // for test
    surfaceDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());
    ASSERT_TRUE(surfaceDrawable_->curCanvas_->GetSurface());
}

/**
 * @tc.name: IsHardwareEnabled
 * @tc.desc: Test IsHardwareEnabled
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, IsHardwareEnabled, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());

    auto nodePtr = std::make_shared<RSRenderNode>(0);
    ASSERT_NE(nodePtr, nullptr);
    auto rsSurfaceRenderNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(nodePtr);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(
        std::make_tuple(0, 0, rsSurfaceRenderNode));
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());

    auto rsRenderNode = std::make_shared<RSRenderNode>(0);
    ASSERT_NE(rsRenderNode, nullptr);
    auto surfaceRenderNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsRenderNode);
    surfaceRenderNode->renderParams_ = std::make_unique<RSRenderParams>(0);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(
        std::make_tuple(0, 0, surfaceRenderNode));
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, FinishOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::SamplingOptions samping;
    surfaceDrawable_->FinishOffscreenRender(samping);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas backupCanvas(&canvas);
    surfaceDrawable_->canvasBackup_ = &backupCanvas;

    Drawing::Canvas canvas2;
    RSPaintFilterCanvas curCanvas(&canvas2);
    surfaceDrawable_->curCanvas_ = &curCanvas;
    surfaceDrawable_->curCanvas_->Save();
    surfaceDrawable_->offscreenSurface_ = Drawing::Surface::MakeRasterN32Premul(100, 100);
    surfaceDrawable_->FinishOffscreenRender(samping);
    ASSERT_NE(surfaceDrawable_->curCanvas_, nullptr);
}

/**
 * @tc.name: GetVisibleDirtyRegion
 * @tc.desc: Test GetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetVisibleDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetVisibleDirtyRegion(region);
    ASSERT_FALSE(surfaceDrawable_->GetVisibleDirtyRegion().IsEmpty());
}

/**
 * @tc.name: SetVisibleDirtyRegion
 * @tc.desc: Test SetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetVisibleDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetVisibleDirtyRegion(region);
    ASSERT_FALSE(surfaceDrawable_->GetVisibleDirtyRegion().IsEmpty());

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetVisibleDirtyRegion(region);
}

/**
 * @tc.name: SetAlignedVisibleDirtyRegion
 * @tc.desc: Test SetAlignedVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetAlignedVisibleDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetAlignedVisibleDirtyRegion(region);

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetAlignedVisibleDirtyRegion(region);
}

/**
 * @tc.name: SetGlobalDirtyRegion
 * @tc.desc: Test SetGlobalDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetGlobalDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region rect;
    surfaceDrawable_->SetGlobalDirtyRegion(rect);

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetGlobalDirtyRegion(rect);
}
/**
 * @tc.name: SetDirtyRegionBelowCurrentLayer
 * @tc.desc: Test SetDirtyRegionBelowCurrentLayer
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetDirtyRegionBelowCurrentLayer, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);

    Occlusion::Region region;
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(region);

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(region);
}

/**
 * @tc.name: GetSyncDirtyManager
 * @tc.desc: Test GetSyncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetSyncDirtyManager, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);

    std::shared_ptr<RSDirtyRegionManager> manager = surfaceDrawable_->GetSyncDirtyManager();
    ASSERT_NE(manager, nullptr);
}

/**
 * @tc.name: GetSyncDirtyManager
 * @tc.desc: Test GetSyncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetSyncDirtyManagerTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    drawable->GetSyncDirtyManager();
    renderNode = nullptr;
    drawable->GetSyncDirtyManager();
}

/**
 * @tc.name: SetDirtyRegionBelowCurrentLayer
 * @tc.desc: Test SetDirtyRegionBelowCurrentLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetDirtyRegionBelowCurrentLayerTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    Occlusion::Region VisibleDirtyRegion;
    drawable->SetDirtyRegionBelowCurrentLayer(VisibleDirtyRegion);
    renderNode = nullptr;
    drawable->SetDirtyRegionBelowCurrentLayer(VisibleDirtyRegion);
}

/**
 * @tc.name: SetDirtyRegionAlignedEnable
 * @tc.desc: Test SetDirtyRegionAlignedEnable
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetDirtyRegionAlignedEnableTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    drawable->SetDirtyRegionAlignedEnable(true);
    renderNode = nullptr;
    drawable->SetDirtyRegionAlignedEnable(true);
}

/**
 * @tc.name: SetGlobalDirtyRegion
 * @tc.desc: Test SetGlobalDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetGlobalDirtyRegionTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    const Occlusion::Region rect{Occlusion::Rect(0, 0, 100, 100)};
    drawable->SetGlobalDirtyRegion(rect);
    renderNode = nullptr;
    drawable->SetGlobalDirtyRegion(rect);
}

/**
 * @tc.name: GetVisibleDirtyRegion
 * @tc.desc: Test GetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetVisibleDirtyRegionTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    drawable->GetVisibleDirtyRegion();
    renderNode = nullptr;
    drawable->GetVisibleDirtyRegion();
}

/**
 * @tc.name: EnableGpuOverDrawDrawBufferOptimization
 * @tc.desc: Test EnableGpuOverDrawDrawBufferOptimization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, EnableGpuOverDrawDrawBufferOptimizationTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    surfaceDrawable_->EnableGpuOverDrawDrawBufferOptimization(drawingCanvas, surfaceParams);

    Vector4f vct(1.f, 1.f, 1.f, 1.f); // for test
    surfaceParams->overDrawBufferNodeCornerRadius_ = vct;
    surfaceDrawable_->EnableGpuOverDrawDrawBufferOptimization(drawingCanvas, surfaceParams);
}

/**
 * @tc.name: HasCornerRadius
 * @tc.desc: Test HasCornerRadius
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, HasCornerRadiusTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    const auto params01 = surfaceParams;
    ASSERT_FALSE(surfaceDrawable_->HasCornerRadius(*params01));

    surfaceParams->rrect_.radius_[0].y_ = 4.f;
    const auto params02 = surfaceParams;
    ASSERT_TRUE(surfaceDrawable_->HasCornerRadius(*params02));

    surfaceParams->rrect_.radius_[0].x_ = 0.f;
    const auto params03 = surfaceParams;
    ASSERT_TRUE(surfaceDrawable_->HasCornerRadius(*params03));
}

/**
 * @tc.name: OnGeneralProcess
 * @tc.desc: Test OnGeneralProcess
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnGeneralProcessTest, TestSize.Level1)
{
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    if (!rtThread.uniRenderEngine_) {
        rtThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    }

    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
    EXPECT_FALSE(surfaceParams->GetBuffer());
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, true);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, true);
    /* RSSurfaceRenderNodeDrawable::DrawMagnificationRegion base case */
    surfaceParams->frameRect_ = {0, 0, 100, 100};
    surfaceParams->regionToBeMagnified_ = {0, 0, 50, 50};
    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, true);
    /* RSSurfaceRenderNodeDrawable::DrawMagnificationRegion abnormal case */
    surfaceParams->frameRect_ = {0, 0, 100, 100};
    surfaceParams->regionToBeMagnified_ = {0, 0, 0, 0};
    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, true);
    /* OnGlobalPositionEnabled is true */
    surfaceParams->isGlobalPositionEnabled_ = true;
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
    // To set matrix is singular matrix
    surfaceParams->matrix_.SetMatrix(1, 2, 3, 2, 4, 6, 3, 6, 9);
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
}

/**
 * @tc.name: OnGeneralProcess_StoppedByRangeCapTest
 * @tc.desc: OnGeneralProcess_StoppedByRangeCapTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnGeneralProcess_StoppedByRangeCapTest, TestSize.Level1)
{
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    if (!rtThread.uniRenderEngine_) {
        rtThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    }

    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    canvas.SetUICapture(true);
    RSUniRenderThread::GetCaptureParam().endNodeId_ = surfaceParams->GetId();
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
    EXPECT_FALSE(surfaceParams->GetBuffer());
    RSUniRenderThread::GetCaptureParam().endNodeId_ = INVALID_NODEID;
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
    EXPECT_FALSE(surfaceParams->GetBuffer());
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot001
 * @tc.desc: Test CheckIfSurfaceSkipInMirrorOrScreenshot for main screen
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot001, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(false);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot002
 * @tc.desc: Test CheckIfSurfaceSkipInMirrorOrScreenshot while don't has white list and black list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot002, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot003
 * @tc.desc: Test CheckIfSurfaceSkipInMirrorOrScreenshot for node in black list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot003, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    std::unordered_set<NodeId> blackList = {surfaceParams->GetId()};
    RSUniRenderThread::Instance().SetBlackList(blackList);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));

    params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(false);
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    RSUniRenderThread::GetCaptureParam().isSingleSurface_ = true;
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    RSUniRenderThread::GetCaptureParam().isSingleSurface_ = false;
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot004
 * @tc.desc: Test CheckIfSurfaceSkipInMirrorOrScreenshot while white list isn't empty and node not in white list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot004, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    std::unordered_set<NodeId> whiteList = {surfaceParams->GetId() + 1};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot006
 * @tc.desc: Test CheckIfSurfaceSkipInMirrorOrScreenshot for security display
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot006, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    params->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    RSUniRenderThread::GetCaptureParam().isSingleSurface_ = true;
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    RSUniRenderThread::GetCaptureParam().isSingleSurface_ = false;
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: SetAndGet
 * @tc.desc: Test SetAndGet
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetAndGetTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Occlusion::Region region = surfaceDrawable_->GetVisibleDirtyRegion();
    EXPECT_TRUE(region.IsEmpty());
    Occlusion::Region OccRegion;
    surfaceDrawable_->SetVisibleDirtyRegion(OccRegion);
    surfaceDrawable_->SetAlignedVisibleDirtyRegion(OccRegion);
    Occlusion::Region rect;
    surfaceDrawable_->SetGlobalDirtyRegion(rect);
    surfaceDrawable_->SetDirtyRegionAlignedEnable(true);
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(OccRegion);
    surfaceDrawable_->GetSyncDirtyManager();
}

/**
 * @tc.name: SetAndGet
 * @tc.desc: Test SetAndGet
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetAndGetTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->renderParams_ = std::make_unique<RSRenderParams>(0);
    Occlusion::Region OccRegion;
    surfaceDrawable_->SetVisibleDirtyRegion(OccRegion);
    Occlusion::Region region = surfaceDrawable_->GetVisibleDirtyRegion();
    EXPECT_TRUE(region.IsEmpty());

    surfaceDrawable_->SetAlignedVisibleDirtyRegion(OccRegion);
    Occlusion::Region rect;
    surfaceDrawable_->SetGlobalDirtyRegion(rect);
    surfaceDrawable_->SetDirtyRegionAlignedEnable(true);
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(OccRegion);
    ASSERT_NE(surfaceDrawable_->GetSyncDirtyManager(), nullptr);
}

/**
 * @tc.name: DrawSelfDrawingNodeBuffer
 * @tc.desc: Test DrawSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAOJHQ
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawSelfDrawingNodeBufferTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    BufferDrawParam params;
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);

    Color color(255, 255, 255);
    surfaceParams->backgroundColor_ = color;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    surfaceParams->rrect_.radius_[0].x_ = 1.f;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    surfaceParams->selfDrawingType_ = SelfDrawingNodeType::VIDEO;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    ASSERT_FALSE(surfaceParams->GetHardwareEnabled());
}

/**
 * @tc.name: DealWithSelfDrawingNodeBuffer
 * @tc.desc: Test DealWithSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAOJHQ
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithSelfDrawingNodeBufferTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    surfaceParams->isInFixedRotation_ = false;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_FALSE(surfaceParams->GetHardwareEnabled());
    ASSERT_FALSE(surfaceParams->IsInFixedRotation());

    surfaceParams->isInFixedRotation_ = true;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    surfaceParams->isHardwareEnabled_ = true;
    surfaceDrawable_->name_ = "test";
    surfaceDrawable_->surfaceNodeType_ = RSSurfaceNodeType::DEFAULT;
    surfaceParams->isLayerTop_ = false;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_TRUE(surfaceParams->GetHardwareEnabled());

    surfaceParams->isHardCursor_ = true;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_TRUE(surfaceParams->GetHardCursorStatus());
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabledTopSurface());

    surfaceParams->isLayerTop_ = true;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_TRUE(surfaceParams->IsLayerTop());
    surfaceDrawable_->surfaceNodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    surfaceDrawable_->name_ = "pointer window";
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
}

/**
 * @tc.name: DrawCloneNode
 * @tc.desc: Test DrawCloneNode while node is not clone
 * @tc.type: FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawCloneNode, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->isCloneNode_ = false;
    RSRenderThreadParams uniParams;
    auto result = surfaceDrawable_->DrawCloneNode(canvas, uniParams, *surfaceParams, false);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: DrawCloneNode
 * @tc.desc: Test DrawCloneNode001 while node is clone
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawCloneNode001, TestSize.Level1)
{
    auto cloneRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(cloneRenderNode, nullptr);
    auto cloneRenderNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(cloneRenderNode));
    ASSERT_NE(cloneRenderNodeDrawable, nullptr);

    auto clonedRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(clonedRenderNode, nullptr);
    auto clonedRenderNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(clonedRenderNode));
    ASSERT_NE(clonedRenderNodeDrawable, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(cloneRenderNodeDrawable.get());
    ASSERT_NE(surfaceDrawable, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(cloneRenderNodeDrawable->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    surfaceParams->isCloneNode_ = true;
    surfaceParams->isClonedNodeOnTheTree_ = false;
    RSRenderThreadParams uniParams;
    auto result = surfaceDrawable->DrawCloneNode(canvas, uniParams, *surfaceParams, false);
    ASSERT_FALSE(result);

    surfaceParams->isClonedNodeOnTheTree_ = true;
    result = surfaceDrawable->DrawCloneNode(canvas, uniParams, *surfaceParams, false);
    ASSERT_FALSE(result);

    DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr clonedNodeRenderDrawableSharedPtr(clonedRenderNodeDrawable);
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr clonedNodeRenderDrawableWeakPtr(clonedNodeRenderDrawableSharedPtr);
    surfaceParams->clonedNodeRenderDrawable_ = clonedNodeRenderDrawableWeakPtr;
    result = surfaceDrawable->DrawCloneNode(canvas, uniParams, *surfaceParams, true);
    ASSERT_TRUE(result);
    clonedRenderNodeDrawable->renderParams_ = nullptr;
    result = surfaceDrawable->DrawCloneNode(canvas, uniParams, *surfaceParams, true);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CheckDrawAndCacheWindowContentTest
 * @tc.desc: Test OnGeneralProcessAndCache
 * @tc.type: FUNC
 * @tc.require: issueB2YOV
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckDrawAndCacheWindowContentTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetNeedCacheSurface(false);
    surfaceParams->isCrossNode_ = false;

    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);

    uniParams->SetIsFirstVisitCrossNodeDisplay(false);
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    surfaceParams->SetNeedCacheSurface(true);
    ASSERT_TRUE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    surfaceParams->isCrossNode_ = true;
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    uniParams->SetIsFirstVisitCrossNodeDisplay(true);
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    RSUniRenderThread::captureParam_.isSnapshot_ = true;
    ASSERT_TRUE(RSUniRenderThread::IsInCaptureProcess());
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));
}

/**
 * @tc.name: OnGeneralProcessAndCache
 * @tc.desc: Test OnGeneralProcessAndCache
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnGeneralProcessAndCache, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetFrameRect({0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE});
    surfaceParams->SetBoundsRect({0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE});
    surfaceParams->SetNeedCacheSurface(true);
    std::shared_ptr<Drawing::Surface> surface =
        Drawing::Surface::MakeRasterN32Premul(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas canvas(surface.get());
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
    ASSERT_TRUE(surfaceDrawable_->GetRsSubThreadCache().GetRSDrawWindowCache().HasCache());
}

/**
 * @tc.name: DrawCacheImageForMultiScreenView001
 * @tc.desc: Test DrawCacheImageForMultiScreenView
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawCacheImageForMultiScreenView001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    ASSERT_FALSE(surfaceDrawable_->DrawCacheImageForMultiScreenView(*canvas_, *surfaceParams));
}

/**
 * @tc.name: DrawCacheImageForMultiScreenView002
 * @tc.desc: Test DrawCacheImageForMultiScreenView
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawCacheImageForMultiScreenView002, TestSize.Level1)
{
    NodeId id = 1;
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto screenRenderNodeDrawable = std::make_shared<RSScreenRenderNodeDrawable>(renderNode);
    screenRenderNodeDrawable->cachedImageByCapture_ = nullptr;
    surfaceParams->sourceDisplayRenderNodeDrawable_ = screenRenderNodeDrawable;
    ASSERT_TRUE(surfaceDrawable_->DrawCacheImageForMultiScreenView(*canvas_, *surfaceParams));
}

/**
 * @tc.name: DrawCacheImageForMultiScreenView003
 * @tc.desc: Test DrawCacheImageForMultiScreenView
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawCacheImageForMultiScreenView003, TestSize.Level1)
{
    NodeId id = 1;
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto screenRenderNodeDrawable = std::make_shared<RSScreenRenderNodeDrawable>(renderNode);
    auto cacheImg = std::make_shared<Drawing::Image>();
    screenRenderNodeDrawable->cachedImageByCapture_ = cacheImg;
    surfaceParams->sourceDisplayRenderNodeDrawable_ = screenRenderNodeDrawable;
    ASSERT_TRUE(surfaceDrawable_->DrawCacheImageForMultiScreenView(*canvas_, *surfaceParams));
}

/**
 * @tc.name: GetGravityMatrix
 * @tc.desc: Test If GetGravityMatrix Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetGravityMatrixTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Matrix res = surfaceDrawable_->GetGravityMatrix(1.f, 1.f);
    ASSERT_EQ(res, Drawing::Matrix());
    surfaceDrawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID);
    surfaceDrawable_->renderParams_->SetCacheSize(Vector2f(100, 100));
    auto surfaceparams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceparams, nullptr);
    surfaceparams->SetUIFirstFrameGravity(Gravity::TOP_LEFT);
    Drawing::Matrix matrix = surfaceDrawable_->GetGravityMatrix(100, 100);
    ASSERT_EQ(matrix.Get(Drawing::Matrix::TRANS_X), 0);
    ASSERT_EQ(matrix.Get(Drawing::Matrix::TRANS_Y), 0);
}

/**
 * @tc.name: IsVisibleRegionEqualOnPhysicalAndVirtualTest
 * @tc.desc: test if visible region and virtual visible region equals.
 * @tc.type: FUNC
 * @tc.require: issueIBZ8K3
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, IsVisibleRegionEqualOnPhysicalAndVirtualTest, TestSize.Level1)
{
    Occlusion::Region emptyRegion;
    NodeId leashId = 1;
    auto leashWindow = std::make_shared<RSSurfaceRenderNode>(leashId);
    auto leashDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(leashWindow));
    ASSERT_NE(leashDrawable, nullptr);
    auto leashParams = static_cast<RSSurfaceRenderParams*>(leashDrawable->GetRenderParams().get());
    ASSERT_NE(leashParams, nullptr);
    leashParams->SetWindowInfo(false, true, false);
    leashParams->SetVisibleRegion(emptyRegion);
    leashParams->SetVisibleRegionInVirtual(emptyRegion);

    NodeId appId = 2;
    auto appWindow = std::make_shared<RSSurfaceRenderNode>(appId);
    auto appDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(appWindow));
    ASSERT_NE(appDrawable, nullptr);
    auto appParams = static_cast<RSSurfaceRenderParams*>(appDrawable->GetRenderParams().get());
    ASSERT_NE(appParams, nullptr);
    appParams->SetWindowInfo(true, false, true);
    leashParams->allSubSurfaceNodeIds_.insert(appId);

    // all empty
    appParams->SetVisibleRegion(emptyRegion);
    appParams->SetVisibleRegionInVirtual(emptyRegion);
    ASSERT_TRUE(leashDrawable->IsVisibleRegionEqualOnPhysicalAndVirtual(*leashParams));

    // region has one area
    Occlusion::Region region({ 100, 100, 1000, 1000 });
    appParams->SetVisibleRegion(region);
    appParams->SetVisibleRegionInVirtual(region);
    ASSERT_TRUE(leashDrawable->IsVisibleRegionEqualOnPhysicalAndVirtual(*leashParams));

    // region has multi area
    Occlusion::Region region2({ 100, 1200, 1000, 1500 });
    auto multiAreaRegion = region.Or(region2);
    appParams->SetVisibleRegion(multiAreaRegion);
    appParams->SetVisibleRegionInVirtual(multiAreaRegion);
    ASSERT_TRUE(leashDrawable->IsVisibleRegionEqualOnPhysicalAndVirtual(*leashParams));

    appParams->SetVisibleRegion(region);
    appParams->SetVisibleRegionInVirtual(multiAreaRegion);
    ASSERT_FALSE(leashDrawable->IsVisibleRegionEqualOnPhysicalAndVirtual(*leashParams));

    // one region is empty
    appParams->SetVisibleRegion(emptyRegion);
    appParams->SetVisibleRegionInVirtual(region);
    ASSERT_FALSE(leashDrawable->IsVisibleRegionEqualOnPhysicalAndVirtual(*leashParams));
}

/**
 * @tc.name: UpdateSurfaceDirtyRegion
 * @tc.desc: Test UpdateSurfaceDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, UpdateSurfaceDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    surfaceDrawable_->UpdateSurfaceDirtyRegion(canvas_);
    drawable_->renderParams_->shouldPaint_ = true;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->UpdateSurfaceDirtyRegion(canvas_);
    Drawing::Region region = surfaceDrawable_->GetSurfaceDrawRegion();
    EXPECT_TRUE(region.IsEmpty());
}

/**
 * @tc.name: DrawMagnificationRegion
 * @tc.desc: Test DrawMagnificationRegion
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawMagnificationRegionTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(500, 1000);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas canvas(surface.get());

    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    /* RSSurfaceRenderNodeDrawable::DrawMagnificationRegion abnormal case1 */
    surfaceParams->frameRect_ = {0, 0, 100, 100};
    surfaceParams->regionToBeMagnified_ = {0, 0, 0, 0};
    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    surfaceDrawable_->DrawMagnificationRegion(canvas, *surfaceParams);

    /* RSSurfaceRenderNodeDrawable::DrawMagnificationRegion abnormal case2 */
    surfaceParams->frameRect_ = {0, 0, 100, 100};
    surfaceParams->regionToBeMagnified_ = {1000, 1500, 100, 100};
    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    surfaceDrawable_->DrawMagnificationRegion(canvas, *surfaceParams);

    /* RSSurfaceRenderNodeDrawable::DrawMagnificationRegion base case */
    surfaceParams->frameRect_ = {0, 0, 100, 100};
    surfaceParams->regionToBeMagnified_ = {0, 0, 50, 50};
    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::ABILITY_MAGNIFICATION_NODE;
    surfaceDrawable_->DrawMagnificationRegion(canvas, *surfaceParams);
}

#ifdef SUBTREE_PARALLEL_ENABLE
/**
 * @tc.name: OnDraw004
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    canvas_->canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    NodeId id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->isMirrorScreen_ = false;
    RSUniRenderThread::Instance().Sync(std::move(params));

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_TRUE(surfaceParams);
    surfaceParams->IsUnobscuredUIExtension_ = false;
    std::unordered_set<NodeId> blackList = { surfaceParams->GetId() };
    auto displayRenderNodeDrawable = std::make_shared<RSScreenRenderNodeDrawable>(renderNode);
    surfaceParams->sourceScreenRenderNodeDrawable_ = std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>();
    ASSERT_FALSE(surfaceParams->sourceScreenRenderNodeDrawable_.lock());
    surfaceParams->cloneSourceDrawable_ = std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>();
    ASSERT_FALSE(surfaceParams->cloneSourceDrawable_.lock());

    surfaceParams->isCloneNode_ = false;
    surfaceParams->isSkipDraw_ = false;
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    canvas_->SetIsParallelCanvas(true);
    surfaceParams->isHardCursor_ = false;
    canvas_->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::SUBTREE_QUICK_DRAW_STATE);
    surfaceDrawable_->OnDraw(*canvas_);
    canvas_->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::DEFAULT_STATE);
    surfaceDrawable_->OnDraw(*canvas_);
    RSUniRenderThread::Instance().SetEnableVisibleRect(true);
    surfaceDrawable_->OnDraw(*canvas_);
    canvas_->canvas_->gpuContext_ = nullptr;
    surfaceDrawable_->OnDraw(*canvas_);
    RSUniRenderThread::Instance().SetBlackList({});
}

/**
 * @tc.name: OnDraw005
 * @tc.desc: Test OnDraw while has special layer
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw005, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    canvas_->canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    NodeId id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->isMirrorScreen_ = false;
    params->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(params));

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_TRUE(surfaceParams);
    surfaceParams->IsUnobscuredUIExtension_ = false;
    std::unordered_set<NodeId> blackList = { surfaceParams->GetId() };
    auto displayRenderNodeDrawable = std::make_shared<RSScreenRenderNodeDrawable>(renderNode);
    surfaceParams->sourceScreenRenderNodeDrawable_ = std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>();
    ASSERT_FALSE(surfaceParams->sourceScreenRenderNodeDrawable_.lock());
    surfaceParams->cloneSourceDrawable_ = std::weak_ptr<DrawableV2::RSRenderNodeDrawableAdapter>();
    ASSERT_FALSE(surfaceParams->cloneSourceDrawable_.lock());
    surfaceParams->isCloneNode_ = false;
    surfaceParams->isSkipDraw_ = false;
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().SetEnableVisibleRect(false);
    surfaceParams->UpdateHDRStatus(HdrStatus::HDR_EFFECT, true);

    // test no special layer
    surfaceDrawable_->OnDraw(*canvas_);
    // test security layer
    surfaceParams->specialLayerManager_.Set(SpecialLayerType::HAS_SECURITY, true);
    surfaceDrawable_->OnDraw(*canvas_);
    surfaceParams->specialLayerManager_.Set(SpecialLayerType::SECURITY, true);
    surfaceDrawable_->OnDraw(*canvas_);
    // test exceptional
    surfaceParams->specialLayerManager_.SetWithScreen(
        surfaceDrawable_->curDisplayScreenId_, SpecialLayerType::HAS_BLACK_LIST, true);
    surfaceDrawable_->OnDraw(*canvas_);
    // test protected layer
    surfaceParams->specialLayerManager_.Set(SpecialLayerType::PROTECTED, true);
    surfaceDrawable_->OnDraw(*canvas_);
}

/**
 * @tc.name: OnDraw006
 * @tc.desc: Test OnDraw when CheckIfSurfaceSkipInMirrorOrScreenshot return true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw006, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    canvas_->canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();

    NodeId id = 10086;
    auto renderNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(renderNode, nullptr);
    auto drawingCacheRoot = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    ASSERT_NE(drawingCacheRoot, nullptr);
    drawable_->curDrawingCacheRoot_ = drawingCacheRoot.get();
    ASSERT_NE(drawable_->curDrawingCacheRoot_, nullptr);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->isMirrorScreen_ = false;
    params->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_TRUE(surfaceParams);
    surfaceParams->isNodeGroupHasChildInBlacklist_ = true;
    EXPECT_TRUE(surfaceParams->NodeGroupHasChildInBlacklist());
    surfaceDrawable_->OnDraw(*drawingCanvas_);
    EXPECT_TRUE(surfaceDrawable_->hasSkipCacheLayer_);
}

/**
 * @tc.name: CalculateVisibleDirtyRegion002
 * @tc.desc: Test CalculateVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CalculateVisibleDirtyRegion002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    surfaceParams->SetWindowInfo(false, true, false);
    Drawing::Region result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, true);
    ASSERT_FALSE(result.IsEmpty());

    surfaceParams->SetWindowInfo(true, false, false);
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, true);
    ASSERT_FALSE(result.IsEmpty());

    Occlusion::Region region;
    surfaceParams->SetVisibleRegion(region);
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetOcclusionEnabled(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());

    surfaceParams->isFirstLevelCrossNode_ = true;
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());

    Occlusion::Region region1(DEFAULT_RECT);
    surfaceParams->SetVisibleRegion(region1);
    surfaceDrawable_->globalDirtyRegion_ = region1;
    surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());

    params = nullptr;
    RSUniRenderThread::Instance().Sync(std::move(params));
    surfaceDrawable_->CalculateVisibleDirtyRegion(*surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());
}

/**
 * @tc.name: DealWithSelfDrawingNodeBuffer
 * @tc.desc: Test DealWithSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithSelfDrawingNodeBufferTest003, TestSize.Level1)
{
    ASSERT_NE(drawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    surfaceParams->isHardwareEnabled_ = true;
    surfaceParams->SetNeedMakeImage(true);
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(*canvas_, *surfaceParams);
}

/**
 * @tc.name: QuickGetDrawState
 * @tc.desc: Test QuickGetDrawState
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, QuickDrawTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(canvas_, nullptr);
    Drawing::Region region;
    auto params = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    canvas_->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::DEFAULT_STATE);
    surfaceDrawable_->QuickGetDrawState(canvas_.get(), region, params);

    canvas_->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::SUBTREE_QUICK_DRAW_STATE);
    surfaceDrawable_->QuickGetDrawState(canvas_.get(), region, params);

    params->windowInfo_.isMainWindowType_ = true;
    surfaceDrawable_->QuickGetDrawState(canvas_.get(), region, params);
    ASSERT_EQ(params->needOffscreen_, false);

    params->needOffscreen_ = true;
    surfaceDrawable_->QuickGetDrawState(canvas_.get(), region, params);

    RotateOffScreenParam::SetRotateOffScreenSurfaceNodeEnable(false);
    surfaceDrawable_->QuickGetDrawState(canvas_.get(), region, params);
    ASSERT_EQ(canvas_->GetDisableFilterCache(), false);

    params->isOcclusionCullingOn_ = true;
    surfaceDrawable_->QuickGetDrawState(canvas_.get(), region, params);
}

/**
 * @tc.name: UpdateSurfaceDirtyRegion
 * @tc.desc: Test UpdateSurfaceDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, UpdateSurfaceDirtyRegionTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(canvas_, nullptr);
    surfaceDrawable_->UpdateSurfaceDirtyRegion(canvas_);

    surfaceDrawable_->renderParams_->shouldPaint_ = true;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());

    surfaceParams->isSkipDraw_ = false;
    surfaceDrawable_->UpdateSurfaceDirtyRegion(canvas_);

    surfaceParams->isSkipDraw_ = true;
    surfaceDrawable_->UpdateSurfaceDirtyRegion(canvas_);

    surfaceDrawable_->renderParams_ = std::make_unique<RSEffectRenderParams>(DEFAULT_ID);
    surfaceDrawable_->renderParams_->shouldPaint_ = true;
    surfaceDrawable_->UpdateSurfaceDirtyRegion(canvas_);
}

/**
 * @tc.name: GetSurfaceDrawRegion
 * @tc.desc: Test GetSurfaceDrawRegion
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetSurfaceDrawRegionTest, TestSize.Level1)
{
    Drawing::Region region;
    surfaceDrawable_->SetSurfaceDrawRegion(region);
    ASSERT_EQ(surfaceDrawable_->GetSurfaceDrawRegion(), region);
}

/**
 * @tc.name: DrawBufferForRotationFixed
 * @tc.desc: Test DrawBufferForRotationFixed
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawBufferForRotationFixedTest, TestSize.Level1)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    surfaceDrawable_->DrawBufferForRotationFixed(*canvas_, *surfaceParams);
    ASSERT_TRUE(canvas_->envStack_.top().hasOffscreenLayer_);
}

/**
 * @tc.name: DrawWatermark01
 * @tc.desc: Test DrawWatermark01
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawWatermark01, TestSize.Level1)
{
    ASSERT_TRUE(surfaceDrawable_ != nullptr);

    auto surfaceParams = static_cast<RSSurfaceRenderParam*>(surfaceDrawable_->GetRenderParams().get());
    surfaceDrawable_->DrawCommSurfaceWatermark(*canvas_, *surfaceParams);

    surfaceDrawable_->SetSystemWatermarkEnabled("watermask1", false);
    surfaceDrawable_->DrawCommSurfaceWatermark(*canvas_, *surfaceParams);
    surfaceDrawable_->ClearSystemWatermarkEnabled("watermask1");

    surfaceDrawable_->SetSystemWatermarkEnabled("watermask1", false);
    surfaceDrawable_->DrawCommSurfaceWatermark(*canvas_, *surfaceParams);

    surfaceDrawable_->SetSystemWatermarkEnabled("watermask1", true);
    surfaceDrawable_->DrawCommSurfaceWatermark(*canvas_, *surfaceParams);

    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->GetWatermark("watermask11111");

    int width = 50;
    int height = 50;
    Media::InitializationOptions opts;
    opts.size.width = 0;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    
    // Test 1
    std::unordered_map<std::string, std::pair<std::shared_ptr<Drawing::Image>, pid_t>> watermarks;
    auto tmpImagePtr = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    watermarks["watermask1"] = {tmpImagePtr, 0};
    uniParams->SetWatermarks(watermarks);
    RSUniRenderThread::Instance.Sync(std::move(uniParams));
    surfaceDrawable_->SetSystemWatermarkEnabled("watermask1", false);
    surfaceDrawable_->DrawCommSurfaceWatermark(*canvas_, *surfaceParams);
    surfaceDrawable_->SetSystemWatermarkEnabled("watermask1", true);

    // Test 2
    opts.size.width = width;
    opts.size.height = 0;
    pixelMap = Media::PixelMap::Create(opts);
    uniParams = std::make_unique<RSRenderThreadParams>();
    tmpImagePtr = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    watermarks["watermask1"] = {tmpImagePtr, 0};
    uniParams->SetWatermarks(watermarks);
    RSUniRenderThread::Instance.Sync(std::move(uniParams));
    surfaceDrawable_->DrawCommSurfaceWatermark(*canvas_, *surfaceParams);

    // Tewt 3
    opts.size.width = width;
    opts.size.height = height;
    pixelMap = Media::PixelMap::Create(opts);
    uniParams = std::make_unique<RSRenderThreadParams>();
    tmpImagePtr = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    watermarks["watermask1"] = {tmpImagePtr, 0};
    uniParams->SetWatermarks(watermarks);
    RSUniRenderThread::Instance.Sync(std::move(uniParams));
    surfaceDrawable_->DrawCommSurfaceWatermark(*canvas_, *surfaceParams);
}
#endif

/**
 * @tc.name: DrawSpecialLayer001
 * @tc.desc: test DrawSpecialLayer while renderThreadParams_ = nullptr
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawSpecialLayer001, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    ASSERT_FALSE(surfaceDrawable_->DrawSpecialLayer(*canvas_, *surfaceParams));
}

/**
 * @tc.name: CaptureSurface010
 * @tc.desc: test CaptureSurface while renderThreadParams_ = nullptr
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface010, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
}

/**
 * @tc.name: CaptureSurface011
 * @tc.desc: test CaptureSurface when UICapture is true or false
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface011, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(rsRenderThreadParams));

    CaptureParam captureParam;
    RSUniRenderThread::SetCaptureParam(captureParam);
    auto rsRenderThreadParams1 = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams1->isUIFirstDebugEnable_ = true;
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::LEASH_WINDOW);
    surfaceParams->SetGlobalPositionEnabled(true);
    surfaceParams->SetUifirstUseStarting(0);
    surfaceParams->SetWindowInfo(false, true, false);

    surfaceParams->matrix_.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    RSUniRenderThread::Instance().Sync(std::move(rsRenderThreadParams1));
    RSUniRenderThread::GetCaptureParam().isSingleSurface_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    canvas_->SetUICapture(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot007
 * @tc.desc: test CheckIfSurfaceSkipInMirrorOrScreenshot while renderThreadParams_ = nullptr
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot007, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot008
 * @tc.desc: Test CheckIfSurfaceSkipInMirrorOrScreenshot while node in white list
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot008, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->leashPersistentId_ = surfaceDrawable_->nodeId_;

    std::unordered_set<NodeId> whiteList = {surfaceParams->leashPersistentId_};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));

    AutoSpecialLayerStateRecover whiteListRecover(surfaceParams->leashPersistentId_);
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirrorOrScreenshot009
 * @tc.desc: Test CheckIfSurfaceSkipInMirrorOrScreenshot while it's a subThread drawing
 * @tc.type: FUNC
 * @tc.require: issue19858
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirrorOrScreenshot009, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));

    canvas_->SetIsParallelCanvas(true);
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirrorOrScreenshot(*surfaceParams, *canvas_));

    // restore
    canvas_->SetIsParallelCanvas(false);
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}
}