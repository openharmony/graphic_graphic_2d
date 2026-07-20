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
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr NodeId SCREEN_NODE_ID = DEFAULT_ID + 1;
constexpr NodeId DISPLAY_NODE_ID = DEFAULT_ID + 2;
constexpr NodeId MIRROR_SOURCE_SCREEN_NODE_ID = DEFAULT_ID + 3;
constexpr NodeId MIRROR_SOURCE_DISPLAY_NODE_ID = DEFAULT_ID + 4;
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr ScreenId SCREEN_ID = 10086;
constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
} // namespace
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

    RSLogicalDisplayRenderParams* displayParams_ = nullptr;
    RSLogicalDisplayRenderParams* mirrorSourceDisplayParams_ = nullptr;
    RSScreenRenderParams* screenParams_ = nullptr;
    RSScreenRenderParams* mirrorSourceScreenParams_ = nullptr;

    std::shared_ptr<Drawing::Canvas> canvas_ = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvas_ = nullptr;

    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcessor_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
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
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();

    RSDisplayNodeConfig config;
    screenNode_ = std::make_shared<RSScreenRenderNode>(SCREEN_NODE_ID, SCREEN_ID);
    displayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DISPLAY_NODE_ID, config);
    mirrorSourceScreenNode_ = std::make_shared<RSScreenRenderNode>(MIRROR_SOURCE_SCREEN_NODE_ID, SCREEN_ID);
    mirrorSourceDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(MIRROR_SOURCE_DISPLAY_NODE_ID, config);
    if (!displayNode_ || !mirrorSourceDisplayNode_ || !screenNode_ || !mirrorSourceScreenNode_) {
        FAIL() << "Failed to create node!";
    }

    screenNode_->InitRenderParams();
    displayNode_->InitRenderParams();
    mirrorSourceScreenNode_->InitRenderParams();
    mirrorSourceDisplayNode_->InitRenderParams();

    screenDrawable_ =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(screenNode_->renderDrawable_).get();
    displayDrawable_ =
        std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(displayNode_->renderDrawable_).get();
    mirrorSourceScreenDrawable_ =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(mirrorSourceScreenNode_->renderDrawable_).get();
    mirrorSourceDisplayDrawable_ =
        std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(mirrorSourceDisplayNode_->renderDrawable_).get();
    if (!displayDrawable_ || !mirrorSourceDisplayDrawable_ || !screenDrawable_ || !mirrorSourceScreenDrawable_) {
        FAIL() << "Failed to create drawable!";
    }

    screenParams_ = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    displayParams_ = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    mirrorSourceScreenParams_ =
        static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable_->GetRenderParams().get());
    mirrorSourceDisplayParams_ =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable_->GetRenderParams().get());
    if (!displayParams_ || !mirrorSourceDisplayParams_ || !screenParams_ || !mirrorSourceScreenParams_) {
        FAIL() << "Failed to init render params!";
    }

    screenParams_->screenInfo_.id = screenNode_->GetScreenId();
    screenParams_->renderNodeType_ = screenNode_->GetType();
    screenParams_->mirrorSourceDrawable_ = mirrorSourceScreenNode_->GetRenderDrawable();
    screenParams_->logicalDisplayNodeDrawables_.emplace_back(displayDrawable_);
    displayParams_->screenId_ = screenNode_->GetScreenId();
    displayParams_->renderNodeType_ = displayNode_->GetType();
    displayParams_->SetAncestorScreenDrawable(screenNode_->GetRenderDrawable());
    displayParams_->mirrorSourceDrawable_ = mirrorSourceDisplayNode_->GetRenderDrawable();
    mirrorSourceScreenParams_->screenInfo_.id = mirrorSourceScreenNode_->GetScreenId();
    mirrorSourceScreenParams_->renderNodeType_ = mirrorSourceScreenNode_->GetType();
    mirrorSourceScreenParams_->logicalDisplayNodeDrawables_.emplace_back(mirrorSourceDisplayDrawable_);
    mirrorSourceDisplayParams_->screenId_ = mirrorSourceScreenNode_->GetScreenId();
    mirrorSourceDisplayParams_->renderNodeType_ = mirrorSourceDisplayNode_->GetType();
    mirrorSourceDisplayParams_->SetAncestorScreenDrawable(mirrorSourceScreenNode_->GetRenderDrawable());

    virtualProcessor_ = std::make_shared<RSUniRenderVirtualProcessor>();
    if (!virtualProcessor_) {
        FAIL() << "Failed to create RSUniRenderVirtualProcessor!";
    }

    // Set canvas for drawable
    canvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (!canvas_) {
        FAIL() << "Failed to create canvas for displayDrawable_!";
    }

    paintFilterCanvas_ = std::make_shared<RSPaintFilterCanvas>(canvas_.get());
    virtualProcessor_->canvas_ = paintFilterCanvas_;
    displayDrawable_->curCanvas_ = paintFilterCanvas_.get();
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

    EXPECT_NE(screenParams_, nullptr);
    ASSERT_NE(displayParams_, nullptr);
    EXPECT_NE(mirrorSourceScreenParams_, nullptr);
    EXPECT_NE(mirrorSourceDisplayParams_, nullptr);

    auto [tempScreenDrawable, tempScreenParams] = displayDrawable_->GetScreenParams(*displayParams_);
    EXPECT_NE(tempScreenDrawable, nullptr);
    EXPECT_NE(tempScreenParams, nullptr);

    auto [tempMirrorSourceDisplayDrawable,
          tempMirrorSourceDisplayParams,
          tempMirrorSourceScreenDrawable,
          tempMirrorSourceScreenParams] = displayDrawable_->GetMirrorSourceParams(*displayParams_);
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
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *displayParams_, processor);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));

    // when screenParams is nullptr
    displayParams_->SetAncestorScreenDrawable(nullptr);
    auto [_, screenParams] = displayDrawable_->GetScreenParams(*displayParams_);
    EXPECT_EQ(screenParams, nullptr);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    EXPECT_NE(displayParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when processor is nullptr
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);

    // when processor UpdateMirrorInfo failed
    displayDrawable_->renderParams_ = nullptr;
    bool res = processor->UpdateMirrorInfo(*displayDrawable_);
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    EXPECT_NE(displayParams_, nullptr);
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

    virtualProcessor_->isMirror_ = false;
    bool res = virtualProcessor_->UpdateMirrorInfo(*displayDrawable_);
    EXPECT_TRUE(res);
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);

    // when compositeType_ of params is UNI_RENDER_COMPOSITE
    displayParams_->compositeType_ = CompositeType::UNI_RENDER_COMPOSITE;
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);

    // when compositeType_ of params is not UNI_RENDER_COMPOSITE
    displayParams_->compositeType_ = CompositeType::UNI_RENDER_MIRROR_COMPOSITE;
    RSMultiScreenUtil::HandleMirrorDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);
    EXPECT_NE(displayParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceScreenParams_, *screenParams_, processor);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));

    // when processor is nullptr
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceScreenParams_, *screenParams_, nullptr);

    // when processor is not nullptr but init failed
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PAUSE));
    bool res = processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceScreenParams_, *screenParams_, processor);
    EXPECT_NE(mirrorSourceScreenParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    bool res = processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    EXPECT_FALSE(res);
    processor->SetDisplaySkipInMirror(true);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceScreenParams_, *screenParams_, processor);
    processor->SetDisplaySkipInMirror(false);
    RSMultiScreenUtil::HandleMirrorScreen(*screenDrawable_, *mirrorSourceScreenParams_, *screenParams_, processor);
    EXPECT_NE(mirrorSourceScreenParams_, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendDisplayTest001
 * @tc.desc: Test HandleVirtualExtendDisplay
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendDisplayTest001, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    // when processor UpdateMirrorInfo failed
    virtualProcessor_->isMirror_ = true;
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    bool res = virtualProcessor_->UpdateMirrorInfo(*displayDrawable_);
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);

    // when processor is nullptr
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *displayParams_, nullptr);

    // when screenParams is nullptr
    displayParams_->SetAncestorScreenDrawable(nullptr);
    auto [_, screenParams] = displayDrawable_->GetScreenParams(*displayParams_);
    EXPECT_EQ(screenParams, nullptr);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleVirtualExtendDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);

    // restore uniParam for other test cases
    auto uniRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniRenderThreadParams));

    // when processor is nullptr
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, nullptr);

    // when processor is not nullptr but init failed
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PAUSE));
    bool res = processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    EXPECT_FALSE(res);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    EXPECT_NE(screenParams_, nullptr);
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

    // when processor is not RSUniRenderVirtualProcessor
    auto processor = std::make_shared<RSUniRenderProcessor>();

    // processor is not RSUniRenderVirtualProcessor
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    EXPECT_NE(screenParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();

    // when processor init success
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);

    // set uniParam dirty and test
    uniParam->SetVirtualExpandScreenDirtyEnabled(false);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->SetVirtualDirtyEnabled(true);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    uniParam->SetVirtualDirtyEnabled(false);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    screenParams_->SetHasMirrorScreen(true);
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    EXPECT_NE(screenParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty and dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = true;

    // Set valid GpuTile
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({ 64, 64 });

    // Test with multiple dirty regions (size > dirtyRegionCountThresholdYf)
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);

    // Reset state
    uniParam->isDirtyAlignEnabled_ = false;
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({ 0, 0 });
    EXPECT_NE(screenParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty but disable dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = false;

    // Set valid GpuTile (should not take the Clear path since dirty align is disabled)
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({ 64, 64 });

    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);

    // Reset state
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({ 0, 0 });
    EXPECT_NE(screenParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty and dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = true;

    // Set invalid GpuTile (zero values), should take ClipRegion path
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({ 0, 0 });

    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);

    // Reset state
    uniParam->isDirtyAlignEnabled_ = false;
    EXPECT_NE(screenParams_, nullptr);
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

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    // Enable virtual expand screen dirty and dirty align
    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isDirtyAlignEnabled_ = true;

    // Set valid GpuTile but with single dirty region (size <= dirtyRegionCountThresholdYf)
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({ 64, 64 });

    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);

    // Reset state
    uniParam->isDirtyAlignEnabled_ = false;
    RSUniDirtyComputeUtil::SetDamageRegionGpuTile({ 0, 0 });
    EXPECT_NE(screenParams_, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest008
 * @tc.desc: Test HandleVirtualExtendScreen with IsVirtualDirtyDfxEnabled true, takes else branch
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest008, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isVirtualDirtyDfxEnabled_ = true;
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    uniParam->isVirtualDirtyDfxEnabled_ = false;
    EXPECT_NE(screenParams_, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest009
 * @tc.desc: Test HandleVirtualExtendScreen with IsMultiSurfaceExtendMode true, takes else branch
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest009, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    RSUniRenderThread::Instance().InitGrContext();
    processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine());

    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    processor->needsOffscreenRender_ = true;
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    processor->needsOffscreenRender_ = false;
    EXPECT_NE(screenParams_, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest010
 * @tc.desc: Test HandleVirtualExtendScreen with all conditions satisfied, enters if branch
 * @tc.type: FUNC
 * @tc.require: #24870
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest010, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    screenParams_->childDisplayCount_ = 1;

    auto csurf = IConsumerSurface::Create("SetMultiSurfCfg_SF");
    ASSERT_NE(csurf, nullptr);
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    ASSERT_NE(rsSurface, nullptr);
    auto rasterFrame = std::make_unique<RSSurfaceFrameOhosRaster>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);

    SurfaceFrameConfig config;
    config.frame = std::make_unique<RSRenderFrame>(
        std::static_pointer_cast<RSSurfaceOhos>(rsSurface), std::move(rasterFrame));
    config.canvas = paintFilterCanvas_;
    config.region = RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    processor->surfaceFrames_.push_back(std::move(config));

    RSUniRenderThread::Instance().InitGrContext();
    ASSERT_TRUE(processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine()));

    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isVirtualDirtyDfxEnabled_ = false;
    processor->needsOffscreenRender_ = false;
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    EXPECT_NE(screenParams_, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest011
 * @tc.desc: Test HandleVirtualExtendScreen with IsVirtualExpandScreenDirtyEnabled false, enters else branch
 * @tc.type: FUNC
 * @tc.require: #24870
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest011, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    screenParams_->childDisplayCount_ = 1;

    auto csurf = IConsumerSurface::Create("SetMultiSurfCfg_SF");
    ASSERT_NE(csurf, nullptr);
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    ASSERT_NE(rsSurface, nullptr);
    auto rasterFrame = std::make_unique<RSSurfaceFrameOhosRaster>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);

    SurfaceFrameConfig config;
    config.frame = std::make_unique<RSRenderFrame>(
        std::static_pointer_cast<RSSurfaceOhos>(rsSurface), std::move(rasterFrame));
    config.canvas = paintFilterCanvas_;
    config.region = RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    processor->surfaceFrames_.push_back(std::move(config));

    RSUniRenderThread::Instance().InitGrContext();
    ASSERT_TRUE(processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine()));

    uniParam->SetVirtualExpandScreenDirtyEnabled(false);
    uniParam->isVirtualDirtyDfxEnabled_ = false;
    processor->needsOffscreenRender_ = false;
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    EXPECT_NE(screenParams_, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest012
 * @tc.desc: Test HandleVirtualExtendScreen with IsVirtualExpandScreenDirtyEnabled true and
 *           IsVirtualDirtyDfxEnabled true, enters else branch
 * @tc.type: FUNC
 * @tc.require: #24870
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest012, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    screenParams_->childDisplayCount_ = 1;

    auto csurf = IConsumerSurface::Create("SetMultiSurfCfg_SF");
    ASSERT_NE(csurf, nullptr);
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    ASSERT_NE(rsSurface, nullptr);
    auto rasterFrame = std::make_unique<RSSurfaceFrameOhosRaster>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);

    SurfaceFrameConfig config;
    config.frame = std::make_unique<RSRenderFrame>(
        std::static_pointer_cast<RSSurfaceOhos>(rsSurface), std::move(rasterFrame));
    config.canvas = paintFilterCanvas_;
    config.region = RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    processor->surfaceFrames_.push_back(std::move(config));

    RSUniRenderThread::Instance().InitGrContext();
    ASSERT_TRUE(processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine()));

    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isVirtualDirtyDfxEnabled_ = true;
    processor->needsOffscreenRender_ = false;
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    uniParam->isVirtualDirtyDfxEnabled_ = false;
    EXPECT_NE(screenParams_, nullptr);
}

/**
 * @tc.name: HandleVirtualExtendScreenTest013
 * @tc.desc: Test HandleVirtualExtendScreen with IsVirtualExpandScreenDirtyEnabled true,
 *           IsVirtualDirtyDfxEnabled false, and IsMultiSurfaceExtendMode true, enters else branch
 * @tc.type: FUNC
 * @tc.require: #24870
 */
HWTEST_F(RSMultiScreenUtilTest, HandleVirtualExtendScreenTest013, TestSize.Level1)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    EXPECT_NE(uniParam, nullptr);

    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    screenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    screenParams_->childDisplayCount_ = 1;

    auto csurf = IConsumerSurface::Create("SetMultiSurfCfg_SF");
    ASSERT_NE(csurf, nullptr);
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    ASSERT_NE(rsSurface, nullptr);
    auto rasterFrame = std::make_unique<RSSurfaceFrameOhosRaster>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);

    SurfaceFrameConfig config;
    config.frame = std::make_unique<RSRenderFrame>(
        std::static_pointer_cast<RSSurfaceOhos>(rsSurface), std::move(rasterFrame));
    config.canvas = paintFilterCanvas_;
    config.region = RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    processor->surfaceFrames_.push_back(std::move(config));

    RSUniRenderThread::Instance().InitGrContext();
    ASSERT_TRUE(processor->InitForRenderThread(*screenDrawable_, RSUniRenderThread::Instance().GetRenderEngine()));

    uniParam->SetVirtualExpandScreenDirtyEnabled(true);
    uniParam->isVirtualDirtyDfxEnabled_ = false;
    processor->needsOffscreenRender_ = true;
    RSMultiScreenUtil::HandleVirtualExtendScreen(*screenDrawable_, *screenParams_, processor);
    processor->needsOffscreenRender_ = false;
    EXPECT_NE(screenParams_, nullptr);
}

/**
 * @tc.name: DrawPhysicalMirrorDisplayTest001
 * @tc.desc: Test DrawPhysicalMirrorDisplay when curCanvas_ is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorDisplayTest001, TestSize.Level1)
{
    // when default
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    // when mirrorSouceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    // when mirrorSourceScreenDrawable is nullptr
    mirrorSourceScreenDrawable_ = nullptr;
    mirrorSourceDisplayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    displayParams_->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    // when curScreenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    // when curScreenDrawable is nullptr
    screenDrawable_ = nullptr;
    displayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    // when canvas is nullptr
    displayDrawable_->curCanvas_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    EXPECT_EQ(displayDrawable_->curCanvas_, nullptr);
}

/**
 * @tc.name: DrawPhysicalMirrorDisplayTest002
 * @tc.desc: Test DrawPhysicalMirrorDisplay when has protected layer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorDisplayTest002, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_PROTECTED, true);
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSMultiScreenUtil::DrawPhysicalMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    EXPECT_TRUE(mirrorSourceDisplayParams_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
}

/**
 * @tc.name: DrawPhysicalMirrorFromCacheTest001
 * @tc.desc: Test DrawPhysicalMirrorFromCache when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorFromCacheTest001, TestSize.Level1)
{
    // when default
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    // when curScreenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    // when curScreenDrawable is nullptr
    screenDrawable_ = nullptr;
    displayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    // when mirrorSourceScreenDrawable is nullptr
    mirrorSourceScreenDrawable_ = nullptr;
    mirrorSourceDisplayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    EXPECT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawPhysicalMirrorFromCacheTest002
 * @tc.desc: Test DrawPhysicalMirrorFromCache when cacheImage is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorFromCacheTest002, TestSize.Level1)
{
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();

    // when rosen.cacheimage.mirror.enabled is 0
    system::SetParameter("rosen.cacheimage.mirror.enabled", "0");
    EXPECT_FALSE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    // restore to 1
    system::SetParameter("rosen.cacheimage.mirror.enabled", "1");
    EXPECT_TRUE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());

    // when enableVisibleRect is false
    displayDrawable_->enableVisibleRect_ = false;
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    // when enableVisibleRect is true
    displayDrawable_->enableVisibleRect_ = true;
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    EXPECT_NE(mirrorSourceScreenDrawable_->GetCacheImgForCapture(), nullptr);
}

/**
 * @tc.name: DrawPhysicalMirrorFromCacheTest003
 * @tc.desc: Test DrawPhysicalMirrorFromCache when cacheImage is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorFromCacheTest003, TestSize.Level1)
{
    // enableVisibleRect is false
    displayDrawable_->enableVisibleRect_ = false;
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    // enableVisibleRect is true
    displayDrawable_->enableVisibleRect_ = true;
    RSMultiScreenUtil::DrawPhysicalMirrorFromCache(*displayDrawable_, *displayParams_);
    EXPECT_EQ(mirrorSourceScreenDrawable_->GetCacheImgForCapture(), nullptr);
}

/**
 * @tc.name: DrawPhysicalMirrorRebuildTest001
 * @tc.desc: Test DrawPhysicalMirrorRebuild when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorRebuildTest001, TestSize.Level1)
{
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    auto processor = std::make_shared<RSUniRenderProcessor>();
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;

    // when default, test once
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    // when mirrorSourceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    // when screenPrams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    // when screenDrawable is nullptr
    screenDrawable_ = nullptr;
    displayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    // when uniParam is nullptr
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    // restore uniParam
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    EXPECT_NE(RSUniRenderThread::Instance().GetRSRenderThreadParams(), nullptr);
}

/**
 * @tc.name: DrawPhysicalMirrorRebuildTest002
 * @tc.desc: Test DrawPhysicalMirrorRebuild when IsExternalScreenSecure is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorRebuildTest002, TestSize.Level1)
{
    auto isExternalScreenSecure = MultiScreenParam::IsExternalScreenSecure();
    MultiScreenParam::SetExternalScreenSecure(true);
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(displayParams_->GetCompositeType(), 0);
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    EXPECT_TRUE(MultiScreenParam::IsExternalScreenSecure());
    MultiScreenParam::SetExternalScreenSecure(isExternalScreenSecure);
}

/**
 * @tc.name: DrawPhysicalMirrorRebuildTest003
 * @tc.desc: Test DrawPhysicalMirrorRebuild when IsExternalScreenSecure is false and hasSecSurface is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorRebuildTest003, TestSize.Level1)
{
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(displayParams_->GetCompositeType(), 0);
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    EXPECT_TRUE(mirrorSourceDisplayParams_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: DrawPhysicalMirrorRebuildTest004
 * @tc.desc: Test DrawPhysicalMirrorRebuild
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorRebuildTest004, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(displayParams_->GetCompositeType(), 0);
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    EXPECT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawPhysicalMirrorRebuildTest005
 * @tc.desc: Test DrawPhysicalMirrorRebuild
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorRebuildTest005, TestSize.Level1)
{
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(displayParams_->GetCompositeType(), 0);
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, false);
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    EXPECT_FALSE(mirrorSourceDisplayParams_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}

/**
 * @tc.name: DrawPhysicalMirrorRebuildTest006
 * @tc.desc: Test DrawPhysicalMirrorRebuild when hasSecSurface && IsExternalScreenSecure
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawPhysicalMirrorRebuildTest006, TestSize.Level1)
{
    auto isExternalScreenSecure = MultiScreenParam::IsExternalScreenSecure();

    // default, hasSecSurface is false and isExternalScreenSecure is false
    std::shared_ptr<RSComposerClientManager> rsComposerClientMgr = std::make_shared<RSComposerClientManager>();
    RSUniRenderThread::Instance().composerClientManager_ = rsComposerClientMgr;
    auto processor = RSProcessorFactory::CreateProcessor(displayParams_->GetCompositeType(), 0);
    MultiScreenParam::SetExternalScreenSecure(false);
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);

    // do something to make hasSecSurface is false and isExternalScreenSecure is true
    MultiScreenParam::SetExternalScreenSecure(true);
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);

    // do something to make hasSecSurface is true and isExternalScreenSecure is true
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);

    // do something to make hasSecSurface is true and isExternalScreenSecure is false
    MultiScreenParam::SetExternalScreenSecure(false);
    RSMultiScreenUtil::DrawPhysicalMirrorRebuild(*displayDrawable_, *displayParams_, processor);
    EXPECT_TRUE(mirrorSourceDisplayParams_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));

    // reset isExternalScreenSecure
    MultiScreenParam::SetExternalScreenSecure(isExternalScreenSecure);
    slManager.Set(SpecialLayerType::HAS_SECURITY, false);
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
}

/**
 * @tc.name: DrawVirtualMirrorDisplayTest001
 * @tc.desc: Test DrawVirtualMirrorDisplay when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorDisplayTest001, TestSize.Level1)
{
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    processor->canvas_ = paintFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    // when processor is not RSUniRenderVirtualProcessor, it will be transformed to nullptr in the function
    auto otherProcessor = std::make_shared<RSUniRenderProcessor>();
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, otherProcessor);
    // when screenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    // when mirrorSourceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    EXPECT_EQ(mirrorSourceScreenDrawable_->renderParams_, nullptr);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
    EXPECT_EQ(mirrorSourceDisplayDrawable_->renderParams_, nullptr);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    displayParams_->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, nullptr);
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
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    processor->canvas_ = paintFilterCanvas_;
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    // when default
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());

    // when mirrorSourceDisplayParams isSecurityDisplay != displayParams isSecurityDisplay
    displayParams_->isSecurityDisplay_ = true;
    mirrorSourceDisplayParams_->isSecurityDisplay_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());

    // and then set specialLayerType HAS_SPECIAL_LAYER
    RSSpecialLayerManager slManager;
    slManager.Set(SpecialLayerType::HAS_SECURITY, true);
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    EXPECT_FALSE(processor->GetDrawVirtualMirrorCopy());

    // when cacheImage is not nullptr
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, processor);
    EXPECT_NE(mirrorSourceScreenDrawable_->cacheImgForCapture_, nullptr);

    // when GetVirtualScreenMuteStatus is true
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = nullptr;
    displayParams_->virtualScreenMuteStatus_ = true;
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, processor);
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
    VirtualScreenStatus status = mirrorSourceScreenParams_->screenProperty_.GetVirtualScreenStatus();
    mirrorSourceScreenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(VIRTUAL_SCREEN_PAUSE);
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);
    // recover virtualScreenStatus
    mirrorSourceScreenParams_->screenProperty_.Set<ScreenPropertyType::SCREEN_STATUS>(status);
    EXPECT_FALSE(virtualProcessor_->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawVirtualMirrorDisplayTest004
 * @tc.desc: Test DrawVirtualMirrorDisplay when mirroredParams SetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issue21104
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorDisplayTest004, TestSize.Level1)
{
    mirrorSourceScreenParams_->SetHDRPresent(false);
    RSUniRenderThread& instance = RSUniRenderThread::Instance();
    instance.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    ASSERT_NE(instance.uniRenderEngine_, nullptr);
    instance.uniRenderEngine_->SetColorFilterMode(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);
    EXPECT_FALSE(virtualProcessor_->GetDrawVirtualMirrorCopy());

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    mirrorSourceScreenParams_->SetHDRPresent(true);
    screenParams->SetHDRPresent(true);
    RSMultiScreenUtil::DrawVirtualMirrorDisplay(*displayDrawable_, *displayParams_, virtualProcessor_);
    EXPECT_FALSE(virtualProcessor_->GetDrawVirtualMirrorCopy());
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest001
 * @tc.desc: Test DrawVirtualMirrorFromCache when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest001, TestSize.Level1)
{
    RSRenderThreadParams uniParam;

    // when the canvas of processor is nullptr
    virtualProcessor_->canvas_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when mirrorSourceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when mirrorSourceScreenDrawable is nullptr
    mirrorSourceScreenDrawable_ = nullptr;
    mirrorSourceDisplayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    displayParams_->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when screenPrams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when screenDrawable is nullptr
    screenDrawable_ = nullptr;
    displayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_EQ(screenDrawable_, nullptr);
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest002
 * @tc.desc: Test DrawVirtualMirrorFromCache
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest002, TestSize.Level1)
{
    RSRenderThreadParams uniParam;

    // when default, isVirtualDirtyEnabled_ is false
    uniParam.isVirtualDirtyEnabled_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_FALSE(uniParam.IsVirtualDirtyEnabled());

    // when isVirtualDirtyEnabled_ is true
    uniParam.isVirtualDirtyEnabled_ = true;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_TRUE(uniParam.IsVirtualDirtyEnabled());

    // restore isVirtualDirtyEnabled_ to false
    uniParam.isVirtualDirtyEnabled_ = false;

    // when slrManager is nullptr
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when slrManager is not nullptr
    virtualProcessor_->slrManager_ = std::make_shared<RSSLRScaleFunction>(1.0f, 1.0f, 1.0f, 1.0f);
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest003
 * @tc.desc: Test DrawVirtualMirrorFromCache when enableVisibleRect_ is true
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest003, TestSize.Level1)
{
    RSRenderThreadParams uniParam;
    displayDrawable_->enableVisibleRect_ = true;
    // when GetTop > 0
    displayDrawable_->curVisibleRect_ = { 1, 1, 1, 1 };
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when GetTop > 0
    displayDrawable_->curVisibleRect_ = { 0, 0, 0, 0 };
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest004
 * @tc.desc: Test DrawVirtualMirrorFromCache when cacheImg is not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest004, TestSize.Level1)
{
    RSRenderThreadParams uniParam;

    // when cacheImg is not nullptr
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // enableVisibleRect_ is false
    displayDrawable_->enableVisibleRect_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_FALSE(displayDrawable_->enableVisibleRect_);
    // enableVisibleRect_ is true
    displayDrawable_->enableVisibleRect_ = true;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_TRUE(displayDrawable_->enableVisibleRect_);
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest005
 * @tc.desc: Test DrawVirtualMirrorFromCache when isSamplingOn is true/false
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest005, TestSize.Level1)
{
    RSRenderThreadParams uniParam;

    // when isSamplingOn is false
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = false;
    mirrorSourceScreenParams_->screenInfo_ = screenInfo;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_FALSE(mirrorSourceScreenParams_->GetScreenInfo().isSamplingOn);
    // when isSamplingOn is true
    screenInfo.isSamplingOn = true;
    mirrorSourceScreenParams_->screenInfo_ = screenInfo;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_TRUE(mirrorSourceScreenParams_->GetScreenInfo().isSamplingOn);
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest006
 * @tc.desc: Test DrawVirtualMirrorFromCache when curVisibleRect_.top/left is not zero
 * @tc.type: FUNC
 * @tc.require: #20348
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest006, TestSize.Level1)
{
    RSRenderThreadParams uniParam;

    uniParam.isVirtualDirtyEnabled_ = true;
    displayDrawable_->enableVisibleRect_ = true;
    displayDrawable_->curVisibleRect_ = { 0, 0, 1, 1 };
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    displayDrawable_->curVisibleRect_ = { 1, 0, 1, 1 };
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    displayDrawable_->curVisibleRect_ = { 0, 1, 1, 1 };
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    displayDrawable_->enableVisibleRect_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    uniParam.isVirtualDirtyEnabled_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_NE(displayDrawable_, nullptr);
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest007
 * @tc.desc: Test DrawVirtualMirrorFromCache with ProcessSingleSelfDrawingNode
 * @tc.type: FUNC
 * @tc.require: issue22872
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest007, TestSize.Level1)
{
    RSRenderThreadParams uniParam;
    uniParam.isVirtualDirtyEnabled_ = false;

    // when ProcessSingleSelfDrawingNode is false
    system::SetParameter("rosen.uni.virtualSelfDrawOptEnabled.enabled", "0");
    EXPECT_FALSE(RSSystemProperties::GetVirtualSelfDrawOptEnabled());
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // restore
    system::SetParameter("rosen.uni.virtualSelfDrawOptEnabled.enabled", "1");
    EXPECT_TRUE(RSSystemProperties::GetVirtualSelfDrawOptEnabled());
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest008
 * @tc.desc: Test DrawVirtualMirrorFromCache when mirrorSourceScreen sampling on
 * @tc.type: FUNC
 * @tc.require: issue22872
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest008, TestSize.Level1)
{
    RSRenderThreadParams uniParam;
    virtualProcessor_->SetDisplaySkipInMirror(false);
    mirrorSourceScreenParams_->layerSkipContext_.screenLayerInvalid_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    mirrorSourceScreenParams_->screenInfo_.isSamplingOn = true;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_TRUE(mirrorSourceScreenParams_->screenInfo_.isSamplingOn);
}

/**
 * @tc.name: DrawVirtualMirrorFromCacheTest009
 * @tc.desc: Test DrawVirtualMirrorFromCache when cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()
 * @tc.type: FUNC
 * @tc.require: issue22872
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorFromCacheTest009, TestSize.Level1)
{
    RSRenderThreadParams uniParam;
    uniParam.isVirtualDirtyEnabled_ = false;

    // ensure ProcessSingleSelfDrawingNode is false
    system::SetParameter("rosen.uni.virtualSelfDrawOptEnabled.enabled", "0");
    EXPECT_FALSE(RSSystemProperties::GetVirtualSelfDrawOptEnabled());

    // when cacheImage is nullptr
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // when cacheImage is not nullptr, but GetDrawMirrorCacheImageEnabled is false
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    system::SetParameter("rosen.cacheimage.mirror.enabled", "0");
    EXPECT_FALSE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // when cacheImage is not nullptr and GetDrawMirrorCacheImageEnabled is true
    mirrorSourceScreenDrawable_->cacheImgForCapture_ = std::make_shared<Drawing::Image>();
    EXPECT_NE(mirrorSourceScreenDrawable_->cacheImgForCapture_, nullptr);
    system::SetParameter("rosen.cacheimage.mirror.enabled", "1");
    EXPECT_TRUE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());
    RSMultiScreenUtil::DrawVirtualMirrorFromCache(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // restore
    system::SetParameter("rosen.uni.virtualSelfDrawOptEnabled.enabled", "1");
    EXPECT_TRUE(RSSystemProperties::GetVirtualSelfDrawOptEnabled());
    system::SetParameter("rosen.cacheimage.mirror.enabled", "1");
    EXPECT_TRUE(RSSystemProperties::GetDrawMirrorCacheImageEnabled());
}

/**
 * @tc.name: DrawVirtualMirrorRebuildTest001
 * @tc.desc: Test DrawVirtualMirrorRebuild when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorRebuildTest001, TestSize.Level1)
{
    RSRenderThreadParams uniParam;

    // when the canvas of processor is nullptr
    virtualProcessor_->canvas_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when mirrorSouceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    displayParams_->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when curScreenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // when curScreenDrawable is nullptr
    screenDrawable_ = nullptr;
    displayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    EXPECT_EQ(screenDrawable_, nullptr);
}

/**
 * @tc.name: DrawVirtualMirrorRebuildTest002
 * @tc.desc: Test DrawVirtualMirrorRebuild when has security layer
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorRebuildTest002, TestSize.Level1)
{
    RSRenderThreadParams uniParam;
    RSSpecialLayerManager slManager;
    slManager.specialLayerType_ = SpecialLayerType::HAS_SECURITY;
    mirrorSourceDisplayParams_->specialLayerManager_ = slManager;
    bool hasSecurity = mirrorSourceDisplayParams_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    EXPECT_EQ(hasSecurity, true);
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // when it is enableVisibleRect
    displayDrawable_->enableVisibleRect_ = true;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
}

/**
 * @tc.name: DrawVirtualMirrorRebuildTest003
 * @tc.desc: Test DrawVirtualMirrorRebuild when uniParam.IsVirtualDirtyEnabled() && !enableVisibleRect
 * @tc.type: FUNC
 * @tc.require: issue22872
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorRebuildTest003, TestSize.Level1)
{
    RSRenderThreadParams uniParam;
    EXPECT_FALSE(displayParams_->GetVirtualScreenMuteStatus());

    // when uniParam.IsVirtualDirtyEnabled() == false
    uniParam.SetVirtualDirtyEnabled(false);
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // when uniParam.IsVirtualDirtyEnabled() == true, but !enableVisibleRect_ == false
    uniParam.SetVirtualDirtyEnabled(true);
    displayDrawable_->enableVisibleRect_ = true;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // set uniParam.IsVirtualDirtyEnabled() == true, and !enableVisibleRect_ is true
    uniParam.SetVirtualDirtyEnabled(true);
    displayDrawable_->enableVisibleRect_ = false;
    // After set, when processer->GetDisplaySkipInMirror() == true
    virtualProcessor_->displaySkipInMirror_ = true;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
    // After set, when processer->GetDisplaySkipInMirror() == false
    virtualProcessor_->displaySkipInMirror_ = false;
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);
}

/**
 * @tc.name: DrawVirtualMirrorRebuildTest004
 * @tc.desc: Test DrawVirtualMirrorRebuild with ProcessSingleSelfDrawingNode
 * @tc.type: FUNC
 * @tc.require: issue22872
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualMirrorRebuildTest004, TestSize.Level1)
{
    RSRenderThreadParams uniParam;
    uniParam.SetVirtualDirtyEnabled(false);

    // when ProcessSingleSelfDrawingNode is false
    system::SetParameter("rosen.uni.virtualSelfDrawOptEnabled.enabled", "0");
    EXPECT_FALSE(RSSystemProperties::GetVirtualSelfDrawOptEnabled());
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // then, when mirrorSourceDisplayDrawable->ShouldPaint() == true
    mirrorSourceDisplayParams_->shouldPaint_ = true;
    mirrorSourceDisplayParams_->contentEmpty_ = false;
    EXPECT_TRUE(mirrorSourceDisplayDrawable_->ShouldPaint());
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // then, when mirrorSourceDisplayDrawable->ShouldPaint() == false
    mirrorSourceDisplayParams_->shouldPaint_ = false;
    mirrorSourceDisplayParams_->contentEmpty_ = true;
    EXPECT_FALSE(mirrorSourceDisplayDrawable_->ShouldPaint());
    RSMultiScreenUtil::DrawVirtualMirrorRebuild(*displayDrawable_, *displayParams_, virtualProcessor_, uniParam);

    // restore
    system::SetParameter("rosen.uni.virtualSelfDrawOptEnabled.enabled", "1");
    EXPECT_TRUE(RSSystemProperties::GetVirtualSelfDrawOptEnabled());
}

/**
 * @tc.name: DrawVirtualExtendTest001
 * @tc.desc: Test DrawVirtualExtend when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualExtendTest001, TestSize.Level1)
{
    // when processor is nullptr
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, nullptr);

    // when screenParams is nullptr
    displayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, nullptr);
    EXPECT_TRUE(displayParams_->GetAncestorScreenDrawable().expired());
}

/**
 * @tc.name: DrawVirtualExtendTest002
 * @tc.desc: Test DrawVirtualExtend when processor->IsMultiSurfaceExtendMode() and screenParam->GetHDRPresent()
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualExtendTest002, TestSize.Level1)
{
    EXPECT_NE(virtualProcessor_, nullptr);

    // when processor->IsMultiSurfaceExtendMode() == true, and screenParam->GetHDRPresent() == true
    virtualProcessor_->needsOffscreenRender_ = true;
    screenParams_->hasHdrPresent_ = true;
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, virtualProcessor_);

    // when processor->IsMultiSurfaceExtendMode() == true, and screenParam->GetHDRPresent() == false
    virtualProcessor_->needsOffscreenRender_ = true;
    screenParams_->hasHdrPresent_ = false;
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, virtualProcessor_);

    // when processor->IsMultiSurfaceExtendMode() == false, and screenParam->GetHDRPresent() == true
    virtualProcessor_->needsOffscreenRender_ = false;
    screenParams_->hasHdrPresent_ = true;
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, virtualProcessor_);

    // when processor->IsMultiSurfaceExtendMode() == false, and screenParam->GetHDRPresent() == false
    virtualProcessor_->needsOffscreenRender_ = false;
    screenParams_->hasHdrPresent_ = false;
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, virtualProcessor_);
}

/**
 * @tc.name: DrawVirtualExtendTest003
 * @tc.desc: Test DrawVirtualExtend when offscreenImage is nullptr or not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DrawVirtualExtendTest003, TestSize.Level1)
{
    // At first, set processor->IsMultiSurfaceExtendMode() == true
    virtualProcessor_->needsOffscreenRender_ = true;
    EXPECT_TRUE(virtualProcessor_->IsMultiSurfaceExtendMode());

    // Then set offscreenImage as nullptr
    displayDrawable_->offscreenSurface_ = nullptr;
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, virtualProcessor_);

    // we hope offscreenImage finally got is not nullptr
    displayDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    RSMultiScreenUtil::DrawVirtualExtend(*displayDrawable_, *displayParams_, virtualProcessor_);
}

/**
 * @tc.name: DumpRenderStrategyTest001
 * @tc.desc: Test DumpRenderStrategy
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, DumpRenderStrategyTest001, TestSize.Level1)
{
    displayDrawable_->lastFrameRenderStrategy_ = RenderStrategy::INVALID;
    RSMultiScreenUtil::DumpRenderStrategy(*displayDrawable_, INVALID_SCREEN_ID, RenderStrategy::INVALID, "");
    RSMultiScreenUtil::DumpRenderStrategy(
        *displayDrawable_, INVALID_SCREEN_ID, RenderStrategy::DRAW_VIRTUAL_MIRROR_FROM_CACHE, "");
    EXPECT_EQ(displayDrawable_->lastFrameRenderStrategy_, RenderStrategy::DRAW_VIRTUAL_MIRROR_FROM_CACHE);
}

/**
 * @tc.name: GetMultiScreenParamsTest001
 * @tc.desc: Test GetMultiScreenParams when drawable or params is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSMultiScreenUtilTest, GetMultiScreenParamsTest001, TestSize.Level1)
{
    ASSERT_NE(displayParams_, nullptr);
    ASSERT_NE(mirrorSourceDisplayParams_, nullptr);
    // when default, all params is valid
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    // when mirrorSourceScreenParams is nullptr
    mirrorSourceScreenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    // when mirrorSourceScreenDrawable is nullptr
    mirrorSourceScreenDrawable_ = nullptr;
    mirrorSourceDisplayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    // when mirrorSourceDisplayParams is nullptr
    mirrorSourceDisplayDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    // when mirrorSourceDisplayDrawable is nullptr
    mirrorSourceDisplayDrawable_ = nullptr;
    displayParams_->mirrorSourceDrawable_.reset();
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    // when screenParams is nullptr
    screenDrawable_->renderParams_ = nullptr;
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    // when screenDrawable nodeType is not SCREEN_NODE
    screenDrawable_->nodeType_ = RSRenderNodeType::UNKNOW;
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    // when screenDrawable is nullptr
    screenDrawable_ = nullptr;
    displayParams_->SetAncestorScreenDrawable(nullptr);
    RSMultiScreenUtil::GetMultiScreenParams(*displayParams_);
    EXPECT_EQ(screenDrawable_, nullptr);
}
} // namespace OHOS::Rosen