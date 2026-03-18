/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
#include "composer/composer_client/pipeline/rs_composer_client_manager.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace {
constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
}

namespace OHOS::Rosen {
class RSDrmUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static NodeId GenerateNodeId()
    {
        static NodeId nextId = 1;
        return nextId++;
    }

    static ScreenId GenerateScreenId()
    {
        static ScreenId nextId = 1;
        return nextId++;
    }

    std::shared_ptr<DrawableV2::RSScreenRenderNodeDrawable> CreateScreenDrawable();
};

void RSDrmUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSDrmUtilTest::TearDownTestCase() {}
void RSDrmUtilTest::SetUp() {}
void RSDrmUtilTest::TearDown() {}

std::shared_ptr<DrawableV2::RSScreenRenderNodeDrawable> RSDrmUtilTest::CreateScreenDrawable()
{
    static NodeId nodeId = GenerateNodeId();
    ScreenId screenId = GenerateScreenId();
    auto screenNode = std::make_shared<RSScreenRenderNode>(nodeId, screenId);
    auto screenDrawable = std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(std::move(screenNode));
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    return screenDrawable;
}

/**
 * Function: MarkBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkBlurIntersectWithDRM001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode;
    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    RSDrmUtil::MarkBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    ASSERT_EQ(surfaceNode, nullptr);
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBCloneWallpaper";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    std::shared_ptr<RSScreenRenderNode> screenNode;
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    ASSERT_EQ(screenNode, nullptr);
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM002, TestSize.Level1)
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
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM003, TestSize.Level1)
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
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM004, TestSize.Level1)
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
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM005, TestSize.Level1)
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
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM006, TestSize.Level1)
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
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM007, TestSize.Level1)
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

    // let drm not intersect with blur
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(2, 2, 3, 3);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM008, TestSize.Level1)
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
    // let drm visibleRegion empty
    Occlusion::Region region1({0, 0, 0, 0});
    drmNode->SetVisibleRegion(region1);

    // let drm intersect with blur
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: MarkAllBlurIntersectWithDRM
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, screenNode and drmNode
 *                  2. operation: MarkAllBlurIntersectWithDRM
 *                  3. result: surfaceNode drmNodes and screenNode are not nullptr
 */
HWTEST_F(RSDrmUtilTest, MarkAllBlurIntersectWithDRM009, TestSize.Level1)
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
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(drmNode);

    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(drmNode);
    screenNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> drmNodes;
    drmNodes.push_back(drmNode);
    RSDrmUtil::MarkAllBlurIntersectWithDRM(surfaceNode, drmNodes, screenNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(drmNode->GetId());
    ASSERT_FALSE(drmNodes.empty());
}

/**
 * Function: IsDRMNodesOnTheTree
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, set normal parameters
 *                  2. operation: IsDRMNodesOnTheTree
 *                  3. result: drmNodes is not OnTheTree
 */
HWTEST_F(RSDrmUtilTest, IsDRMNodesOnTheTree001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->firstLevelNodeId_ = surfaceNode->GetId();
    surfaceNode->isOnTheTree_ = true;
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    RSDrmUtil::CollectDrmNodes(surfaceNode);
    EXPECT_TRUE(RSDrmUtil::IsDRMNodesOnTheTree());
    RSDrmUtil::ClearDrmNodes();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(surfaceNode->GetId());
}

/**
 * Function: IsDRMNodesOnTheTree
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, set abnormal parameters
 *                  2. operation: IsDRMNodesOnTheTree
 *                  3. result: drmNodes is not OnTheTree
 */
HWTEST_F(RSDrmUtilTest, IsDRMNodesOnTheTree002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->firstLevelNodeId_ = surfaceNode->GetId();
    surfaceNode->isOnTheTree_ = true;
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    RSDrmUtil::CollectDrmNodes(surfaceNode);
    EXPECT_FALSE(RSDrmUtil::IsDRMNodesOnTheTree());
    RSDrmUtil::ClearDrmNodes();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(surfaceNode->GetId());
}

/**
 * Function: IsDRMNodesOnTheTree
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNode, set abnormal parameters
 *                  2. operation: IsDRMNodesOnTheTree
 *                  3. result: drmNodes is not OnTheTree
 */
HWTEST_F(RSDrmUtilTest, IsDRMNodesOnTheTree003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->firstLevelNodeId_ = surfaceNode->GetId();
    surfaceNode->isOnTheTree_ = true;
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    RSDrmUtil::CollectDrmNodes(surfaceNode);
    surfaceNode->isOnTheTree_ = false;
    EXPECT_FALSE(RSDrmUtil::IsDRMNodesOnTheTree());
    RSDrmUtil::ClearDrmNodes();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(surfaceNode->GetId());
}

/**
 * Function: PreAllocProtectedFrameBuffers
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: surfaceNode has no ancestor screen node
 *                  2. operation: PreAllocProtectedFrameBuffers
 *                  3. result: function returns without crash
 */
HWTEST_F(RSDrmUtilTest, PreAllocProtectedFrameBuffers002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<SurfaceBuffer> buffer = nullptr;
    std::shared_ptr<RSComposerClientManager> clientManager = nullptr;
    RSDrmUtil::PreAllocProtectedFrameBuffers(surfaceNode, buffer, clientManager);
}

/**
 * Function: PreAllocProtectedFrameBuffers
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: surfaceNode has screen node, clientManager is nullptr
 *                  2. operation: PreAllocProtectedFrameBuffers
 *                  3. result: AddScreenHasProtectedLayerSet is called
 */
HWTEST_F(RSDrmUtilTest, PreAllocProtectedFrameBuffers003, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    
    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    
    surfaceNode->ancestorScreenNode_ = screenNode;
    sptr<SurfaceBuffer> buffer = nullptr;
    std::shared_ptr<RSComposerClientManager> clientManager = nullptr;
    RSDrmUtil::PreAllocProtectedFrameBuffers(surfaceNode, buffer, clientManager);
}

/**
 * Function: PreAllocProtectedFrameBuffers
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: surfaceNode has screen node, clientManager is valid
 *                  2. operation: PreAllocProtectedFrameBuffers
 *                  3. result: AddScreenHasProtectedLayerSet and clientManager->PreAllocProtectedFrameBuffers are called
 */
HWTEST_F(RSDrmUtilTest, PreAllocProtectedFrameBuffers004, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    
    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    
    surfaceNode->ancestorScreenNode_ = screenNode;
    sptr<SurfaceBuffer> buffer = nullptr;
    auto clientManager = std::make_shared<RSComposerClientManager>();
    RSDrmUtil::PreAllocProtectedFrameBuffers(surfaceNode, buffer, clientManager);
}

/**
 * Function: PreAllocProtectedFrameBuffers
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: surfaceNode has screen node, buffer is valid, clientManager is valid
 *                  2. operation: PreAllocProtectedFrameBuffers
 *                  3. result: AddScreenHasProtectedLayerSet and clientManager->PreAllocProtectedFrameBuffers are called
 */
HWTEST_F(RSDrmUtilTest, PreAllocProtectedFrameBuffers005, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    
    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    
    node->ancestorScreenNode_ = screenNode;

    auto clientManager = std::make_shared<RSComposerClientManager>();
    RSDrmUtil::PreAllocProtectedFrameBuffers(node, node->GetRSSurfaceHandler()->GetBuffer(), clientManager);
}

/**
 * Function: DealWithDRMNodes
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: surfaceNode, buffer, clientManager are valid
 *                  2. operation: DealWithDRMNodes
 *                  3. result: CollectDrmNodes and PreAllocProtectedFrameBuffers are called
 */
HWTEST_F(RSDrmUtilTest, DealWithDRMNodes001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    
    sptr<SurfaceBuffer> buffer = nullptr;
    auto clientManager = std::make_shared<RSComposerClientManager>();
    RSDrmUtil::DealWithDRMNodes(surfaceNode, buffer, clientManager);
    RSDrmUtil::ClearDrmNodes();
}

/**
 * Function: DealWithDRMNodes
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: surfaceNode has screen node, buffer, clientManager are valid
 *                  2. operation: DealWithDRMNodes
 *                  3. result: CollectDrmNodes and PreAllocProtectedFrameBuffers are called
 */
HWTEST_F(RSDrmUtilTest, DealWithDRMNodes002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    
    NodeId id = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(screenNode, nullptr);
    
    surfaceNode->ancestorScreenNode_ = screenNode;
    
    sptr<SurfaceBuffer> buffer = nullptr;
    auto clientManager = std::make_shared<RSComposerClientManager>();
    RSDrmUtil::DealWithDRMNodes(surfaceNode, buffer, clientManager);
    RSDrmUtil::ClearDrmNodes();
}

/**
 * Function: HasDRMInVirtualScreen
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: ancestorDrawable is nullptr
 *                  2. operation: HasDRMInVirtualScreen
 *                  3. result: return false
 */
HWTEST_F(RSDrmUtilTest, HasDRMInVirtualScreen001, TestSize.Level2)
{
    RSSurfaceRenderParams surfaceParams(GenerateNodeId());
    surfaceParams.ancestorScreenDrawable_.reset();

    Drawing::Canvas canvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    RSPaintFilterCanvas filterCanvas(&canvas);

    auto result = RSDrmUtil::HasDRMInVirtualScreen(filterCanvas, surfaceParams);
    ASSERT_FALSE(result);
}

/**
 * Function: HasDRMInVirtualScreen
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: params is nullptr
 *                  2. operation: HasDRMInVirtualScreen
 *                  3. result: return false
 */
HWTEST_F(RSDrmUtilTest, HasDRMInVirtualScreen002, TestSize.Level2)
{
    auto screenDrawable = CreateScreenDrawable();
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = nullptr;

    RSSurfaceRenderParams surfaceParams(GenerateNodeId());
    surfaceParams.ancestorScreenDrawable_ = screenDrawable;

    Drawing::Canvas canvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    RSPaintFilterCanvas filterCanvas(&canvas);

    auto result = RSDrmUtil::HasDRMInVirtualScreen(filterCanvas, surfaceParams);
    ASSERT_FALSE(result);
}

/**
 * Function: HasDRMInVirtualScreen
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: screen is not virtual
 *                  2. operation: HasDRMInVirtualScreen
 *                  3. result: return false
 */
HWTEST_F(RSDrmUtilTest, HasDRMInVirtualScreen003, TestSize.Level2)
{
    auto screenDrawable = CreateScreenDrawable();
    ASSERT_NE(screenDrawable, nullptr);

    RSSurfaceRenderParams surfaceParams(GenerateNodeId());
    surfaceParams.ancestorScreenDrawable_ = screenDrawable;

    Drawing::Canvas canvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    RSPaintFilterCanvas filterCanvas(&canvas);

    auto result = RSDrmUtil::HasDRMInVirtualScreen(filterCanvas, surfaceParams);
    ASSERT_FALSE(result);
}

/**
 * Function: HasDRMInVirtualScreen
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: no protected layer
 *                  2. operation: HasDRMInVirtualScreen
 *                  3. result: return false
 */
HWTEST_F(RSDrmUtilTest, HasDRMInVirtualScreen004, TestSize.Level2)
{
    auto screenDrawable = CreateScreenDrawable();
    ASSERT_NE(screenDrawable, nullptr);

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->renderParams_.get());
    ASSERT_NE(screenParams, nullptr);
    screenParams->screenProperty_.Set(ScreenPropertyType::IS_VIRTUAL,
        sptr<ScreenPropertyBase>(new ScreenProperty<bool>(true)));

    RSSurfaceRenderParams surfaceParams(GenerateNodeId());
    surfaceParams.ancestorScreenDrawable_ = screenDrawable;

    Drawing::Canvas canvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    RSPaintFilterCanvas filterCanvas(&canvas);

    auto result = RSDrmUtil::HasDRMInVirtualScreen(filterCanvas, surfaceParams);
    ASSERT_FALSE(result);
}

/**
 * Function: HasDRMInVirtualScreen
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: virtual screen with protected layer
 *                  2. operation: HasDRMInVirtualScreen
 *                  3. result: return true
 */
HWTEST_F(RSDrmUtilTest, HasDRMInVirtualScreen005, TestSize.Level2)
{
    auto screenDrawable = CreateScreenDrawable();
    ASSERT_NE(screenDrawable, nullptr);

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->renderParams_.get());
    ASSERT_NE(screenParams, nullptr);
    screenParams->screenProperty_.Set(ScreenPropertyType::IS_VIRTUAL,
        sptr<ScreenPropertyBase>(new ScreenProperty<bool>(true)));

    RSSurfaceRenderParams surfaceParams(GenerateNodeId());
    surfaceParams.ancestorScreenDrawable_ = screenDrawable;
    surfaceParams.specialLayerManager_.Set(SpecialLayerType::PROTECTED, true);

    Drawing::Canvas canvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    RSPaintFilterCanvas filterCanvas(&canvas);

    auto result = RSDrmUtil::HasDRMInVirtualScreen(filterCanvas, surfaceParams);
    ASSERT_TRUE(result);
}
}