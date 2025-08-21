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
#include "feature/drm/rs_drm_util.h"
#include "graphic_feature_param_manager.h"
#include "gtest/gtest.h"

#include "common/rs_common_def.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrmUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrmUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSDrmUtilTest::TearDownTestCase() {}
void RSDrmUtilTest::SetUp() {}
void RSDrmUtilTest::TearDown() {}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: surfaceHandler is not nullptr
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node->GetRSSurfaceHandler(), nullptr);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer and create displaynode
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: surfaceHandler is not nullptr, node has ancestor
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node->GetRSSurfaceHandler(), nullptr);
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer, screenNode and output
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: surfaceHandler is not nullptr, node has ancestor
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest003, TestSize.Level1)
{
    ScreenId screenId = 0;
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, HdiOutput::CreateHdiOutput(screenId), nullptr);
    rsScreen->screenType_ = EXTERNAL_TYPE_SCREEN;
    auto screenManager = CreateOrGetScreenManager();
    screenManager->MockHdiScreenConnected(rsScreen);

    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, screenId, rsContext->weak_from_this());
    ASSERT_NE(screenNode, nullptr);
    screenNode->screenId_ = screenId;

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer, screenNode and output
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: output, surfaceHandler is not nullptr, node has ancestor
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest004, TestSize.Level1)
{
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    hdiOutput->SetProtectedFrameBufferState(true);
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->screenType_ = EXTERNAL_TYPE_SCREEN;
    auto screenManager = CreateOrGetScreenManager();
    screenManager->MockHdiScreenConnected(rsScreen);

    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(1, screenId, rsContext->weak_from_this());
    ASSERT_NE(screenNode, nullptr);
    screenNode->screenId_ = screenId;
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer, screenNode and output
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: output, node has ancestor, surfaceHandler getbuffer is nullptr,
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest005, TestSize.Level1)
{
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    hdiOutput->SetProtectedFrameBufferState(false);
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->screenType_ = EXTERNAL_TYPE_SCREEN;
    auto screenManager = CreateOrGetScreenManager();
    screenManager->MockHdiScreenConnected(rsScreen);

    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(1, screenId, rsContext->weak_from_this());
    ASSERT_NE(screenNode, nullptr);
    screenNode->screenId_ = screenId;
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(2);
    surfaceHandler->buffer_.buffer = nullptr;
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter001, TestSize.Level1)
{
    DRMParam::AddBlackList("SCBCloneWallpaper");
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBCloneWallpaper";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 110);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter002, TestSize.Level1)
{
    DRMParam::AddBlackList("SCBCloneWallpaper");
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBCloneWallpaper";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 10);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter003, TestSize.Level1)
{
    DRMParam::AddBlackList("SCBCloneWallpaper");
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBCloneWallpaper";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 10);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter004, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBBannerNotification";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 10);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);
    drmNode->instanceRootNodeId_ = drmNode->GetId();
    Occlusion::Region region1({120, 50, 1000, 1500});
    drmNode->SetVisibleRegion(region1);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter005, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBSmartDock";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 10);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    surfaceNode->renderProperties_.SetBgBrightnessSaturation(1.5f);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);
    drmNode->instanceRootNodeId_ = drmNode->GetId();
    Occlusion::Region region1({120, 50, 1000, 1500});
    drmNode->SetVisibleRegion(region1);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter006, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBSmartDock";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 10);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);
    drmNode->instanceRootNodeId_ = drmNode->GetId();
    Occlusion::Region region1({120, 50, 1000, 1500});
    drmNode->SetVisibleRegion(region1);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(2, 2, 3, 3);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter007, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBBannerNotification";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 10);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);
    drmNode->instanceRootNodeId_ = drmNode->GetId();
    Occlusion::Region region1({0, 0, 0, 0});
    drmNode->SetVisibleRegion(region1);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
}

/**
 * Function: MarkBlurIntersectWithDRMForAllParentFilter
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRMForAllParentFilter
 *                  3. result: surfaceNode drmNodes andd screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRMForAllParentFilter008, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBBannerNotification";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    Color color(10, 10, 10, 10);
    surfaceNode->renderProperties_.SetBackgroundBlurMaskColor(color);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);
    drmNode->instanceRootNodeId_ = drmNode->GetId();
    Occlusion::Region region1({120, 50, 1000, 1500});
    drmNode->SetVisibleRegion(region1);

    // let drm intersect with blur
    surfaceNode->filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkBlurIntersectWithDRMForAllParentFilter(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
}
}