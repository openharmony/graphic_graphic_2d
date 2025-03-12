/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <memory>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "mock/mock_matrix.h"
#include "rs_test_util.h"
#include "system/rs_system_parameters.h"

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"

using namespace testing;
using namespace testing::ext;

namespace {
    const std::string DEFAULT_NODE_NAME = "1";
    const std::string INVALID_NODE_NAME = "2";
    const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
    const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
    constexpr int MAX_ALPHA = 255;
}

namespace OHOS::Rosen {
class RSUniRenderVisitorGlobalDirtyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Mock::MatrixMock* matrixMock_;
};

void RSUniRenderVisitorGlobalDirtyTest::SetUpTestCase()
{
    matrixMock_ = Mock::MatrixMock::GetInstance();
    EXPECT_CALL(*matrixMock_, GetMinMaxScales(_)).WillOnce(testing::Return(false));
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderVisitorGlobalDirtyTest::TearDownTestCase() {}
void RSUniRenderVisitorGlobalDirtyTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RSUniRenderVisitorGlobalDirtyTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

/*
 * @tc.name: CheckMergeGlobalFilterForDisplay
 * @tc.desc: Test RSUniRenderVisitorGlobalDirtyTest.CheckMergeGlobalFilterForDisplay
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeGlobalFilterForDisplay001, TestSize.Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    nodeMap.renderNodeMap_[id0] = nullptr;
    NodeId id1 = 1;
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[id1] = node1;
    NodeId id2 = 2;
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    rsUniRenderVisitor->containerFilter_.insert({node2->GetId(), rect});
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    auto dirtyRegion = Occlusion::Region{ Occlusion::Rect{ rect } };
    rsUniRenderVisitor->CheckMergeGlobalFilterForDisplay(dirtyRegion);
}

/*
 * @tc.name: UpdateOccludedStatusWithFilterNode
 * @tc.desc: Test RSUniRenderVisitorGlobalDirtyTest.UpdateOccludedStatusWithFilterNode while surface node nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, UpdateOccludedStatusWithFilterNode001, TestSize.Level2)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateOccludedStatusWithFilterNode(surfaceNode);
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion003
 * @tc.desc: Test MergeRemovedChildDirtyRegion while curSurfaceDirtyManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;

    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSDisplayRenderNode>(0, config, rsContext->weak_from_this());
    auto dirtyManager = displayNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = nullptr;
    rsUniRenderVisitor->curDisplayDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), RectI(0, 0, 0, 0));
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion002
 * @tc.desc: Test MergeRemovedChildDirtyRegion while has removed child which dirty rect isn't empty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), RectI(0, 0, 0, 0));
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion004
 * @tc.desc: Test MergeRemovedChildDirtyRegion while need map
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty004
 * @tc.desc: Test while app window node skip in calculate global dirty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty004, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto dirtyManager = surfaceNode->GetDirtyManager();
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_RECT);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty001
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty without curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->containerFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->containerFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty002
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with non-transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(MAX_ALPHA);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->globalFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->globalFilter_.empty());
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion005
 * @tc.desc: Test MergeRemovedChildDirtyRegion while node's bounds geometry is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;
    surfaceNode->GetMutableRenderProperties().boundsGeo_ = nullptr;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion006
 * @tc.desc: Test MergeRemovedChildDirtyRegion while dirtyManager is target for FDX
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;
    surfaceNode->GetDirtyManager()->MarkAsTargetForDfx();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: PrevalidateHwcNode001
 * @tc.desc: Test while prevalidate hwcNode disable
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, PrevalidateHwcNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSUniHwcPrevalidateUtil::GetInstance().isPrevalidateHwcNodeEnable_ = false;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->PrevalidateHwcNode();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty002
 * @tc.desc: Test while surface node has hardware enabled node as child
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty002, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty003
 * @tc.desc: Test while current frame dirty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty003, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto dirtyManager = surfaceNode->GetDirtyManager();
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_RECT);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty003
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(0);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->transparentCleanFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->transparentCleanFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty004
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent(needDrawBehindWindow) curSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->renderContent_->renderProperties_.needDrawBehindWindow_ = true;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    RectI dirtyRegion(0, 0, 100, 100);
    dirtyManager->currentFrameDirtyRegion_ = dirtyRegion;
    RectI globalFilterRect(0, 0, 20, 20);
    rsUniRenderVisitor->transparentCleanFilter_ = {};
    rsUniRenderVisitor->curDisplayDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsUniRenderVisitor->curDisplayDirtyManager_->currentFrameDirtyRegion_ = {};

    ASSERT_TRUE(rsUniRenderVisitor->transparentCleanFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*surfaceNode, *dirtyManager, globalFilterRect);
    ASSERT_FALSE(rsUniRenderVisitor->transparentCleanFilter_.empty());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha001
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter nullptr / eqeual nodeid / hwcNodes empty.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter001, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(std::make_shared<RSSurfaceRenderNode>(node->GetId()));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter, child node force disabled hardware.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter002, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node and surface node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(++surfaceId);
    childNode->isHardwareForcedDisabled_ = true;
    surfaceNode->AddChildHardwareEnabledNode(childNode);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
    ASSERT_TRUE(childNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha002
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareForcedDisabledState(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha003
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha003, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetNodeHasBackgroundColorAlpha(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode1));
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode2));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha004
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha004, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/*
 * @tc.name: UpdateOccludedStatusWithFilterNode
 * @tc.desc: Test RSUniRenderVisitorGlobalDirtyTest.UpdateOccludedStatusWithFilterNode with surfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, UpdateOccludedStatusWithFilterNode002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    ASSERT_NE(surfaceNode1->renderContent_, nullptr);
    surfaceNode1->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    surfaceNode1->isOccludedByFilterCache_ = true;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id1 = 2;
    auto filterNode1 = std::make_shared<RSRenderNode>(id1);
    ASSERT_NE(filterNode1, nullptr);
    nodeMap.renderNodeMap_[id1] = filterNode1;
    NodeId id2 = 3;
    auto filterNode2 = std::make_shared<RSRenderNode>(id2);
    ASSERT_NE(filterNode2, nullptr);
    ASSERT_NE(filterNode2->renderContent_, nullptr);
    filterNode2->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    nodeMap.renderNodeMap_[id2] = filterNode2;
    surfaceNode1->visibleFilterChild_.emplace_back(filterNode1->GetId());
    surfaceNode1->visibleFilterChild_.emplace_back(filterNode2->GetId());

    ASSERT_FALSE(filterNode1->isOccluded_);
    ASSERT_FALSE(filterNode2->isOccluded_);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateOccludedStatusWithFilterNode(surfaceNode1);
    ASSERT_TRUE(filterNode2->isOccluded_);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentFilter
 * @tc.desc: Test RSUniRenderVisitorGlobalDirtyTest.CheckMergeDisplayDirtyByTransparentFilter with mainWindow
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeDisplayDirtyByTransparentFilter001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(surfaceNode->GetVisibleRegion().IsEmpty());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 260;
    uint32_t height = 600;
    RectI rect{left, top, width, height};
    Occlusion::Region region{rect};
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, region);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentFilter
 * @tc.desc: Test RSUniRenderVisitorGlobalDirtyTest.CheckMergeDisplayDirtyByTransparentFilter
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeDisplayDirtyByTransparentFilter002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    nodeMap.renderNodeMap_[id0] = nullptr;
    NodeId id1 = 1;
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[id1] = node1;
    NodeId id2 = 2;
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left1 = 0;
    uint32_t top1 = 0;
    uint32_t width1 = 260;
    uint32_t height1 = 600;
    RectI rect1{left1, top1, width1, height1};
    Occlusion::Region region1{rect1};
    uint32_t left2 = 0;
    uint32_t top2 = 0;
    uint32_t width2 = 200;
    uint32_t height2 = 300;
    RectI rect2{left2, top2, width2, height2};
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id0, rect2});
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id1, rect2});
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id2, rect2});
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    ASSERT_NE(node2->renderContent_, nullptr);
    node2->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    node2->renderContent_->renderProperties_.SetFilter(filter);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);

    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, region1);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparent001
 * @tc.desc: Test CheckMergeDisplayDirtyByTransparent with transparent node
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
*/
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeDisplayDirtyByTransparent001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode to transparent, create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(DEFAULT_RECT);
    rsSurfaceRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    Occlusion::Region visibleRegion{DEFAULT_RECT};
    rsSurfaceRenderNode->SetVisibleRegion(visibleRegion);
    auto dirtyRect = rsSurfaceRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion();
    ASSERT_TRUE(rsSurfaceRenderNode->GetVisibleRegion().IsIntersectWith(dirtyRect));
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparent(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByZorderChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByZorderChanged with Z order change node
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
*/
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeDisplayDirtyByZorderChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode Zorder change
    rsSurfaceRenderNode->zOrderChanged_ = true;
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByZorderChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByPosChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByPosChanged with position change
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
*/
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeDisplayDirtyByPosChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode pos change
    rsDisplayRenderNode->UpdateSurfaceNodePos(config.id, RectI());
    rsDisplayRenderNode->ClearCurrentSurfacePos();
    rsDisplayRenderNode->UpdateSurfaceNodePos(config.id, DEFAULT_RECT);

    rsUniRenderVisitor->CheckMergeDisplayDirtyByPosChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByShadowChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByShadowChanged with shadow change
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
*/
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeDisplayDirtyByShadowChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode shadow change
    rsSurfaceRenderNode->isShadowValidLastFrame_ = true;
    rsSurfaceRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByShadowChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentRegions
 * @tc.desc: Test RSUniRenderVisitorGlobalDirtyTest.CheckMergeDisplayDirtyByTransparentRegions with no container window
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckMergeDisplayDirtyByTransparentRegions002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->isNodeDirty_ = true;
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 260;
    uint32_t height = 600;
    Occlusion::Rect rect1{left, top, width, height};
    Occlusion::Region region1{rect1};
    surfaceNode->transparentRegion_ = region1;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentRegions(*surfaceNode);
}

/**
 * @tc.name: CollectEffectInfo001
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with not parent node.
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CollectEffectInfo001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    rsUniRenderVisitor->CollectEffectInfo(*node);
}

/**
 * @tc.name: CollectEffectInfo002
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, need filter
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CollectEffectInfo002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    constexpr NodeId parentNodeId = 2;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    auto parent = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parent, nullptr);
    node->InitRenderParams();
    parent->InitRenderParams();
    parent->AddChild(node);
    node->GetMutableRenderProperties().needFilter_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleFilter());
}

/**
 * @tc.name: CollectEffectInfo003
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, useEffect
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CollectEffectInfo003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    constexpr NodeId parentNodeId = 2;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    auto parent = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parent, nullptr);
    node->InitRenderParams();
    parent->InitRenderParams();
    parent->AddChild(node);
    node->GetMutableRenderProperties().useEffect_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleEffect());
}

/**
 * @tc.name: CheckFilterCacheFullyCovered001
 * @tc.desc: Test RSUnitRenderVisitorTest.CheckFilterCacheFullyCovered with mutiple nodes.
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CheckFilterCacheFullyCovered001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    constexpr NodeId id0 = 0;
    nodeMap.renderNodeMap_[id0] = nullptr;
    constexpr NodeId id1 = 1;
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[id1] = node1;
    ASSERT_NE(node1, nullptr);
    constexpr NodeId id2 = 2;
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[id2] = node2;
    ASSERT_NE(node2, nullptr);

    ASSERT_NE(node2->renderContent_, nullptr);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    node2->renderContent_->renderProperties_.SetBackgroundFilter(filter);

    surfaceNode1->visibleFilterChild_.emplace_back(node1->GetId());
    surfaceNode1->visibleFilterChild_.emplace_back(node2->GetId());
    rsUniRenderVisitor->CheckFilterCacheFullyCovered(surfaceNode1);
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty001
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty without curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CollectFilterInfoAndUpdateDirty001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->containerFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->containerFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty002
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with non-transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CollectFilterInfoAndUpdateDirty002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(MAX_ALPHA);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->globalFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->globalFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty003
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
 */
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, CollectFilterInfoAndUpdateDirty003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(0);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->transparentCleanFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->transparentCleanFilter_.empty());
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty001
 * @tc.desc: Test UpdateSurfaceDirtyAndGlobalDirty
 * @tc.type: FUNC
 * @tc.require: issueIB7K6W
*/
HWTEST_F(RSUniRenderVisitorGlobalDirtyTest, UpdateSurfaceDirtyAndGlobalDirty001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    // 80, 2560, 1600. dummy value used to create rectangle with non-zero dimension
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_EQ(rsUniRenderVisitor->curDisplayDirtyManager_->dirtyRegion_.left_, 0);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_006
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and
 *           parent's parent is not null.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_006, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    parentNode->AddChild(childNode);
    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_EQ(parent->GetId(), parentNodeId);

    NodeId grandparentNodeId = 3;
    auto grandparentNode = std::make_shared<RSSurfaceRenderNode>(grandparentNodeId);
    ASSERT_NE(grandparentNode, nullptr);
    grandparentNode->InitRenderParams();
    grandparentNode->AddChild(parentNode);

    EXPECT_NE(parent->GetType(), RSRenderNodeType::DISPLAY_NODE);
    auto grandparent = parent->GetParent().lock();
    ASSERT_NE(grandparent, nullptr);
    ASSERT_EQ(grandparent->GetId(), grandparentNodeId);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    NodeId displayNodeId = 4;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: CheckMergeGlobalFilterForDisplay
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeGlobalFilterForDisplay
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeGlobalFilterForDisplay001, TestSize.Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    pid_t pid0 = ExtractPid(id0);
    nodeMap.renderNodeMap_[pid0][id0] = nullptr;
    NodeId id1 = 1;
    pid_t pid1 = ExtractPid(id1);
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[pid1][id1] = node1;
    NodeId id2 = 2;
    pid_t pid2 = ExtractPid(id2);
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[pid2][id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    rsUniRenderVisitor->containerFilter_.insert({node2->GetId(), rect});
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    auto dirtyRegion = Occlusion::Region{ Occlusion::Rect{ rect } };
    rsUniRenderVisitor->CheckMergeGlobalFilterForDisplay(dirtyRegion);
}

/*
 * @tc.name: UpdateHwcNodeEnable_003
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodePtr is not on the tree.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    NodeId childId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childId);
    childNode->SetIsOnTheTree(false);
    ASSERT_FALSE(childNode->IsOnTheTree());
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}


/*
 * @tc.name: UpdateHwcNodeEnable_001
 * @tc.desc: Test UpdateHwcNodeEnable when surfaceNode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeEnable_002
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->ResetChildHardwareEnabledNodes();
    ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_004
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is false.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    NodeId fakeParentNodeId = 3;
    auto fakeParentNode = std::make_shared<RSSurfaceRenderNode>(fakeParentNodeId);
    ASSERT_NE(fakeParentNode, nullptr);
    fakeParentNode->AddChild(childNode);

    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(parent->GetId(), parentNodeId);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_005
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is true.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    parentNode->AddChild(childNode);
    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_EQ(parent->GetId(), parentNodeId);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 0);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_002
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode has corner radius and anco force do direct.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(true);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(0x0001));
    ASSERT_TRUE(surfaceNode->GetAncoForceDoDirect());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, false);
    EXPECT_EQ(hwcRects.size(), 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_003
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode intersects with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_TRUE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_004
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode does not intersect with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI());
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_FALSE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_001
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           curSurfaceNode_ is null or hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        rsUniRenderVisitor->curSurfaceNode_ = nullptr;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        ASSERT_NE(surfaceNode, nullptr);
        surfaceNode->ResetChildHardwareEnabledNodes();
        ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_002
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           hwcNodePtr is nullptr or hwcNodePtr is not on the tree or GetCalcRectInPrepare is true.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        childNode = nullptr;
        ASSERT_EQ(childNode, nullptr);
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        ASSERT_NE(childNode, nullptr);
        childNode->SetIsOnTheTree(false);
        ASSERT_FALSE(childNode->IsOnTheTree());
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        ASSERT_NE(childNode, nullptr);
        childNode->SetIsOnTheTree(true);
        ASSERT_TRUE(childNode->IsOnTheTree());
        childNode->SetCalcRectInPrepare(true);
        ASSERT_TRUE(childNode->GetCalcRectInPrepare());
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_003
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and parent is null.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());
    ASSERT_EQ(childNode->GetParent().lock(), nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}
} // OHOS::Rosen