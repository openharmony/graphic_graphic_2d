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

#include "color_gamut_param.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUifirstManagerTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline RectI DEFAULT_RECT = {0, 0, 10, 10};
    static inline uint8_t GROUPED_BY_ANIM = 1;
    static inline NodeId INVALID_NODEID = 0xFFFF;
    static inline RSMainThread* mainThread_;
    static inline RSUifirstManager& uifirstManager_ = RSUifirstManager::Instance();
    static inline Occlusion::Region DEFAULT_VISIBLE_REGION = Occlusion::Rect(0, 0, 10, 10);
    static inline RectI VISIBLE_DIRTY_REGION = {0, 0, 10, 10};
    static inline RectI INVISIBLE_DIRTY_REGION = {20, 20, 10, 10};
};

void RSUifirstManagerTest2::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    if (mainThread_) {
        uifirstManager_.mainThread_ = mainThread_;
    }
    RSTestUtil::InitRenderNodeGC();
}

void RSUifirstManagerTest2::TearDownTestCase()
{
    auto mainThread = RSMainThread::Instance();
    if (!mainThread || !mainThread->context_) {
        return;
    }
    auto& renderNodeMap = mainThread->context_->GetMutableNodeMap();
    renderNodeMap.renderNodeMap_.clear();
    renderNodeMap.surfaceNodeMap_.clear();
    renderNodeMap.residentSurfaceNodeMap_.clear();
    renderNodeMap.screenNodeMap_.clear();
    renderNodeMap.canvasDrawingNodeMap_.clear();
    renderNodeMap.uiExtensionSurfaceNodes_.clear();

    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.markForceUpdateByUifirst_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.pendingResetNodes_.clear();
    uifirstManager_.pendingResetWindowCachedNodes_.clear();

    mainThread->context_->globalRootRenderNode_->renderDrawable_ = nullptr;
    mainThread->context_->globalRootRenderNode_ = nullptr;
}
void RSUifirstManagerTest2::SetUp() {}

void RSUifirstManagerTest2::TearDown() {}

/**
 * @tc.name: OnPurgePendingPostNodesInner
 * @tc.desc: Test OnPurgePendingPostNodesInner
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest2, OnPurgePendingPostNodesInner, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));
    surfaceRenderNode->renderDrawable_ = nullptr;
    uifirstManager_.OnPurgePendingPostNodesInner(surfaceRenderNode, false, adapter->GetRsSubThreadCache());
    EXPECT_TRUE(surfaceRenderNode->skipFrameDirtyRect_.IsEmpty());
    surfaceRenderNode->renderDrawable_ = adapter;
    auto surfaceDirtyManager = surfaceRenderNode->GetDirtyManager();
    RectI dirtyRect1 = {1, 1, 10, 10};
    surfaceDirtyManager->SetCurrentFrameDirtyRect(dirtyRect1);
    uifirstManager_.OnPurgePendingPostNodesInner(surfaceRenderNode, false, adapter->GetRsSubThreadCache());
    EXPECT_EQ(surfaceRenderNode->skipFrameDirtyRect_, dirtyRect1);

    nodeId = 2;
    auto surfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(nodeId);
    surfaceRenderNode->childSubSurfaceNodes_[nodeId] = surfaceRenderNode2;

    nodeId = 3;
    auto surfaceRenderNode3 = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter3 = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode3));
    auto surfaceDirtyManager3 = surfaceRenderNode3->GetDirtyManager();
    RectI dirtyRect2 = {0, 0, 5, 20};
    surfaceDirtyManager3->SetCurrentFrameDirtyRect(dirtyRect2);
    RectI dirtyRectRet = {0, 0, 11, 20};
    surfaceRenderNode->childSubSurfaceNodes_[nodeId] = surfaceRenderNode3;
    std::cout << surfaceRenderNode->SubSurfaceNodesDump() << std::endl;
    uifirstManager_.OnPurgePendingPostNodesInner(surfaceRenderNode, false, adapter->GetRsSubThreadCache());
    EXPECT_EQ(surfaceRenderNode->skipFrameDirtyRect_, dirtyRectRet);
}

/**
 * @tc.name: CommonPendingNodePurgeTest
 * @tc.desc: Test CommonPendingNodePurge
 * @tc.type: FUNC
 * @tc.require: issue20223
 */
HWTEST_F(RSUifirstManagerTest2, CommonPendingNodePurgeTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> map = { { nodeId, surfaceRenderNode } };
    auto iter = map.begin();

    ScreenId screenId = 1;
    auto screenManager = CreateOrGetScreenManager();
    screenManager->powerOffNeedProcessOneFrame_ = false;

    bool powerStatus = screenManager->screenPowerStatus_[screenId];
    screenManager->screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_ON;
    EXPECT_FALSE(uifirstManager_.CommonPendingNodePurge(iter));
    screenManager->screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_OFF;
    EXPECT_TRUE(uifirstManager_.CommonPendingNodePurge(iter));
    screenManager->screenPowerStatus_[screenId] = powerStatus;
}

/**
 * @tc.name: NeedPurgePendingPostNodesInner
 * @tc.desc: Test NeedPurgePendingPostNodesInner
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest2, NeedPurgePendingPostNodesInner, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));

    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> map = {{nodeId, surfaceRenderNode}};
    auto iter = map.begin();
    auto ret = uifirstManager_.NeedPurgePendingPostNodesInner(iter, adapter, false);
    EXPECT_FALSE(ret);

    uifirstManager_.purgeEnable_ = true;
    auto& subThreadCache = adapter->GetRsSubThreadCache();
    subThreadCache.isCacheCompletedValid_ = true;
    subThreadCache.SetSubThreadSkip(false);
    ret = uifirstManager_.NeedPurgePendingPostNodesInner(iter, adapter, true);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: DoPurgePendingPostNodes000
 * @tc.desc: Test DoPurgePendingPostNodes
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest2, DoPurgePendingPostNodes000, TestSize.Level1)
{
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    surfaceRenderNode->isOnTheTree_ = true;
    uifirstManager_.purgeEnable_ = true;
    auto& subThreadCache = adapter->GetRsSubThreadCache();
    subThreadCache.isCacheCompletedValid_ = true;
    subThreadCache.SetSubThreadSkip(false);
    subThreadCache.SetUifirstSurfaceCacheContentStatic(true);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(adapter->GetRenderParams().get());
    EXPECT_NE(surfaceParams, nullptr);
    surfaceParams->surfaceCacheContentStatic_ = true;

    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_TRUE(pendingNode.empty());
}

/**
 * @tc.name: DoPurgePendingPostNodes001
 * @tc.desc: Test DoPurgePendingPostNodes
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest2, DoPurgePendingPostNodes001, TestSize.Level1)
{
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    nodeId = 2;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    surfaceRenderNode->lastFrameUifirstFlag_ = MultiThreadCacheType::ARKTS_CARD;
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    adapter->GetRsSubThreadCache().hasHdrPresent_ = true;
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());
    uifirstManager_.subthreadProcessingNode_.clear();
}

/**
 * @tc.name: DoPurgePendingPostNodes002
 * @tc.desc: Test DoPurgePendingPostNodes
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest2, DoPurgePendingPostNodes002, TestSize.Level1)
{
    uifirstManager_.pendingNodeBehindWindow_.clear();
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    uifirstManager_.SetPurgeEnable(true);
    auto emptyRegion = Occlusion::Region();
    auto visibleRegion = Occlusion::Region({50, 50, 100, 100});
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    adapter->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    adapter->GetRsSubThreadCache().isCacheValid_ = true;
    uifirstManager_.UpdateCompletedSurface(surfaceRenderNode->GetId());

    surfaceRenderNode->SetGlobalAlpha(1.f);
    surfaceRenderNode->uifirstContentDirty_ = false;
    surfaceRenderNode->SetVisibleRegion(emptyRegion);
    surfaceRenderNode->SetVisibleRegionBehindWindow(emptyRegion);
    auto appWindow = RSTestUtil::CreateSurfaceNode();
    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    surfaceRenderNode->AddChild(appWindow);
    surfaceRenderNode->GenerateFullChildrenList();
    surfaceRenderNode->UpdateChildSubSurfaceNodes(appWindow, true);
    uifirstManager_.pendingNodeBehindWindow_[nodeId] = RSUifirstManager::NodeDataBehindWindow{
        .curTime = -30,
        .isFirst = false
    };
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    surfaceRenderNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONFOCUS_WINDOW);
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.pendingNodeBehindWindow_.clear();
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
}

/**
 * @tc.name: NeedPurgeByBehindWindow001
 * @tc.desc: Test NeedPurgeByBehindWindow
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest2, NeedPurgeByBehindWindow001, TestSize.Level1)
{
    auto emptyRegion = Occlusion::Region();
    auto visibleRegion = Occlusion::Region({50, 50, 100, 100});
    uifirstManager_.SetPurgeEnable(true);
    bool hasTexture = true;
    NodeId id = 0;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetGlobalAlpha(1.f);
    surfaceNode->uifirstContentDirty_ = false;
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONFOCUS_WINDOW);
    surfaceNode->SetVisibleRegion(emptyRegion);
    surfaceNode->SetVisibleRegionBehindWindow(emptyRegion);

    auto appWindow = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindow, nullptr);
    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    surfaceNode->AddChild(appWindow);
    surfaceNode->GenerateFullChildrenList();
    surfaceNode->UpdateChildSubSurfaceNodes(appWindow, true);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    ASSERT_FALSE(uifirstManager_.NeedPurgeByBehindWindow(id, hasTexture, surfaceNode));

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    system::SetParameter("rosen.ui.first.behindwindow.enabled", "0");
    ASSERT_FALSE(uifirstManager_.NeedPurgeByBehindWindow(id, hasTexture, surfaceNode));

    system::SetParameter("rosen.ui.first.behindwindow.enabled", "1");
    ASSERT_TRUE(uifirstManager_.NeedPurgeByBehindWindow(id, hasTexture, surfaceNode));

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
}

/**
 * @tc.name: HandlePurgeBehindWindow001
 * @tc.desc: Test HandlePurgeBehindWindow
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest2, HandlePurgeBehindWindow001, TestSize.Level1)
{
    uifirstManager_.pendingNodeBehindWindow_.clear();
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceRenderNode, nullptr);
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    auto it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId] = RSUifirstManager::NodeDataBehindWindow{
        .curTime = -30,
        .isFirst = false
    };

    auto ret = uifirstManager_.HandlePurgeBehindWindow(it);
    EXPECT_FALSE(ret);

    it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId].isFirst = true;
    uifirstManager_.pendingNodeBehindWindow_[nodeId].curTime = -30;
    ret = uifirstManager_.HandlePurgeBehindWindow(it);
    EXPECT_FALSE(ret);

    it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId].isFirst = true;
    uifirstManager_.pendingNodeBehindWindow_[nodeId].curTime = -29;
    ret = uifirstManager_.HandlePurgeBehindWindow(it);
    EXPECT_FALSE(ret);

    it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId].isFirst = false;
    uifirstManager_.pendingNodeBehindWindow_[nodeId].curTime = -29;
    ret = uifirstManager_.HandlePurgeBehindWindow(it);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: IsBehindWindowOcclusion001
 * @tc.desc: Test IsBehindWindowOcclusion
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest2, IsBehindWindowOcclusion001, TestSize.Level1)
{
    auto emptyRegion = Occlusion::Region();
    auto visibleRegion = Occlusion::Region({50, 50, 100, 100});
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto childSubSurfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetGlobalAlpha(1.f);
    surfaceNode->uifirstContentDirty_ = false;
    surfaceNode->SetVisibleRegion(emptyRegion);
    surfaceNode->SetVisibleRegionBehindWindow(emptyRegion);
    surfaceNode->UpdateChildSubSurfaceNodes(childSubSurfaceNode, true);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    surfaceNode->childSubSurfaceNodes_.clear();
    auto appWindow = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindow, nullptr);
    surfaceNode->AddChild(appWindow);
    surfaceNode->GenerateFullChildrenList();
    surfaceNode->UpdateChildSubSurfaceNodes(appWindow, true);
    appWindow->SetVisibleRegion(emptyRegion);
    appWindow->SetVisibleRegionBehindWindow(visibleRegion);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    appWindow->SetVisibleRegion(emptyRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(visibleRegion);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    EXPECT_TRUE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));
}

/**
 * @tc.name: GetUiFirstMode
 * @tc.desc: Test different uifirst mode
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, GetUiFirstMode, TestSize.Level1)
{
    uifirstManager_.SetUiFirstType(-1);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::SINGLE_WINDOW_MODE);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::SINGLE_WINDOW_MODE);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::MULTI_WINDOW_MODE);

    uifirstManager_.SetFreeMultiWindowStatus(true);
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::HYBRID);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::MULTI_WINDOW_MODE);

    uifirstManager_.SetFreeMultiWindowStatus(false);
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::HYBRID);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::SINGLE_WINDOW_MODE);
}

/**
 * @tc.name: RefreshUIFirstParam
 * @tc.desc: Test RefreshUIFirstParam
 * @tc.type: FUNC
 * @tc.require: issueIANPC2
 */
HWTEST_F(RSUifirstManagerTest2, RefreshUIFirstParam, TestSize.Level1)
{
    uifirstManager_.RefreshUIFirstParam();
    ASSERT_EQ(uifirstManager_.purgeEnable_, RSSystemParameters::GetUIFirstPurgeEnabled());

    // screenNode TypeError
    auto rootNode = RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode();
    ASSERT_NE(rootNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    rootNode->AddChild(surfaceNode, -1);
    uifirstManager_.RefreshUIFirstParam();
    ASSERT_EQ(uifirstManager_.purgeEnable_, RSSystemParameters::GetUIFirstPurgeEnabled());

    // screenNode
    NodeId id = 1;
    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 0, context->weak_from_this());
    screenNode->InitRenderParams();
    rootNode->ClearChildren();
    rootNode->AddChild(screenNode, -1);
    uifirstManager_.RefreshUIFirstParam();
    ASSERT_EQ(uifirstManager_.purgeEnable_, RSSystemParameters::GetUIFirstPurgeEnabled());
}

/**
 * @tc.name: IsSubTreeNeedPrepareForSnapshot
 * @tc.desc: Test IsSubTreeNeedPrepareForSnapshot in recents.
 * @tc.type: FUNC
 * @tc.require: issueIB7WHH
 */
HWTEST_F(RSUifirstManagerTest2, IsSubTreeNeedPrepareForSnapshot, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::ENTER_RECENTS);
    bool isOccluded = uifirstManager_.IsSubTreeNeedPrepareForSnapshot(*surfaceNode);
    ASSERT_EQ(isOccluded, false);
}

/**
@tc.name: CheckAndWaitPreFirstLevelDrawableNotify
@tc.desc: Test CheckAndWaitPreFirstLevelDrawableNotify
@tc.type: FUNC
@tc.require: #IBHZJA
*/
HWTEST_F(RSUifirstManagerTest2, CheckAndWaitPreFirstLevelDrawableNotify, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto renderParams = surfaceNode->GetRenderParams().get();
    bool res = RSUiFirstProcessStateCheckerHelper::CheckAndWaitPreFirstLevelDrawableNotify(*renderParams);
    ASSERT_TRUE(res);

    auto leashSurfaceNode = RSTestUtil::CreateSurfaceNode();
    renderParams->SetUiFirstRootNode(leashSurfaceNode->GetId());
    res = RSUiFirstProcessStateCheckerHelper::CheckAndWaitPreFirstLevelDrawableNotify(*renderParams);
    ASSERT_TRUE(res);
}

/**
@tc.name: IsCurFirstLevelMatch
@tc.desc: Test IsCurFirstLevelMatch
@tc.type: FUNC
@tc.require: #IBJQV8
*/
HWTEST_F(RSUifirstManagerTest2, IsCurFirstLevelMatch, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto renderParams = surfaceNode->GetRenderParams().get();
    bool res = RSUiFirstProcessStateCheckerHelper::IsCurFirstLevelMatch(*renderParams);
    ASSERT_EQ(res, true);
}

/**
@tc.name: CheckHwcChildrenType
@tc.desc: Test CheckHwcChildrenType
@tc.type: FUNC
@tc.require: #IBJQV8
*/
HWTEST_F(RSUifirstManagerTest2, CheckHwcChildrenType, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    SurfaceHwcNodeType enabledType = SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB;
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode->AddChildHardwareEnabledNode(surfaceNode1);
    ASSERT_EQ(surfaceNode->childHardwareEnabledNodes_.size(), 1);
    enabledType = SurfaceHwcNodeType::DEFAULT_HWC_TYPE;
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);

    surfaceNode1->name_ = "hipreview";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_VIDEO);

    surfaceNode1->name_ = "RosenWeb";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB);

    enabledType = SurfaceHwcNodeType::DEFAULT_HWC_TYPE;
    surfaceNode->name_ = "hipreview";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_VIDEO);

    surfaceNode->name_ = "RosenWeb";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB);

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto surfaceNdoe2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode->AddChild(surfaceNdoe2);
    surfaceNode->GenerateFullChildrenList();
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB);
    surfaceNode->ResetChildHardwareEnabledNodes();
}

/**
@tc.name: ResetWindowCache
@tc.desc: Test ResetWindowCache
@tc.type: FUNC
@tc.require: #IBWRGL
*/
HWTEST_F(RSUifirstManagerTest2, ResetWindowCache, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode1;
    ASSERT_EQ(surfaceNode1, nullptr);
    uifirstManager_.ResetWindowCache(surfaceNode1);

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode2, nullptr);
    uifirstManager_.ResetWindowCache(surfaceNode2);
}

/**
 * @tc.name: IsMissionCenterScene
 * @tc.desc: Test IsMissionCenterScene for pc
 * @tc.type: FUNC
 * @tc.require: #IBVTGU
 */
HWTEST_F(RSUifirstManagerTest2, IsMissionCenterScene, TestSize.Level1)
{
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::ENTER_MISSION_CENTER);
    auto isMissionCenterScene = uifirstManager_.IsMissionCenterScene();
    ASSERT_EQ(isMissionCenterScene, true);
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::OTHERS);
    isMissionCenterScene = uifirstManager_.IsMissionCenterScene();
    ASSERT_EQ(isMissionCenterScene, false);
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::EXIT_MISSION_CENTER);
    isMissionCenterScene = uifirstManager_.IsMissionCenterScene();
    ASSERT_EQ(isMissionCenterScene, true);
}

/**
 * @tc.name: IsSplitScreenScene
 * @tc.desc: Test IsSplitScreenScene for pc
 * @tc.type: FUNC
 * @tc.require: #IBVTGU
 */
HWTEST_F(RSUifirstManagerTest2, IsSplitScreenScene, TestSize.Level1)
{
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::ENTER_SPLIT_SCREEN);
    auto isSplitScreenScene = uifirstManager_.IsSplitScreenScene();
    ASSERT_EQ(isSplitScreenScene, true);
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::OTHERS);
    isSplitScreenScene = uifirstManager_.IsSplitScreenScene();
    ASSERT_EQ(isSplitScreenScene, false);
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::EXIT_SPLIT_SCREEN);
    isSplitScreenScene = uifirstManager_.IsSplitScreenScene();
    ASSERT_EQ(isSplitScreenScene, true);
}

/**
 * @tc.name: CheckMatchAndWaitNotify
 * @tc.desc: Test if first level node match
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, CheckMatchAndWaitNotify, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto renderParams = surfaceNode->GetRenderParams().get();
    ASSERT_TRUE(RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(*renderParams, false));

    renderParams->SetUiFirstRootNode(1);
    renderParams->SetFirstLevelNode(1);
    RSUiFirstProcessStateCheckerHelper::curUifirstRootNodeId_ = 1;
    RSUiFirstProcessStateCheckerHelper::curFirstLevelNodeId_ = 1;
    ASSERT_TRUE(RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(*renderParams, true));
}

/**
 * @tc.name: UpdateCompletedSurface
 * @tc.desc: Test UpdateCompletedSurface
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, UpdateCompletedSurface, TestSize.Level1)
{
    uifirstManager_.UpdateCompletedSurface(0);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(surfaceNode->GetId(), surfaceDrawable));

    surfaceDrawable->GetRsSubThreadCache().cacheSurface_ = nullptr;
    uifirstManager_.UpdateCompletedSurface(surfaceNode->GetId());
    ASSERT_FALSE(surfaceDrawable->GetRsSubThreadCache().isCacheValid_);

    surfaceDrawable->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable->GetRsSubThreadCache().isCacheValid_ = true;
    uifirstManager_.UpdateCompletedSurface(surfaceNode->GetId());
    ASSERT_TRUE(surfaceDrawable->GetRsSubThreadCache().isCacheCompletedValid_);
}

/**
 * @tc.name: OnProcessEventResponse
 * @tc.desc: Test receive system event
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, OnProcessEventResponse, TestSize.Level1)
{
    int32_t scbPid = 100;
    uifirstManager_.scbPid_ = scbPid;
    DataBaseRs scrollEvent {
        .uniqueId = 1,
        .appPid = scbPid,
        .sceneId = "LAUNCHER_SCROLL"
    };
    uifirstManager_.OnProcessEventResponse(scrollEvent);
    ASSERT_TRUE(uifirstManager_.currentFrameCanSkipFirstWait_);
    uifirstManager_.globalFrameEvent_.clear();

    DataBaseRs minimizedEvent {
        .uniqueId = 10,
        .appPid = scbPid,
        .sceneId = "WINDOW_TITLE_BAR_MINIMIZED"
    };
    uifirstManager_.OnProcessEventResponse(minimizedEvent);
    ASSERT_FALSE(uifirstManager_.currentFrameCanSkipFirstWait_);
    uifirstManager_.globalFrameEvent_.clear();
}

/**
 * @tc.name: PostUifistSubTasks
 * @tc.desc: Test post task to subthread
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, PostUifistSubTasks, TestSize.Level1)
{
    uifirstManager_.sortedSubThreadNodeIds_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->isOnTheTree_ = true;
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode1->GetId(), surfaceNode1));
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->isOnTheTree_ = true;
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode2->GetId(), surfaceNode2));
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNode();
    surfaceNode3->isOnTheTree_ = true;
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode3->GetId(), surfaceNode3));
    auto surfaceNode4 = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode4->GetId(), surfaceNode4));
    uifirstManager_.PostUifistSubTasks();
    ASSERT_TRUE(uifirstManager_.sortedSubThreadNodeIds_.empty());
}

/**
 * @tc.name: ForceUpdateUifirstNodes001
 * @tc.desc: Test uifirst not enabled
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, ForceUpdateUifirstNodes001, TestSize.Level1)
{
    // uifirst close
    uifirstManager_.isUiFirstOn_ = false;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);

    // node not support
    uifirstManager_.isUiFirstOn_ = true;
    surfaceNode->isUifirstNode_ = false;
    surfaceNode->isChildSupportUifirst_ = true;
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);

    // force disable uifirst
    uifirstManager_.isUiFirstOn_ = true;
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::FORCE_DISABLE);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);

    // protect layer not support
    uifirstManager_.isUiFirstOn_ = true;
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::NONE);
    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::HAS_PROTECTED, true);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);
}

/**
 * @tc.name: ForceUpdateUifirstNodes002
 * @tc.desc: Test force enable or disable uifirst
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, ForceUpdateUifirstNodes002, TestSize.Level1)
{
    uifirstManager_.isUiFirstOn_ = true;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::NONE);

    surfaceNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    ASSERT_FALSE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    // force enable
    surfaceNode->MarkUifirstNode(true, true);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::LEASH_WINDOW);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONFOCUS_WINDOW);

    // force disable
    surfaceNode->MarkUifirstNode(true, false);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
}

/**
 * @tc.name: ForceUpdateUifirstNodes003
 * @tc.desc: Test force enable uifirst on phone or pc
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, ForceUpdateUifirstNodes003, TestSize.Level1)
{
    uifirstManager_.isUiFirstOn_ = true;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::FORCE_ENABLE);

    // force enable on phone
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::LEASH_WINDOW);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE); // must reset uifirst flag

    // force enable on pc
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONFOCUS_WINDOW);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
}

/**
 * @tc.name: AddProcessDoneNode
 * @tc.desc: Test AddProcessDoneNode
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, PurgePendingPostNode, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.markForceUpdateByUifirst_.push_back(surfaceNode);
    uifirstManager_.pendingForceUpdateNode_.push_back(1);
    uifirstManager_.PurgePendingPostNodes();
    ASSERT_TRUE(uifirstManager_.markForceUpdateByUifirst_.empty());
    uifirstManager_.markForceUpdateByUifirst_.clear();
    uifirstManager_.pendingForceUpdateNode_.clear();
}

/**
 * @tc.name: AddProcessDoneNode
 * @tc.desc: Test AddProcessDoneNode
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, AddProcessDoneNode, TestSize.Level1)
{
    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.AddProcessDoneNode(0);
    ASSERT_TRUE(uifirstManager_.subthreadProcessDoneNode_.empty());

    uifirstManager_.AddProcessDoneNode(1);
    ASSERT_FALSE(uifirstManager_.subthreadProcessDoneNode_.empty());
    uifirstManager_.subthreadProcessDoneNode_.clear();
}

/**
 * @tc.name: AddCapturedNodes
 * @tc.desc: Test AddCapturedNodes
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, AddCapturedNodes, TestSize.Level1)
{
    uifirstManager_.AddCapturedNodes(1);
    ASSERT_FALSE(uifirstManager_.capturedNodes_.empty());
}

/**
 * @tc.name: CheckCurrentFrameHasCardNodeReCreate
 * @tc.desc: Test if card node recreate on single frame
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, CheckCurrentFrameHasCardNodeReCreate, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    uifirstManager_.CheckCurrentFrameHasCardNodeReCreate(*surfaceNode);
    ASSERT_FALSE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);

    surfaceNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    surfaceNode->name_ = "ArkTSCardNode";

    // card off the tree
    surfaceNode->isOnTheTree_ = false;
    uifirstManager_.CheckCurrentFrameHasCardNodeReCreate(*surfaceNode);
    ASSERT_FALSE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);

    // card on the tree
    surfaceNode->isOnTheTree_ = true;
    uifirstManager_.CheckCurrentFrameHasCardNodeReCreate(*surfaceNode);
    ASSERT_TRUE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);

    uifirstManager_.ResetCurrentFrameDeletedCardNodes();
    ASSERT_FALSE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);
    ASSERT_TRUE(uifirstManager_.currentFrameDeletedCardNodes_.empty());
}

/**
 * @tc.name: ReadUIFirstCcmParam
 * @tc.desc: Test ccm config
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, ReadUIFirstCcmParam, TestSize.Level1)
{
    uifirstManager_.ReadUIFirstCcmParam();
    ASSERT_TRUE(uifirstManager_.isUiFirstOn_);
    ASSERT_TRUE(uifirstManager_.isCardUiFirstOn_);
}

/**
 * @tc.name: IsSubHighPriorityType
 * @tc.desc: Test if surface has high priority
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, IsSubHighPriorityType, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->name_ = "hipreview";
    ASSERT_TRUE(uifirstManager_.IsSubHighPriorityType(*surfaceNode));

    surfaceNode->name_ = "video";
    ASSERT_FALSE(uifirstManager_.IsSubHighPriorityType(*surfaceNode));
}

/**
 * @tc.name: MarkSubHighPriorityType
 * @tc.desc: Test different priority strategy
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest2, MarkSubHighPriorityType, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->stagingRenderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    uifirstManager_.MarkSubHighPriorityType(*surfaceNode);
    ASSERT_FALSE(surfaceParams->GetPreSubHighPriorityType());

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    uifirstManager_.MarkSubHighPriorityType(*surfaceNode);
    ASSERT_FALSE(surfaceParams->GetPreSubHighPriorityType());
}

/**
 * @tc.name: CheckHasTransAndFilter001
 * @tc.desc: Test strategy with trans and blur scenes by uifirst.
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest2, CheckHasTransAndFilter001, TestSize.Level1)
{
    // no children node
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    bool result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // canvas children node
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    NodeId nodeId = 100;
    auto childNode1 = std::make_shared<RSCanvasRenderNode>(nodeId);
    parentNode->AddChild(childNode1);
    parentNode->GenerateFullChildrenList();
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // surface children node, but surface is not app window
    auto childNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode2, nullptr);
    parentNode->AddChild(childNode2);
    parentNode->GenerateFullChildrenList();
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // children type is app window
    auto childNode3 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode3, nullptr);
    childNode3->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parentNode->AddChild(childNode3);
    parentNode->GenerateFullChildrenList();
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckHasTransAndFilter002
 * @tc.desc: Test strategy with trans and blur scenes by uifirst.
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest2, CheckHasTransAndFilter002, TestSize.Level1)
{
    // leash window --> app window
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    auto childNode1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode1, nullptr);
    childNode1->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    childNode1->absDrawRect_ = {0, 0, 100, 100};
    parentNode->AddChild(childNode1);
    parentNode->GenerateFullChildrenList();
    auto result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // leash window ---> (app window, default surface) --> rootNode
    auto childNode2 = RSTestUtil::CreateSurfaceNode();
    childNode2->absDrawRect_ = {0, 0, 50, 50};
    parentNode->AddChild(childNode2);
    parentNode->GenerateFullChildrenList();
    NodeId nodeId = 200;
    auto rootChildNode = std::make_shared<RSRootRenderNode>(nodeId);
    childNode1->AddChild(rootChildNode);
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // leash window ---> (app window, default surface) --> rootNode --> canvasNode
    nodeId = 300;
    auto canvasChildNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    rootChildNode->AddChild(canvasChildNode);
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // has filter and transparent
    parentNode->childHasVisibleFilter_ = true;
    childNode2->absDrawRect_ = {0, 0, 200, 200};
    childNode2->abilityBgAlpha_ = 0;
    childNode2->globalAlpha_ = 0;
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: CheckHasTransAndFilter003
 * @tc.desc: Test strategy with trans and blur scenes by uifirst.
 * @tc.type: FUNC
 * @tc.require: issue21834
 */
HWTEST_F(RSUifirstManagerTest2, CheckHasTransAndFilter003, TestSize.Level1)
{
    auto leashWindow = RSTestUtil::CreateSurfaceNode();
    leashWindow->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    leashWindow->childHasVisibleFilter_ = true;

    auto appWindow = RSTestUtil::CreateSurfaceNode();
    appWindow->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    auto subWindow = RSTestUtil::CreateSurfaceNode();
    subWindow->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    leashWindow->AddChild(appWindow);
    leashWindow->AddChild(subWindow);
    leashWindow->GenerateFullChildrenList();

    // app window is opaque
    appWindow->SetGlobalAlpha(1.0f);
    appWindow->SetAbilityBGAlpha(255);
    appWindow->absDrawRect_ = { 0, 0, 100, 100 };
    appWindow->childHasVisibleFilter_ = true;
    auto rootNode = std::make_shared<RSRootRenderNode>(100);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(200);
    rootNode->AddChild(canvasNode);
    appWindow->AddChild(rootNode);

    auto subRootNode = std::make_shared<RSRootRenderNode>(300);
    auto subCanvasNode = std::make_shared<RSCanvasRenderNode>(400);
    subRootNode->AddChild(subCanvasNode);
    subWindow->AddChild(subRootNode);

    // sub window is opaque
    subWindow->SetGlobalAlpha(1.0f);
    subWindow->SetAbilityBGAlpha(255);
    ASSERT_FALSE(uifirstManager_.CheckHasTransAndFilter(*leashWindow));

    // sub window is transparent
    subWindow->SetGlobalAlpha(0.f);
    subWindow->SetAbilityBGAlpha(0);
    subWindow->childHasVisibleFilter_ = false;
    ASSERT_FALSE(uifirstManager_.CheckHasTransAndFilter(*leashWindow));

    subWindow->childHasVisibleFilter_ = true;
    // sub window inside main window
    subWindow->absDrawRect_ = { 10, 10, 50, 50 };
    ASSERT_FALSE(uifirstManager_.CheckHasTransAndFilter(*leashWindow));

    // sub window outside main window
    subWindow->absDrawRect_ = { 0, 0, 200, 200 };
    ASSERT_TRUE(uifirstManager_.CheckHasTransAndFilter(*leashWindow));
}

/**
 * @tc.name: GetCacheSurfaceProcessedStatusTest
 * @tc.desc: Test GetCacheSurfaceProcessedStatus
 * @tc.type: FUNC
 * @tc.require: issueIC3DK9
 */
HWTEST_F(RSUifirstManagerTest2, GetCacheSurfaceProcessedStatusTest, TestSize.Level1)
{
    NodeId nodeId = 100;
    RSSurfaceRenderParams surfaceParams(nodeId);
    ASSERT_EQ(uifirstManager_.GetCacheSurfaceProcessedStatus(surfaceParams), CacheProcessStatus::UNKNOWN);

    surfaceParams.SetUiFirstRootNode(nodeId);
    surfaceParams.SetFirstLevelNode(nodeId);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->GetRsSubThreadCache().SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    ASSERT_EQ(uifirstManager_.GetCacheSurfaceProcessedStatus(surfaceParams), CacheProcessStatus::DOING);

    surfaceParams.SetUiFirstRootNode(0);
    surfaceParams.SetFirstLevelNode(nodeId);
    surfaceDrawable->GetRsSubThreadCache().SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    ASSERT_EQ(uifirstManager_.GetCacheSurfaceProcessedStatus(surfaceParams), CacheProcessStatus::WAITING);
}

/**
 * @tc.name: SubThreadControlFrameRate
 * @tc.desc: Test SubThreadControlFrameRate
 * @tc.type: FUNC
 * @tc.require: issueIC3DK9
 */
HWTEST_F(RSUifirstManagerTest2, SubThreadControlFrameRate, TestSize.Level1)
{
    NodeId id = 100;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    auto rsSubThreadManager = RSSubThreadManager::Instance();
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable = nullptr;
    rsSubThreadManager->ScheduleRenderNodeDrawable(drawable);
    EXPECT_FALSE(drawable);
    bool ret = uifirstManager_.SubThreadControlFrameRate(id, drawable, node);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: ProcessFirstFrameCache
 * @tc.desc: Test main thread cache preprocess
 * @tc.type: FUNC
 * @tc.require: issueICPTT5
 */
HWTEST_F(RSUifirstManagerTest2, ProcessFirstFrameCache, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    surfaceNode->SetSubThreadAssignable(false);
    surfaceNode->SetSelfAndParentShouldPaint(true);
    surfaceNode->SetSkipDraw(false);
    uifirstManager_.ProcessFirstFrameCache(*surfaceNode, MultiThreadCacheType::ARKTS_CARD);
    ASSERT_TRUE(surfaceNode->GetSubThreadAssignable());

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    surfaceNode->SetSubThreadAssignable(false);
    surfaceNode->SetSelfAndParentShouldPaint(true);
    surfaceNode->SetSkipDraw(false);
    uifirstManager_.ProcessFirstFrameCache(*surfaceNode, MultiThreadCacheType::LEASH_WINDOW);
    ASSERT_TRUE(surfaceNode->GetSubThreadAssignable());
}

/**
 * @tc.name: IsArkTsCardCache
 * @tc.desc: Test if ArkTsCard node should enable uifirst
 * @tc.type: FUNC
 * @tc.require: issueICCSTG
 */
HWTEST_F(RSUifirstManagerTest2, IsArkTsCardCache, TestSize.Level1)
{
    NodeId id = 100;
    RSSurfaceRenderNode node(id);
    auto context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 0, context->weak_from_this());
    node.SetAncestorScreenNode(screenNode);

    // card disable uifirst by ccm
    uifirstManager_.SetCardUiFirstSwitch(false);
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    uifirstManager_.SetCardUiFirstSwitch(true);
    // leash window doesn't match
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node.name_ = "ArkTSCardNode";
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    // uifirst type doesn't match
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node.name_ = "ArkTSCardNode";
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    // node name doesn't match
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    node.nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node.name_ = "TestNode";
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    // force disable
    node.name_ = "ArkTSCardNode";
    node.SetUIFirstSwitch(RSUIFirstSwitch::FORCE_DISABLE_CARD);
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    uifirstManager_.entryViewNodeId_ = 1;
    uifirstManager_.negativeScreenNodeId_ = 1;
    node.instanceRootNodeId_ = 1;
    node.shouldPaint_ = true;

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    node.nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node.SetUIFirstSwitch(RSUIFirstSwitch::NONE);
    EXPECT_TRUE(RSUifirstManager::IsArkTsCardCache(node, true));
}

/**
 * @tc.name: ProcessDoneNodeInnerTest
 * @tc.desc: Test ProcessDoneNodeInner
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest2, ProcessDoneNodeInnerTest, TestSize.Level1)
{
    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.subthreadProcessDoneNode_.size(), 0);
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.subthreadProcessDoneNode_.push_back(surfaceNode1->GetId());
    uifirstManager_.subthreadProcessDoneNode_.push_back(++RSTestUtil::id);
    auto tmp = uifirstManager_.subthreadProcessDoneNode_;
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.pendingForceUpdateNode_.size(), 0);

    uifirstManager_.subthreadProcessDoneNode_ = tmp;
    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode1->renderDrawable_);
    drawable->GetRsSubThreadCache().SetCacheSurfaceNeedUpdated(true);
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.pendingForceUpdateNode_.size(), 0);

    uifirstManager_.subthreadProcessDoneNode_ = tmp;
    drawable->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.pendingForceUpdateNode_.size(), 1);

    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.pendingForceUpdateNode_.clear();
}

/**
 * @tc.name: AddPurgedNodeTest
 * @tc.desc: Test AddPurgedNode
 * @tc.type: FUNC
 * @tc.require: issue20223
 */
HWTEST_F(RSUifirstManagerTest2, AddPurgedNodeTest, TestSize.Level1)
{
    uifirstManager_.purgedNode_.clear();
    NodeId id = 0;
    uifirstManager_.AddPurgedNode(id);
    ASSERT_TRUE(uifirstManager_.purgedNode_.empty());

    id = 1;
    uifirstManager_.AddPurgedNode(id);
    ASSERT_FALSE(uifirstManager_.purgedNode_.empty());
}

/**
 * @tc.name: ProcessPurgedNodeTest
 * @tc.desc: Test ProcessPurgedNode
 * @tc.type: FUNC
 * @tc.require: issue20223
 */
HWTEST_F(RSUifirstManagerTest2, ProcessPurgedNodeTest, TestSize.Level1)
{
    uifirstManager_.purgedNode_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.ProcessPurgedNode();
    ASSERT_TRUE(uifirstManager_.pendingPostNodes_.empty());

    NodeId id = 1;
    uifirstManager_.AddPurgedNode(id);
    uifirstManager_.ProcessPurgedNode();
    ASSERT_TRUE(uifirstManager_.pendingPostCardNodes_.empty());
}

/**
 * @tc.name: ProcessPurgedNodeTest
 * @tc.desc: Test ProcessSubThreadSkipped
 * @tc.type: FUNC
 * @tc.require: issue20223
 */
HWTEST_F(RSUifirstManagerTest2, ProcessSubThreadSkippedNodeTest, TestSize.Level1)
{
    uifirstManager_.subthreadProcessSkippedNode_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.ProcessSubThreadSkippedNode();
    ASSERT_TRUE(uifirstManager_.pendingPostNodes_.empty());

    NodeId id = 1;
    uifirstManager_.AddProcessSkippedNode(id);
    uifirstManager_.ProcessSubThreadSkippedNode();
    ASSERT_TRUE(uifirstManager_.pendingPostCardNodes_.empty());
}

/**
 * @tc.name: ProcessSkippedNodeTest
 * @tc.desc: Test ProcessSkippedNode
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest2, ProcessSkippedNodeTest, TestSize.Level1)
{
    uifirstManager_.pendingPostCardNodes_.clear();
    std::unordered_set<NodeId> skippedNode_;
    uifirstManager_.ProcessSkippedNode(skippedNode_, true);
    ASSERT_TRUE(uifirstManager_.pendingPostCardNodes_.empty());

    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    skippedNode_.insert(surfaceNode1->GetId());
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode1);
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->isOnTheTree_ = true;
    surfaceNode2->lastFrameUifirstFlag_ = MultiThreadCacheType::ARKTS_CARD;
    skippedNode_.insert(surfaceNode2->GetId());
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode2);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(++RSTestUtil::id);
    skippedNode_.insert(canvasNode->GetId());
    mainThread_->context_->nodeMap.RegisterRenderNode(canvasNode);
    uifirstManager_.ProcessSkippedNode(skippedNode_, true);
    ASSERT_EQ(uifirstManager_.pendingPostCardNodes_.size(), 1);

    auto surfaceNode3 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->isOnTheTree_ = true;
    surfaceNode2->lastFrameUifirstFlag_ = MultiThreadCacheType::LEASH_WINDOW;
    skippedNode_.insert(surfaceNode3->GetId());
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode3);
    uifirstManager_.ProcessSkippedNode(skippedNode_, true);
    ASSERT_EQ(uifirstManager_.pendingPostNodes_.size(), 1);
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    mainThread_->context_->nodeMap.UnregisterRenderNode(surfaceNode1->GetId());
    mainThread_->context_->nodeMap.UnregisterRenderNode(surfaceNode2->GetId());
    mainThread_->context_->nodeMap.UnregisterRenderNode(surfaceNode3->GetId());
    mainThread_->context_->nodeMap.UnregisterRenderNode(canvasNode->GetId());
}

/**
 * @tc.name: IsNonFocusWindowCacheTest
 * @tc.desc: Test uifirst not enabled when exceed window threshold
 * @tc.type: FUNC
 * @tc.require: issueICJJK9
 */
HWTEST_F(RSUifirstManagerTest2, IsNonFocusWindowCacheTest, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.uifirstWindowsNumThreshold_ = 0;
    ASSERT_FALSE(RSUifirstManager::Instance().IsNonFocusWindowCache(*surfaceNode, true));

    RSMainThread::Instance()->focusNodeId_ = 0;
    RSMainThread::Instance()->focusLeashWindowId_ = 0;
    uifirstManager_.curUifirstWindowNums_ = 1;
    uifirstManager_.uifirstWindowsNumThreshold_ = 1;
    ASSERT_FALSE(RSUifirstManager::Instance().IsNonFocusWindowCache(*surfaceNode, true));
}

/**
 * @tc.name: IsFocusedNodeTest
 * @tc.desc: Test if node is focused node
 * @tc.type: FUNC
 * @tc.require: issueICJJK9
 */
HWTEST_F(RSUifirstManagerTest2, IsFocusedNodeTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    mainThread->focusNodeId_ = surfaceNode->GetId();
    mainThread->focusLeashWindowId_ = 0;
    ASSERT_TRUE(uifirstManager_.IsFocusedNode(*surfaceNode));

    mainThread->focusNodeId_ = 0;
    mainThread->focusLeashWindowId_ = surfaceNode->GetId();
    ASSERT_TRUE(uifirstManager_.IsFocusedNode(*surfaceNode));

    mainThread->focusNodeId_ = 0;
    mainThread->focusLeashWindowId_ = 0;
    ASSERT_FALSE(uifirstManager_.IsFocusedNode(*surfaceNode));
}

/**
 * @tc.name: IncreaseUifirstWindowCountTest
 * @tc.desc: Test increase current uifirst window count
 * @tc.type: FUNC
 * @tc.require: issueICJJK9
 */
HWTEST_F(RSUifirstManagerTest2, IncreaseUifirstWindowCountTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    mainThread->focusNodeId_ = surfaceNode->GetId();
    mainThread->focusLeashWindowId_ = 0;
    uifirstManager_.curUifirstWindowNums_ = 0;
    // focus window don't count
    uifirstManager_.IncreaseUifirstWindowCount(*surfaceNode);
    ASSERT_EQ(uifirstManager_.curUifirstWindowNums_, 0);

    mainThread->focusNodeId_ = 0;
    mainThread->focusLeashWindowId_ = 0;
    surfaceNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    // ArkTsCard node don't count
    uifirstManager_.IncreaseUifirstWindowCount(*surfaceNode);
    ASSERT_EQ(uifirstManager_.curUifirstWindowNums_, 0);

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    uifirstManager_.IncreaseUifirstWindowCount(*surfaceNode);
    ASSERT_EQ(uifirstManager_.curUifirstWindowNums_, 1);
}

/**
 * @tc.name: DecreaseUifirstWindowCountTest
 * @tc.desc: Test decrease current uifirst window count
 * @tc.type: FUNC
 * @tc.require: issueICJJK9
 */
HWTEST_F(RSUifirstManagerTest2, DecreaseUifirstWindowCountTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    mainThread->focusNodeId_ = surfaceNode->GetId();
    mainThread->focusLeashWindowId_ = 0;
    uifirstManager_.curUifirstWindowNums_ = 1;
    // focus window don't count
    uifirstManager_.DecreaseUifirstWindowCount(*surfaceNode);
    ASSERT_EQ(uifirstManager_.curUifirstWindowNums_, 1);

    mainThread->focusNodeId_ = 0;
    mainThread->focusLeashWindowId_ = 0;
    surfaceNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    // ArkTsCard node don't count
    uifirstManager_.DecreaseUifirstWindowCount(*surfaceNode);
    ASSERT_EQ(uifirstManager_.curUifirstWindowNums_, 1);

    uifirstManager_.curUifirstWindowNums_ = 1;
    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    uifirstManager_.DecreaseUifirstWindowCount(*surfaceNode);
    ASSERT_EQ(uifirstManager_.curUifirstWindowNums_, 0);

    uifirstManager_.curUifirstWindowNums_ = 0;
    uifirstManager_.DecreaseUifirstWindowCount(*surfaceNode);
    ASSERT_EQ(uifirstManager_.curUifirstWindowNums_, 0);
}

/**
 * @tc.name: IsExceededWindowsThresholdTest
 * @tc.desc: Test if exceed window threshold
 * @tc.type: FUNC
 * @tc.require: issueICJJK9
 */
HWTEST_F(RSUifirstManagerTest2, IsExceededWindowsThresholdTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.uifirstWindowsNumThreshold_ = 0;
    ASSERT_FALSE(uifirstManager_.IsExceededWindowsThreshold(*surfaceNode));
    
    uifirstManager_.uifirstWindowsNumThreshold_ = 1;
    mainThread->focusNodeId_ = surfaceNode->GetId();
    ASSERT_FALSE(uifirstManager_.IsExceededWindowsThreshold(*surfaceNode));

    mainThread->focusNodeId_ = 0;
    mainThread->focusLeashWindowId_ = 0;
    uifirstManager_.curUifirstWindowNums_ = 0;
    ASSERT_FALSE(uifirstManager_.IsExceededWindowsThreshold(*surfaceNode));

    uifirstManager_.curUifirstWindowNums_ = 1;
    ASSERT_TRUE(uifirstManager_.IsExceededWindowsThreshold(*surfaceNode));
}

/**
 * @tc.name: AddMarkedClearCacheNodeTest
 * @tc.desc: Test AddMarkedClearCacheNode
 * @tc.type: FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSUifirstManagerTest2, AddMarkedClearCacheNodeTest, TestSize.Level1)
{
    uifirstManager_.markedClearCacheNodes_.clear();
    uifirstManager_.AddMarkedClearCacheNode(0);
    ASSERT_EQ(uifirstManager_.markedClearCacheNodes_.size(), 0);

    for (int i = 0; i < 1000; i++) {
        uifirstManager_.AddMarkedClearCacheNode(i);
    }
    // max node count is 500
    ASSERT_NE(uifirstManager_.markedClearCacheNodes_.size(), 1000);

    uifirstManager_.markedClearCacheNodes_.clear();
    uifirstManager_.AddMarkedClearCacheNode(1);
    ASSERT_EQ(uifirstManager_.markedClearCacheNodes_.size(), 1);
}

/**
 * @tc.name: IsCacheSizeValid
 * @tc.desc: Test IsCacheSizeValid for Rotate situation
 * @tc.type: FUNC
 * @tc.require: issue#19987
 */
HWTEST_F(RSUifirstManagerTest2, IsCacheSizeValid, TestSize.Level2)
{
    auto surfaceNodePtr = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodePtr, nullptr);
    auto& surfaceNode = *surfaceNodePtr;

    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode.GetStagingRenderParams().get());
    ASSERT_NE(stagingSurfaceParams, nullptr);

    auto ret = false;

    /* test case: defaultValue
    */
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_TRUE(ret);

    /* test case: lastUifirstFlag
    */
    surfaceNode.SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_TRUE(ret);

    /* test case: cachedSize
    */
    stagingSurfaceParams->SetCacheSize({0.f, 1.f});
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_TRUE(ret);
    stagingSurfaceParams->SetCacheSize({1.f, 0.f});
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_TRUE(ret);
    stagingSurfaceParams->SetCacheSize({1.f, 2.f});
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_TRUE(ret);

    /* test case: lastCachedSize Rotate
    */
    stagingSurfaceParams->UpdateLastCacheSize();
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_TRUE(ret);

    stagingSurfaceParams->SetCacheSize({2.f, 1.f});
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_FALSE(ret);

    /* test case: Gravity::RESIZE
    */
    stagingSurfaceParams->SetUIFirstFrameGravity(Gravity::RESIZE);
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_TRUE(ret);

    /* test case: invalid stagingSurfaceParams
    */
    surfaceNode.GetStagingRenderParams() = nullptr;
    ret = uifirstManager_.IsCacheSizeValid(surfaceNode);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ProcessMarkedNodeSubThreadCacheTest
 * @tc.desc: Test ProcessMarkedNodeSubThreadCache
 * @tc.type: FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSUifirstManagerTest2, ProcessMarkedNodeSubThreadCacheTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    uifirstManager_.AddMarkedClearCacheNode(nodeId);
    uifirstManager_.subthreadProcessingNode_.insert({nodeId, nullptr});
    // node is processing, do not clear
    uifirstManager_.ProcessMarkedNodeSubThreadCache();
    ASSERT_EQ(uifirstManager_.markedClearCacheNodes_.size(), 0);

    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.AddMarkedClearCacheNode(nodeId);
    // node not processing, but drawable is null
    uifirstManager_.ProcessMarkedNodeSubThreadCache();
    ASSERT_EQ(uifirstManager_.markedClearCacheNodes_.size(), 0);

    uifirstManager_.AddMarkedClearCacheNode(nodeId);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    auto& rsSubThreadCache = surfaceDrawable->GetRsSubThreadCache();
    rsSubThreadCache.cacheSurface_ = std::make_shared<Drawing::Surface>();
    uifirstManager_.pendingPostNodes_.insert({nodeId, nullptr});
    // node in pendingPostNodes, do not clear
    uifirstManager_.ProcessMarkedNodeSubThreadCache();
    ASSERT_TRUE(rsSubThreadCache.cacheSurface_ != nullptr);
    ASSERT_EQ(uifirstManager_.markedClearCacheNodes_.size(), 0);

    uifirstManager_.AddMarkedClearCacheNode(nodeId);
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.insert({nodeId, nullptr});
    // node in pendingPostCardNodes, do not clear
    uifirstManager_.ProcessMarkedNodeSubThreadCache();
    ASSERT_TRUE(rsSubThreadCache.cacheSurface_ != nullptr);
    ASSERT_EQ(uifirstManager_.markedClearCacheNodes_.size(), 0);

    uifirstManager_.AddMarkedClearCacheNode(nodeId);
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    // clear cachesurface
    uifirstManager_.ProcessMarkedNodeSubThreadCache();
    ASSERT_TRUE(rsSubThreadCache.cacheSurface_ == nullptr);
    ASSERT_EQ(uifirstManager_.markedClearCacheNodes_.size(), 0);
}

/**
 * @tc.name: OnProcessAnimateScene002
 * @tc.desc: Test OnProcessAnimateScene when tablet snapshot rotation scene
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSUifirstManagerTest2, OnProcessAnimateSceneTest002, TestSize.Level2)
{
    bool isSnapshotRotationScene = uifirstManager_.isSnapshotRotationScene_;
    EXPECT_FALSE(uifirstManager_.IsSnapshotRotationScene());

    auto scene = SystemAnimatedScenes::SNAPSHOT_ROTATION;
    uifirstManager_.OnProcessAnimateScene(scene);
    EXPECT_TRUE(uifirstManager_.IsSnapshotRotationScene());

    scene = SystemAnimatedScenes::OTHERS;
    uifirstManager_.OnProcessAnimateScene(scene);
    EXPECT_FALSE(uifirstManager_.IsSnapshotRotationScene());

    uifirstManager_.isSnapshotRotationScene_ = isSnapshotRotationScene;
}

/**
 * @tc.name: ShouldAutoCleanCacheTest001
 * @tc.desc: Test auto clear cache not enabled in single mode
 * @tc.type: FUNC
 * @tc.require: issues20692
 */
HWTEST_F(RSUifirstManagerTest2, ShouldAutoCleanCacheTest001, TestSize.Level1)
{
    NodeId id = 1;
    RsSubThreadCache subThreadCache;
    subThreadCache.ResetCacheReuseCount();
    uifirstManager_.clearCacheThreshold_ = 10;
    uifirstManager_.uifirstType_ = UiFirstCcmType::SINGLE;

    uifirstManager_.ShouldAutoCleanCache(id, subThreadCache);
    ASSERT_EQ(subThreadCache.GetCacheReuseCount(), 0);
}

/**
 * @tc.name: ShouldAutoCleanCacheTest002
 * @tc.desc: Test auto clear cache not enabled when threshold<=0
 * @tc.type: FUNC
 * @tc.require: issues20692
 */
HWTEST_F(RSUifirstManagerTest2, ShouldAutoCleanCacheTest002, TestSize.Level1)
{
    NodeId id = 1;
    RsSubThreadCache subThreadCache;
    subThreadCache.ResetCacheReuseCount();
    uifirstManager_.clearCacheThreshold_ = 0;
    uifirstManager_.uifirstType_ = UiFirstCcmType::MULTI;

    uifirstManager_.ShouldAutoCleanCache(id, subThreadCache);
    ASSERT_EQ(subThreadCache.GetCacheReuseCount(), 0);
}

/**
 * @tc.name: ShouldAutoCleanCacheTest003
 * @tc.desc: Test auto clear cache not enabled property is false
 * @tc.type: FUNC
 * @tc.require: issues20692
 */
HWTEST_F(RSUifirstManagerTest2, ShouldAutoCleanCacheTest003, TestSize.Level1)
{
    NodeId id = 1;
    RsSubThreadCache subThreadCache;
    subThreadCache.ResetCacheReuseCount();
    uifirstManager_.clearCacheThreshold_ = 5;
    int autoClearEnabled = RSSystemProperties::GetUIFirstAutoClearCacheEnabled();
    system::SetParameter("rosen.ui.first.auto.clearcache.enabled", "0");
    uifirstManager_.ShouldAutoCleanCache(id, subThreadCache);
    ASSERT_EQ(subThreadCache.GetCacheReuseCount(), 0);
    system::SetParameter("rosen.ui.first.auto.clearcache.enabled", std::to_string(autoClearEnabled));
}

/**
 * @tc.name: ShouldAutoCleanCacheTest004
 * @tc.desc: Test no need to clear cache when cachesurface is nullptr
 * @tc.type: FUNC
 * @tc.require: issues20692
 */
HWTEST_F(RSUifirstManagerTest2, ShouldAutoCleanCacheTest004, TestSize.Level1)
{
    NodeId id = 1;
    RsSubThreadCache subThreadCache;
    subThreadCache.ResetCacheReuseCount();
    uifirstManager_.clearCacheThreshold_ = 5;

    subThreadCache.cacheSurface_ = nullptr;
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.ShouldAutoCleanCache(id, subThreadCache);
    ASSERT_EQ(subThreadCache.GetCacheReuseCount(), 0);
}

/**
 * @tc.name: ShouldAutoCleanCacheTest005
 * @tc.desc: Test auto clear cache
 * @tc.type: FUNC
 * @tc.require: issues20692
 */
HWTEST_F(RSUifirstManagerTest2, ShouldAutoCleanCacheTest005, TestSize.Level1)
{
    NodeId id = 1;
    RsSubThreadCache subThreadCache;
    subThreadCache.ResetCacheReuseCount();
    uifirstManager_.clearCacheThreshold_ = 3;
    uifirstManager_.markedClearCacheNodes_.clear();
    uifirstManager_.subthreadProcessingNode_.clear();
    subThreadCache.cacheSurface_ = std::make_shared<Drawing::Surface>();

    // reuse count is 1
    uifirstManager_.ShouldAutoCleanCache(id, subThreadCache);
    ASSERT_EQ(subThreadCache.GetCacheReuseCount(), 1);
    ASSERT_TRUE(uifirstManager_.markedClearCacheNodes_.empty());

    uifirstManager_.subthreadProcessingNode_.insert({ id, nullptr });
    // reuse count is 2
    uifirstManager_.ShouldAutoCleanCache(id, subThreadCache);
    ASSERT_EQ(subThreadCache.GetCacheReuseCount(), 2);
    ASSERT_TRUE(uifirstManager_.markedClearCacheNodes_.empty());

    // cache reuse reach threshold
    uifirstManager_.ShouldAutoCleanCache(id, subThreadCache);
    ASSERT_EQ(subThreadCache.GetCacheReuseCount(), 3);
    ASSERT_FALSE(uifirstManager_.markedClearCacheNodes_.empty());

    uifirstManager_.subthreadProcessingNode_.clear();
}

/**
 * @tc.name: NotifyUIStartingWindowTest001
 * @tc.desc: Test block first frame callback failed
 * @tc.type:FUNC
 * @tc.require: issue21674
 */
HWTEST_F(RSUifirstManagerTest2, NotifyUIStartingWindowTest001, TestSize.Level1)
{
    RSUifirstManager uifirstManager;
    NodeId id = 100;
    auto& nodeMap = mainThread_->context_->nodeMap;
    nodeMap.UnregisterRenderNode(id);
    // node is null
    uifirstManager.NotifyUIStartingWindow(id, false);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    nodeMap.RegisterRenderNode(surfaceNode);
    surfaceNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    // node is not leash
    uifirstManager.NotifyUIStartingWindow(surfaceNode->GetId(), false);
    ASSERT_FALSE(surfaceNode->IsWaitUifirstFirstFrame());

    nodeMap.UnregisterRenderNode(surfaceNode->GetId());
}

/**
 * @tc.name: NotifyUIStartingWindowTest002
 * @tc.desc: Test block first callback in different case
 * @tc.type:FUNC
 * @tc.require: issue21674
 */
HWTEST_F(RSUifirstManagerTest2, NotifyUIStartingWindowTest002, TestSize.Level1)
{
    RSUifirstManager uifirstManager;
    bool waitUifirstFrame;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    NodeId id = surfaceNode->GetId();
    auto& nodeMap = mainThread_->context_->nodeMap;
    nodeMap.RegisterRenderNode(surfaceNode);
    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    auto appWindow = RSTestUtil::CreateSurfaceNode();
    auto startingWindow = std::make_shared<RSCanvasRenderNode>(200);

    surfaceNode->AddChild(appWindow);
    surfaceNode->AddChild(startingWindow);
    surfaceNode->GenerateFullChildrenList();

    // test child is not main window
    appWindow->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    appWindow->SetWaitUifirstFirstFrame(true);
    waitUifirstFrame = false;
    uifirstManager.NotifyUIStartingWindow(id, waitUifirstFrame);
    ASSERT_TRUE(appWindow->IsWaitUifirstFirstFrame());

    // test no need to set when IsWaitUifirstFirstFrame equals waitUifirstFrame
    appWindow->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    appWindow->SetWaitUifirstFirstFrame(false);
    waitUifirstFrame = false;
    uifirstManager.NotifyUIStartingWindow(id, waitUifirstFrame);
    ASSERT_FALSE(appWindow->IsWaitUifirstFirstFrame());

    // test set not wait uifirst without check
    appWindow->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    appWindow->SetWaitUifirstFirstFrame(true);
    waitUifirstFrame = false;
    uifirstManager.NotifyUIStartingWindow(id, waitUifirstFrame, nullptr, false);
    ASSERT_FALSE(appWindow->IsWaitUifirstFirstFrame());

    // test check if child is drawn by subthread, but drawable is null
    appWindow->SetWaitUifirstFirstFrame(true);
    waitUifirstFrame = false;
    uifirstManager.NotifyUIStartingWindow(id, waitUifirstFrame, nullptr, true);
    ASSERT_FALSE(appWindow->IsWaitUifirstFirstFrame());

    // test when child not drawn by subthread, still wait uifirst
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    auto& subCache = surfaceDrawable->GetRsSubThreadCache();
    subCache.cacheCompletedSurfaceInfo_.processedSubSurfaceNodeIds.clear();
    appWindow->SetWaitUifirstFirstFrame(true);
    waitUifirstFrame = false;
    uifirstManager.NotifyUIStartingWindow(id, waitUifirstFrame, surfaceDrawable, true);
    ASSERT_TRUE(appWindow->IsWaitUifirstFirstFrame());

    // test success cancel wait uifirst frame
    appWindow->SetWaitUifirstFirstFrame(true);
    waitUifirstFrame = false;
    subCache.cacheCompletedSurfaceInfo_.processedNodeCount = 10;
    subCache.cacheCompletedSurfaceInfo_.processedSubSurfaceNodeIds.emplace(appWindow->GetId());
    uifirstManager.NotifyUIStartingWindow(id, waitUifirstFrame, surfaceDrawable, true);
    ASSERT_FALSE(appWindow->IsWaitUifirstFirstFrame());

    nodeMap.UnregisterRenderNode(id);
}

/**
 * @tc.name: CheckAndBlockFirstFrameCallbackTest001
 * @tc.desc: Test CheckAndBlockFirstFrameCallback
 * @tc.type:FUNC
 * @tc.require: issue21674
 */
HWTEST_F(RSUifirstManagerTest2, CheckAndBlockFirstFrameCallbackTest001, TestSize.Level1)
{
    RSUifirstManager uifirstManager;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->SetUifirstHasContentAppWindow(false);
    surfaceNode->SetUifirstStartingWindowId(0);
    // no starting window, not block
    uifirstManager.CheckAndBlockFirstFrameCallback(*surfaceNode);
    EXPECT_FALSE(surfaceNode->GetUifirstHasContentAppWindow());

    surfaceNode->SetUifirstHasContentAppWindow(true);
    surfaceNode->SetUifirstStartingWindowId(100);
    // aleary has content app window, not block
    uifirstManager.CheckAndBlockFirstFrameCallback(*surfaceNode);
    EXPECT_TRUE(surfaceNode->GetUifirstHasContentAppWindow());
}

/**
 * @tc.name: CheckAndBlockFirstFrameCallbackTest002
 * @tc.desc: Test block first frame callback in special case
 * @tc.type:FUNC
 * @tc.require: issue21674
 */
HWTEST_F(RSUifirstManagerTest2, CheckAndBlockFirstFrameCallbackTest002, TestSize.Level1)
{
    RSUifirstManager uifirstManager;
    NodeId startingWindowNodeId = 100;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->SetUifirstStartingWindowId(startingWindowNodeId);
    surfaceNode->SetUifirstHasContentAppWindow(false);
    // no child
    uifirstManager.CheckAndBlockFirstFrameCallback(*surfaceNode);
    EXPECT_FALSE(surfaceNode->GetUifirstHasContentAppWindow());

    auto appWindow = RSTestUtil::CreateSurfaceNode();
    auto startingWindow = std::make_shared<RSCanvasRenderNode>(startingWindowNodeId);

    surfaceNode->AddChild(appWindow);
    surfaceNode->AddChild(startingWindow);
    surfaceNode->GenerateFullChildrenList();
    appWindow->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    uifirstManager.CheckAndBlockFirstFrameCallback(*surfaceNode);
    EXPECT_FALSE(appWindow->IsWaitUifirstFirstFrame());
    EXPECT_FALSE(surfaceNode->GetUifirstHasContentAppWindow());

    appWindow->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    appWindow->leashPersistentId_ = 1;
    uifirstManager.CheckAndBlockFirstFrameCallback(*surfaceNode);
    EXPECT_FALSE(appWindow->IsWaitUifirstFirstFrame());
    EXPECT_FALSE(surfaceNode->GetUifirstHasContentAppWindow());

    // block first frame callback
    appWindow->leashPersistentId_ = INVALID_LEASH_PERSISTENTID;
    uifirstManager.CheckAndBlockFirstFrameCallback(*surfaceNode);
    EXPECT_TRUE(appWindow->IsWaitUifirstFirstFrame());
    EXPECT_TRUE(surfaceNode->GetUifirstHasContentAppWindow());
}
}