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

#include "common/rs_special_layer_manager.h"
#include "composer/composer_client/pipeline/rs_composer_client_manager.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/multi_screen/rs_multi_screen_util.h"
#include "graphic_feature_param_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;  // 65535
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

    static inline NodeId id_ = DEFAULT_ID;
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
    ++id_;
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
        FAIL() << "Failed to create node!";
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
    if (!displayDrawable_ || !mirrorSourceDisplayDrawable_ || !screenDrawable_ || !mirrorSourceScreenDrawable_) {
        FAIL() << "Failed to create drawable!";
    }

    displayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(id_);
    displayDrawable_->renderParams_->renderNodeType_ = displayNode_->GetType();
    mirrorSourceDisplayDrawable_->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(++id_);
    mirrorSourceDisplayDrawable_->renderParams_->renderNodeType_ = mirrorSourceDisplayNode_->GetType();
    auto tempScreenParams = std::make_unique<RSScreenRenderParams>(++id_);
    tempScreenParams->screenInfo_.id = screenId;
    screenDrawable_->renderParams_ = std::move(tempScreenParams);
    screenDrawable_->renderParams_->renderNodeType_ = screenNode_->GetType();
    auto tempMirrorSourceScreenParams = std::make_unique<RSScreenRenderParams>(++id_);
    tempMirrorSourceScreenParams->screenInfo_.id = mirroredScreenId;
    mirrorSourceScreenDrawable_->renderParams_ = std::move(tempMirrorSourceScreenParams);
    mirrorSourceScreenDrawable_->renderParams_->renderNodeType_ = mirrorSourceScreenNode_->GetType();

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirrorSourceScreenParams =
        static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable_->GetRenderParams().get());
    auto mirrorSourceDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    if (!displayParams || !mirrorSourceDisplayParams || !screenParams || !mirrorSourceScreenParams) {
        FAIL() << "Failed to init render params!";
    }
    displayParams->SetAncestorScreenDrawable(screenNode_->GetRenderDrawable());
    displayParams->mirrorSourceDrawable_ = mirrorSourceDisplayNode_->GetRenderDrawable();
    displayParams->screenId_ = screenNode_->GetScreenId();
    mirrorSourceDisplayParams->SetAncestorScreenDrawable(mirrorSourceScreenNode_->GetRenderDrawable());
    mirrorSourceDisplayParams->screenId_ = mirrorSourceScreenNode_->GetScreenId();
    screenParams->mirrorSourceDrawable_ = mirrorSourceScreenNode_->GetRenderDrawable();
    if (!screenNode_->GetRenderDrawable() || !displayNode_->GetRenderDrawable() ||
        !mirrorSourceScreenNode_->GetRenderDrawable() || !mirrorSourceScreenNode_->GetRenderDrawable()) {
        FAIL() << "Failed to get drawable from node!";
    }

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
 * @tc.name: RSMultiScreenUtilBasicTest001
 * @tc.desc: Test GetScreenParams and GetMirrorSourceParams, they are important and basic functions
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, RSMultiScreenUtilBasicTest001, TestSize.Level1)
{
    // Pay attention!
    // This is a important and basic test, you must ensure this testcase passed all!
    // If this case failed, it may lead to a significant drop in coverage even if other testcases do not failed!

    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirrorSourceScreenDrawable_, nullptr);
    ASSERT_NE(mirrorSourceDisplayDrawable_, nullptr);

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirrorSourceScreenParams =
        static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable_->GetRenderParams().get());
    auto mirrorSourceDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    
    EXPECT_NE(screenParams, nullptr);
    ASSERT_NE(displayParams, nullptr);
    EXPECT_NE(mirrorSourceScreenParams, nullptr);
    EXPECT_NE(mirrorSourceDisplayParams, nullptr);

    auto [tempScreenDrawable, tempScreenParams] = displayDrawable_->GetScreenParams(*displayParams);
    EXPECT_NE(tempScreenDrawable, nullptr);
    EXPECT_NE(tempScreenParams, nullptr);

    auto [tempMirrorSourceDisplayDrawable,
          tempMirrorSourceDisplayParams,
          tempMirrorSourceScreenDrawable,
          tempMirrorSourceScreenParams] = displayDrawable_->GetMirrorSourceParams(*displayParams);
    EXPECT_NE(tempMirrorSourceDisplayDrawable, nullptr);
    EXPECT_NE(tempMirrorSourceDisplayParams, nullptr);
    EXPECT_NE(tempMirrorSourceScreenDrawable, nullptr);
    EXPECT_NE(tempMirrorSourceScreenParams, nullptr);
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

/**
 * @tc.name: HandleVirtualExtendScreenTest004
 * @tc.desc: Test HandleVirtualExtendScreen with dirty align enabled and valid GpuTile
 * @tc.type: FUNC
 * @tc.require: issue23778
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest004, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty and dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = true;

    // Set valid GpuTile
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({64, 64});

    // Test with multiple dirty regions (size > dirtyRegionCountThresholdYf)
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);

    // Reset state
    uniParam->isDirtyAlignEnabled_ = false;
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({0, 0});
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest005
 * @tc.desc: Test HandleVirtualExtendScreen with dirty align disabled
 * @tc.type: FUNC
 * @tc.require: issue23778
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest005, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty but disable dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = false;

    // Set valid GpuTile (should not take the Clear path since dirty align is disabled)
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({64, 64});

    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);

    // Reset state
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({0, 0});
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest006
 * @tc.desc: Test HandleVirtualExtendScreen with invalid GpuTile
 * @tc.type: FUNC
 * @tc.require: issue23778
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest006, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty and dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = true;

    // Set invalid GpuTile (zero values), should take ClipRegion path
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({0, 0});

    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);

    // Reset state
    uniParam->isDirtyAlignEnabled_ = false;
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest007
 * @tc.desc: Test HandleVirtualExtendScreen with single dirty region
 * @tc.type: FUNC
 * @tc.require: issue23778
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest007, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    params->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty and dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = true;

    // Set valid GpuTile but with single dirty region (size <= dirtyRegionCountThresholdYf)
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({64, 64});

    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *params, processor);

    // Reset state
    uniParam->isDirtyAlignEnabled_ = false;
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({0, 0});
    EXPECT_NE(params, nullptr);
}

/**
 * @tc.name: DrawWiredMirrorDisplayTest001
 * @tc.desc: Test DrawWiredMirrorDisplay when curCanvas_ is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorDisplayTest001, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirrorSourceDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());

    // set canvas for test
    displayDrawable_->curCanvas_ = drawingFilterCanvas_.get();
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    // when mirrorSouceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    // when mirrorSourceScreenDrawable is nullptr
    mirrorSourceScreenDrawable_ = nullptr;
    mirrorSourceDisplayParams->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    params->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    // when curScreenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    // when curScreenDrawable is nullptr
    screenDrawable_ = nullptr;
    params->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    // when canvas is nullptr
    displayDrawable_->curCanvas_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, nullptr);
    EXPECT_EQ(displayDrawable_->curCanvas_, nullptr);
}

/**
 * @tc.name: DrawWiredMirrorDisplayTest002
 * @tc.desc: Test DrawWiredMirrorDisplay when has protected layer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorDisplayTest002, TestSize.Level1)
{
    auto mirroredRenderParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_PROTECTED, true);
    mirroredRenderParams->specialLayerManager_ = slManager;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSMultiScreenUtil::DrawWiredMirrorDisplay(*displayDrawable_, *params, processor);
    EXPECT_TRUE(mirroredRenderParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
}

/**
 * @tc.name: DrawWiredMirrorCopyTest001
 * @tc.desc: Test DrawWiredMirrorCopy when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorCopyTest001, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirrorSourceDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    
    // set canvas for test, you must ensure it is not nullptr forever
    displayDrawable_->curCanvas_ = drawingFilterCanvas_.get();
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    // when curScreenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    // when curScreenDrawable is nullptr
    screenDrawable_ = nullptr;
    params->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    // when mirrorSourceScreenDrawable is nullptr
    mirrorSourceScreenDrawable_ = nullptr;
    mirrorSourceDisplayParams->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    EXPECT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawWiredMirrorCopyTest002
 * @tc.desc: Test DrawWiredMirrorCopy when cacheImage is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorCopyTest002, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    displayDrawable_->curCanvas_ = drawingFilterCanvas_.get();

    // when rosen.cacheimage.mirror.enabled is 0
    system::SetParameter("rosen.cacheimage.mirror.enabled", "0");
    EXPECT_FALSE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    // restore to 1
    system::SetParameter("rosen.cacheimage.mirror.enabled", "1");
    EXPECT_TRUE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());

    // when enableVisibleRect is false
    displayDrawable_->enableVisibleRect_ = false;
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    // when enableVisibleRect is true
    displayDrawable_->enableVisibleRect_ = true;
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    EXPECT_NE(mirrorSourceScreenDrawable_->GetCacheImgForCapture(), nullptr);
}

/**
 * @tc.name: DrawWiredMirrorCopyTest003
 * @tc.desc: Test DrawWiredMirrorCopy when cacheImage is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorCopyTest003, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());

    // enableVisibleRect is false
    displayDrawable_->enableVisibleRect_ = false;
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    // enableVisibleRect is true
    displayDrawable_->enableVisibleRect_ = true;
    RSMultiScreenUtil::DrawWiredMirrorCopy(*displayDrawable_, *params);
    EXPECT_EQ(mirrorSourceScreenDrawable_->GetCacheImgForCapture(), nullptr);
}

/**
 * @tc.name: DrawWiredMirrorRebuildTest001
 * @tc.desc: Test DrawWiredMirrorRebuild when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorRebuildTest001, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto processor = std::make_shared<RSUniRenderProcessor>();
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;

    // when default, test once
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    // when mirrorSourceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    // when screenPrams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    // when screenDrawable is nullptr
    screenDrawable_ = nullptr;
    params->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    // restore uniParam
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    EXPECT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
}

/**
 * @tc.name: DrawWiredMirrorRebuildTest002
 * @tc.desc: Test DrawWiredMirrorRebuild when IsExternalScreenSecure is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorRebuildTest002, TestSize.Level1)
{
    auto isExternalScreenSecure = MultiScreenParam::IsExternalScreenSecure();
    MultiScreenParam::SetExternalScreenSecure(true);
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType(), 0);
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    EXPECT_TRUE(MultiScreenParam::IsExternalScreenSecure());
    MultiScreenParam::SetExternalScreenSecure(isExternalScreenSecure);
}

/**
 * @tc.name: DrawWiredMirrorRebuildTest003
 * @tc.desc: Test DrawWiredMirrorRebuild when IsExternalScreenSecure is false and hasSecSurface is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorRebuildTest003, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    auto mirroredParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    mirroredParams->specialLayerManager_ = slManager;
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType(), 0);
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    EXPECT_TRUE(mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: DrawWiredMirrorRebuildTest004
 * @tc.desc: Test DrawWiredMirrorRebuild
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorRebuildTest004, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType(), 0);
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    auto mirroredParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    mirroredParams->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    EXPECT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawWiredMirrorRebuildTest005
 * @tc.desc: Test DrawWiredMirrorRebuild
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorRebuildTest005, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType(), 0);
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, false);
    auto mirroredParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    mirroredParams->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    EXPECT_FALSE(mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: DrawWiredMirrorRebuildTest006
 * @tc.desc: Test DrawWiredMirrorRebuild when hasSecSurface && IsExternalScreenSecure
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawWiredMirrorRebuildTest006, TestSize.Level1)
{
    auto isExternalScreenSecure = MultiScreenParam::IsExternalScreenSecure();

    // default, hasSecSurface is false and isExternalScreenSecure is false
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType(), 0);
    MultiScreenParam::SetExternalScreenSecure(false);
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);

    // do something to make hasSecSurface is false and isExternalScreenSecure is true
    MultiScreenParam::SetExternalScreenSecure(true);
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);

    // do something to make hasSecSurface is true and isExternalScreenSecure is true
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    auto mirroredParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    mirroredParams->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);

    // do something to make hasSecSurface is true and isExternalScreenSecure is false
    MultiScreenParam::SetExternalScreenSecure(false);
    RSMultiScreenUtil::DrawWiredMirrorRebuild(*displayDrawable_, *params, processor);
    EXPECT_TRUE(mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));

    // reset isExternalScreenSecure
    MultiScreenParam::SetExternalScreenSecure(isExternalScreenSecure);
    slManager.Set(SpecialLayerType::HAS_SECURITY, false);
    mirroredParams->specialLayerManager_ = slManager;
}

/**
 * @tc.name: DrawVirtualMirrorDisplayTest001
 * @tc.desc: Test DrawVirtualMirrorDisplay when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorDisplayTest001, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    processor->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    // when processor is not RSUniRenderVirtualProcessor, it will be transformed to nullptr in the function
    auto otherProcessor = std::make_shared<RSUniRenderProcessor>();
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, otherProcessor);
    // when screenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, processor);
    // when mirrorSourceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, nullptr);
    EXPECT_EQ(mirrorSourceScreenDrawable_->renderParams_, nullptr);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, nullptr);
    EXPECT_EQ(mirrorSourceDisplayDrawable_->renderParams_, nullptr);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    params->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, nullptr);
    EXPECT_EQ(mirrorSourceDisplayDrawable_, nullptr);
}

/**
 * @tc.name: DrawVirtualMirrorDisplayTest002
 * @tc.desc: Test DrawVirtualMirrorDisplay when mirroredParams isSecurityDisplay != params isSecurityDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorDisplayTest002, TestSize.Level1)
{
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirrorSourceDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    processor->canvas_ = drawingFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    // when default
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());

    // when mirrorSourceDisplayParams isSecurityDisplay != displayParams isSecurityDisplay
    displayParams->isSecurityDisplay_ = true;
    mirrorSourceDisplayParams->isSecurityDisplay_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());

    // and then set specialLayerType HAS_SPECIAL_LAYER
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirrorSourceDisplayParams->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());

    // when cacheImage is not nullptr
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams, processor);
    EXPECT_NE(mirrorSourceScreenDrawable_->cacheImgForCapture_, nullptr);

    // when GetVirtualScreenMuteStatus is true
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = nullptr;
    displayParams->virtualScreenMuteStatus_ = true;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawVirtualMirrorDisplayTest003
 * @tc.desc: Test DrawVirtualMirrorDisplay when mirroredScreenIsPause is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorDisplayTest003, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirroredParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    ASSERT_NE(mirroredParams, nullptr);
    auto mirroredScreenParams =
        static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable_->GetRenderParams().get());
    VirtualScreenStatus status = mirroredScreenParams->screenProperty_.GetVirtualScreenStatus();
    mirroredScreenParams->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(VIRTUAL_SCREEN_PAUSE);
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    processor->canvas_ = drawingFilterCanvas_;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, processor);
    // recover virtualScreenStatus
    mirroredScreenParams->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(status);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawVirtualMirrorDisplayTest004
 * @tc.desc: Test DrawVirtualMirrorDisplay when mirroredParams SetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorDisplayTest004, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirroredScreenParams =
        static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable_->GetRenderParams().get());
    mirroredScreenParams->SetHDRPresent(false);
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    processor->canvas_ = drawingFilterCanvas_;
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
    instance.uniRenderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    mirroredScreenParams->SetHDRPresent(true);
    screenParams->SetHDRPresent(true);
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *params, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DumpDrawingPathTest001
 * @tc.desc: Test DumpDrawingPath
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DumpDrawingPathTest001, TestSize.Level1)
{
    displayDrawable_->lastFrameDrawingPath_ = DrawingPath::INVALID;
    RSMultiScreenUtil::DumpDrawingPath(*displayDrawable_, INVALID_SCREEN_ID,
        DrawingPath::INVALID, "");
    RSMultiScreenUtil::DumpDrawingPath(*displayDrawable_, INVALID_SCREEN_ID,
        DrawingPath::DRAW_VIRTUAL_MIRROR_COPY, "");
    EXPECT_EQ(displayDrawable_->lastFrameDrawingPath_, DrawingPath::DRAW_VIRTUAL_MIRROR_COPY);
}

/**
 * @tc.name: GetMultiScreenParamsTest001
 * @tc.desc: Test GetMultiScreenParams when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, GetMultiScreenParamsTest001, TestSize.Level1)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto mirrorSourceDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    ASSERT_NE(mirrorSourceDisplayParams, nullptr);
    // when default, all params is valid
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    // when mirrorSourceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    // when mirrorSourceScreenDrawable is nullptr
    mirrorSourceScreenDrawable_ = nullptr;
    mirrorSourceDisplayParams->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    params->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    // when screenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    // when screenDrawable nodeType is not SCREEN_NODE
    screenDrawable_->nodeType_ = RSRenderNodeType::UNKNOW;
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    // when screenDrawable is nullptr
    screenDrawable_ = nullptr;
    params->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::GetMultiScreenParams(*params);
    EXPECT_EQ(screenDrawable_, nullptr);
}
}