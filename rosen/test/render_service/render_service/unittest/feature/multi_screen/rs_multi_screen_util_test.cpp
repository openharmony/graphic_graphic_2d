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

#include <parameters.h>

#include "drawable/rs_logical_display_render_node_drawable.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/multi_screen/rs_multi_screen_util.h"
#include "graphic_feature_param_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
}
class RSMultiScreenUtilTest : public testing::Test {
public:
    std::shared_ptr<RSLogicalDisplayRenderNode> displayNode_;
    std::shared_ptr<RSLogicalDisplayRenderNode> mirrorSourceDisplayNode_;
    std::shared_ptr<RSScreenRenderNode> screenNode_;
    std::shared_ptr<RSScreenRenderNode> mirrorSourceScreenNode_;

    RSLogicalDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* mirrorSourceDisplayDrawable_ = nullptr;
    RSScreenRenderNodeDrawable* screenDrawable_ = nullptr;
    RSScreenRenderNodeDrawable* mirrorSourceScreenDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> drawingFilterCanvas_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirror_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> drawingFilterCanvasForMirror_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForScreen_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirrorScreen_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSMultiScreenUtilTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
}

void RSMultiScreenUtilTest::TearDownTestCase() {}

void RSMultiScreenUtilTest::SetUp()
{
    ++id;
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();

    RSDisplayNodeConfig config;
    displayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID, config);
    mirrorSourceDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID + 1, config); // 1 is id offset
    auto context = std::make_shared<RSContext>();
    auto screenId = 2;
    screenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID + 2, screenId, context); // 2 is id offset
    auto mirrorContext = std::make_shared<RSContext>();
    auto mirroredScreenId = 3;
    mirrorSourceScreenNode_ =
        std::make_shared<RSScreenRenderNode>(DEFAULT_ID + 3, mirroredScreenId, mirrorContext); // 3 is id offset
    if (!displayNode_ || !mirrorSourceDisplayNode_ || !screenNode_ || !mirrorSourceScreenNode_) {
        RS_LOGE("RSMultiScreenUtilTest: failed to create display node.");
        return;
    }
    displayNode_->InitRenderParams();
    mirrorSourceDisplayNode_->InitRenderParams();
    screenNode_->InitRenderParams();
    mirrorSourceScreenNode_->InitRenderParams();

    displayDrawable_ =
        std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(displayNode_->GetRenderDrawable()).get();
    mirrorSourceDisplayDrawable_ = std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(
        mirrorSourceDisplayNode_->GetRenderDrawable()).get();
    screenDrawable_ =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(screenNode_->GetRenderDrawable()).get();
    mirrorSourceScreenDrawable_ =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(mirrorSourceScreenNode_->GetRenderDrawable()).get();
    if (!displayDrawable_ || !mirrorSourceDisplayDrawable_ ||
        !screenDrawable_ || !mirrorSourceScreenDrawable_) {
        RS_LOGE("RSMultiScreenUtilTest: failed to create drawable.");
        return;
    }

    displayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id);
    mirrorSourceDisplayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(++id);
    auto screenParams = std::make_unique<RSScreenRenderParams>(++id);
    screenParams->screenInfo_.id = screenId;
    screenDrawable_->renderParams_ = std::move(screenParams);
    auto mirroredScreenParams = std::make_unique<RSScreenRenderParams>(++id);
    mirroredScreenParams->screenInfo_.id = mirroredScreenId;
    mirrorSourceScreenDrawable_->renderParams_ = std::move(mirroredScreenParams);
    if (!displayDrawable_->renderParams_ || !mirrorSourceDisplayDrawable_->renderParams_ ||
        !screenDrawable_->renderParams_ || !mirrorSourceScreenDrawable_->renderParams_) {
        RS_LOGE("RSMultiScreenUtilTest: failed to init render params.");
        return;
    }

    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (!displayParams) {
        RS_LOGE("RSMultiScreenUtilTest: failed to init render params.");
        return;
    }
    if (!screenNode_->GetRenderDrawable() || !displayNode_->GetRenderDrawable()) {
        RS_LOGE("RSMultiScreenUtilTest: failed to init render params.");
        return;
    }
    displayParams->SetAncestorScreenDrawable(screenNode_->GetRenderDrawable());
    displayParams->mirrorSourceDrawable_ = mirrorSourceDisplayNode_->GetRenderDrawable();
    displayParams->screenId_ = screenNode_->GetScreenId();

    auto mirroredDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    if (!mirroredDisplayParams || !mirrorSourceScreenNode_->GetRenderDrawable()) {
        RS_LOGE("RSMultiScreenUtilTest: failed to init render params.");
        return;
    }
    mirroredDisplayParams->SetAncestorScreenDrawable(mirrorSourceScreenNode_->GetRenderDrawable());
    mirroredDisplayParams->screenId_ = mirrorSourceScreenNode_->GetScreenId();

    auto tempScreenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    if (!tempScreenParams || !mirrorSourceScreenNode_->GetRenderDrawable()) {
        RS_LOGE("RSMultiScreenUtilTest: failed to init render params.");
        return;
    }
    tempScreenParams->mirrorSourceDrawable_ = mirrorSourceScreenNode_->GetRenderDrawable();

    // generate canvas for displayDrawable_
    drawingCanvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        drawingFilterCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
        displayDrawable_->curCanvas_ = drawingFilterCanvas_.get();
    }
    // generate canvas for mirrorSourceDisplayDrawable_
    drawingCanvasForMirror_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForMirror_) {
        drawingFilterCanvasForMirror_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvasForMirror_.get());
        mirrorSourceDisplayDrawable_->curCanvas_ = drawingFilterCanvasForMirror_.get();
    }
    // generate canvas for screenDrawable_
    drawingCanvasForScreen_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForScreen_) {
        screenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvasForScreen_.get());
    }
    // generate canvas for mirrorSourceScreenDrawable_
    drawingCanvasForMirrorScreen_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForMirrorScreen_) {
        mirrorSourceScreenDrawable_->curCanvas_ =
            std::make_shared<RSPaintFilterCanvas>(drawingCanvasForMirrorScreen_.get());
    }
}

void RSMultiScreenUtilTest::TearDown()
{
    // clear RSRenderThreadParams after each testcase
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

/**
 * @tc.name: HandleMirrorDisplayTest001
 * @tc.desc: Test HandleMirrorDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, HandleMirrorDisplayTest001, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *params, processor);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));

    // when screenParams is nullptr
    params->SetAncestorScreenDrawable(nullptr);
    auto [_, screenParams] = displayDrawable_->GetScreenParams(*params);
    EXPECT_EQ(screenParams, nullptr);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleMirrorDisplayTest002
 * @tc.desc: Test HandleMirrorDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, HandleMirrorDisplayTest002, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when processor is nullptr
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *params, nullptr);

    // when processor UpdateMirrorInfo failed
    displayDrawable_->renderParams_ = nullptr;
    bool res = processor->UpdateMirrorInfo(*displayDrawable_);
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleMirrorDisplayTest003
 * @tc.desc: Test HandleMirrorDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, HandleMirrorDisplayTest003, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    processor->isMirror_ = false;
    bool res = processor->UpdateMirrorInfo(*displayDrawable_);
    EXPECT_TRUE(res);
    Drawing::Canvas canvas;
    processor->canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    ASSERT_NE(processor->canvas_, nullptr);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *params, processor);

    // when compositeType_ of params is UNI_RENDER_COMPOSITE
    params->compositeType_ = CompositeType::UNI_RENDER_COMPOSITE;
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *params, processor);

    // when compositeType_ of params is not UNI_RENDER_COMPOSITE
    params->compositeType_ = CompositeType::UNI_RENDER_MIRROR_COMPOSITE;
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleMirrorScreenTest001
 * @tc.desc: Test HandleMirrorScreen
 * @tc.type: FUNC
 * @tc.require: #ICQ74B
 */
HWTEST_F(RSMultiScreenUtilTest, HandleMirrorScreenTest001, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto mirrorSourceParams = static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable_->GetRenderParams().get());
    EXPECT_NE(mirrorSourceParams, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceParams, *params, processor);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));

    // when processor is nullptr
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceParams, *params, nullptr);

    // when processor is not nullptr but init failed
    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PAUSE));
    bool res = processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceParams, *params, processor);
    EXPECT_NE(mirrorSourceParams, nullptr);
}

/**
 * @tc.name: HandleMirrorScreenTest002
 * @tc.desc: Test HandleMirrorScreen
 * @tc.type: FUNC
 * @tc.require: #ICQ74B
 */
HWTEST_F(RSMultiScreenUtilTest, HandleMirrorScreenTest002, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto mirrorSourceParams = static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable_->GetRenderParams().get());
    EXPECT_NE(mirrorSourceParams, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    bool res = processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    EXPECT_FALSE(res);
    processor->SetDisplaySkipInMirror(true);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceParams, *params, processor);
    processor->SetDisplaySkipInMirror(false);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceParams, *params, processor);
    EXPECT_NE(mirrorSourceParams, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendDisplayTest001
 * @tc.desc: Test HandleVirtualExtendDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendDisplayTest001, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *params, processor);

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *params, processor);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));

    // when screenParams is nullptr
    params->SetAncestorScreenDrawable(nullptr);
    auto [_, screenParams] = displayDrawable_->GetScreenParams(*params);
    EXPECT_EQ(screenParams, nullptr);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendDisplayTest002
 * @tc.desc: Test HandleVirtualExtendDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendDisplayTest002, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *params, processor);

    // when processor is nullptr
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *params, nullptr);

    // when processor UpdateMirrorInfo failed
    displayDrawable_->renderParams_ = nullptr;
    bool res = processor->UpdateMirrorInfo(*displayDrawable_);
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest001
 * @tc.desc: Test HandleVirtualExtendScreen
 * @tc.type: FUNC
 * @tc.require: #ICQ74B
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest001, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));

    // when processor is nullptr
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, nullptr);

    // when processor is not nullptr but init failed
    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PAUSE));
    bool res = processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest002
 * @tc.desc: Test HandleVirtualExtendScreen
 * @tc.type: FUNC
 * @tc.require: #ICQ74B
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest002, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    // when processor is not RSUniRenderVirtualProcessor
    auto processor = std::make_shared<RSUniRenderProcessor>();

    // processor is not RSUniRenderVirtualProcessor
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest003
 * @tc.desc: Test HandleVirtualExtendScreen
 * @tc.type: FUNC
 * @tc.require: #ICQ74B
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest003, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when processor init success
    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);

    // set uniParam dirty and test
    uniParam->SetVirtualExpandScreenDirtyEnabled(false);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->SetVirtualDirtyEnabled(true);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);
    uniParam->SetVirtualDirtyEnabled(false);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);
    params->SetHasMirrorScreen(true);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);
    EXPECT_NE(params, nullptr);
}
}