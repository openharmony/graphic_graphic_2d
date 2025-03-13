/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "ipc_callbacks/buffer_clear_callback_proxy.h"
#include "pipeline/rs_context.h"
#include "params/rs_surface_render_params.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNodeUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
    uint8_t MAX_ALPHA = 255;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
};

void RSSurfaceRenderNodeUnitTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSSurfaceRenderNodeUnitTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSSurfaceRenderNodeUnitTest::SetUp() {}
void RSSurfaceRenderNodeUnitTest::TearDown() {}

/**
 * @tc.name: NodeCostTest
 * @tc.desc: test GetNodeCost and SetNodeCost
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, NodeCostTest, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    auto result = surfaceRenderNode.nodeCost_;
    ASSERT_EQ(0, result);
    surfaceRenderNode.SetNodeCost(6);
    result = surfaceRenderNode.GetNodeCost();
    ASSERT_EQ(6, result);
}

/**
 * @tc.name: FingerprintTest
 * @tc.desc: test SetFingerprint and GetFingerprint
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, FingerprintTest, TestSize.Level1)
{
    RSSurfaceRenderNode surfaceRenderNode(id, context);
    surfaceRenderNode.SetFingerprint(true);
    auto result = surfaceRenderNode.GetFingerprint();
    ASSERT_EQ(true, result);
    surfaceRenderNode.SetFingerprint(false);
    result = surfaceRenderNode.GetFingerprint();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: HDRPresentTest
 * @tc.desc: test SetHDRPresent and GetHDRPresent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, HDRPresentTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(id + 2, rsContext);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(leashWindowNode, nullptr);

    NodeId childNodeId = childNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    NodeId leashWindowNodeId = leashWindowNode->GetId();
    pid_t leashWindowNodePid = ExtractPid(leashWindowNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[childNodePid][childNodeId] = childNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[leashWindowNodePid][leashWindowNodeId] = leashWindowNode;

    parentNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    childNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    leashWindowNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    leashWindowNode->AddChild(parentNode);
    parentNode->AddChild(childNode);
    leashWindowNode->SetIsOnTheTree(true);
    parentNode->SetIsOnTheTree(true);
    childNode->SetIsOnTheTree(true);
    
    childNode->SetHDRPresent(false);
    EXPECT_EQ(childNode->GetHDRPresent(), false);
    leashWindowNode->SetHDRPresent(true);
    EXPECT_EQ(leashWindowNode->GetHDRPresent(), true);
}

/**
 * @tc.name: CollectSurfaceTest001
 * @tc.desc: test CollectSurface api
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, CollectSurfaceTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> rsBaseRenderNode;
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    node->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    node->CollectSurface(rsBaseRenderNode, vec, isUniRender, false);
    ASSERT_FALSE(vec.empty());
}

/**
 * @tc.name: CollectSurfaceTest002
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, CollectSurfaceTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> rsBaseRenderNode;
    std::vector<RSBaseRenderNode::SharedPtr> vec;
    bool isUniRender = true;
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->CollectSurface(rsBaseRenderNode, vec, isUniRender, true);
    ASSERT_FALSE(vec.empty());
}


/**
 * @tc.name: SurfaceNodeType001
 * @tc.desc: test SetSurfaceNodeType and GetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SurfaceNodeType001, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    EXPECT_TRUE(isSameType);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_FALSE(isSameType);
}

/**
 * @tc.name: SurfaceNodeType002
 * @tc.desc: test SetSurfaceNodeType and GetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SurfaceNodeType002, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_TRUE(isSameType);
    node->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE);
    isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::DEFAULT);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: SurfaceNodeType003
 * @tc.desc: test SetSurfaceNodeType and GetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SurfaceNodeType003, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: SurfaceNodeType004
 * @tc.desc: test SetSurfaceNodeType and GetSurfaceNodeType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SurfaceNodeType004, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    bool isSameType = (node->GetSurfaceNodeType() == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE);
    EXPECT_TRUE(isSameType);
}

/**
 * @tc.name: CheckIfOcclusionReusableTest
 * @tc.desc: test CheckIfOcclusionReusable api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, CheckIfOcclusionReusableTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    std::queue<NodeId> surfaceNodesIds;
    ASSERT_TRUE(surfaceNode->CheckIfOcclusionReusable(surfaceNodesIds));
    surfaceNodesIds.push(id + 1);
    ASSERT_TRUE(surfaceNode->CheckIfOcclusionReusable(surfaceNodesIds));
    surfaceNodesIds.push(id);
    ASSERT_FALSE(surfaceNode->CheckIfOcclusionReusable(surfaceNodesIds));
}

/**
 * @tc.name: CheckAndUpdateOpaqueRegionTest
 * @tc.desc: test CheckAndUpdateOpaqueRegion api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, CheckAndUpdateOpaqueRegionTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    RectI screeninfo { 0, 0, 100, 100 };
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    node->CheckAndUpdateOpaqueRegion(screeninfo, screenRotation, true);
    EXPECT_FALSE(node->IsOpaqueRegionChanged());
}

/**
 * @tc.name: UpdateChildrenFilterRectsTest
 * @tc.desc: test UpdateChildrenFilterRects api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, UpdateChildrenFilterRectsTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id);
    std::shared_ptr<RSRenderNode> filterNode = std::make_shared<RSRenderNode>(id + 1);
    RectI rect { 0, 0, 20, 20 };
    bool cacheValid = true;
    testNode->ResetChildrenFilterRects();
    testNode->UpdateChildrenFilterRects(filterNode, rect, cacheValid);
    testNode->UpdateChildrenFilterRects(filterNode, RectI(), cacheValid);
    const std::vector<RectI>& filterRects = testNode->GetChildrenNeedFilterRects();
    ASSERT_EQ(filterRects.size(), 1);
}

/**
 * @tc.name: OriAncoForceDoDirectTest
 * @tc.desc: test SetAncoForceDoDirect and GetOriAncoForceDoDirect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, OriAncoForceDoDirectTest, TestSize.Level1)
{
    RSSurfaceRenderNode::SetAncoForceDoDirect(false);
    EXPECT_FALSE(RSSurfaceRenderNode::GetOriAncoForceDoDirect());
}

/**
 * @tc.name: CheckUpdateHwcNodeLayerInfo
 * @tc.desc: test results of CheckUpdateHwcNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, CheckUpdateHwcNodeLayerInfo, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    RectI screeninfo;
    RectI absRect;
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    bool isFocusWindow = true;
    Vector4<int> cornerRadius;
    ASSERT_FALSE(node->CheckOpaqueRegionBaseInfo(screeninfo, absRect, screenRotation, isFocusWindow, cornerRadius));
    bool hasContainer = true;
    node->containerConfig_.Update(hasContainer, rrect);

    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    node->addedToPendingSyncList_ = true;
    node->isHardwareForcedDisabled_ = false;
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_BUTT;

    RSLayerInfo layerInfo;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(node->stagingRenderParams_.get());
    surfaceParams->SetLayerInfo(layerInfo);
    node->UpdateHwcNodeLayerInfo(transform);
    auto layer = node->stagingRenderParams_->GetLayerInfo();
    ASSERT_TRUE(layer.layerType == GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
}

/**
 * @tc.name: BufferClearCallbackProxy
 * @tc.desc: test results of BufferClearCallbackProxy
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, BufferClearCallbackProxy, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    ASSERT_NE(remoteObject, nullptr);
    sptr<RSBufferClearCallbackProxy> callback = new RSBufferClearCallbackProxy(remoteObject);
    ASSERT_NE(callback, nullptr);
    testNode->RegisterBufferClearListener(callback);
    testNode->SetNotifyRTBufferAvailable(true);
    ASSERT_TRUE(testNode->isNotifyRTBufferAvailable_);
}

/**
 * @tc.name: NeedCacheSurface
 * @tc.desc: test SetNeedCacheSurface and GetNeedCacheSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetNeedCacheSurface, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    testNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    ASSERT_NE(testNode->stagingRenderParams_, nullptr);

    testNode->SetNeedCacheSurface(true);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_TRUE(surfaceParams->GetNeedCacheSurface());

    testNode->SetNeedCacheSurface(false);
    surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_FALSE(surfaceParams->GetNeedCacheSurface());
}

/**
 * @tc.name: BootAnimationTest
 * @tc.desc: test SetBootAnimation and GetBootAnimation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, BootAnimationTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetBootAnimation(true);
    ASSERT_EQ(node->GetBootAnimation(), true);
    node->SetBootAnimation(false);
    ASSERT_FALSE(node->GetBootAnimation());
}

/**
 * @tc.name: GlobalPositionEnabledTest
 * @tc.desc: test SetGlobalPositionEnabled and GetGlobalPositionEnabled
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, GlobalPositionEnabledTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->SetGlobalPositionEnabled(true);
    ASSERT_EQ(node->GetGlobalPositionEnabled(), true);
    node->SetGlobalPositionEnabled(false);
    ASSERT_FALSE(node->GetGlobalPositionEnabled());
}

/**
 * @tc.name: AncestorDisplayNodeTest
 * @tc.desc: SetAncestorDisplayNode and GetAncestorDisplayNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, AncestorDisplayNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto displayNode = std::make_shared<RSBaseRenderNode>(0, context);
    node->SetAncestorDisplayNode(displayNode);
    ASSERT_EQ(node->GetAncestorDisplayNode().lock(), displayNode);
}

/**
 * @tc.name: UpdateSurfaceCacheContentStaticTest
 * @tc.desc: Set dirty subNode and check if surfacenode static
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, UpdateSurfaceCacheContentStaticTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto subNode = std::make_shared<RSRenderNode>(id + 1, context);
    if (node == nullptr || subNode == nullptr) {
        return;
    }
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> activeNodeIds;
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    std::shared_ptr<RSRenderNode> nullNode = nullptr;
    activeNodeIds[subNode->GetId()] = nullNode;
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    activeNodeIds[subNode->GetId()] = subNode;
    node->AddChild(subNode, 0);
    subNode->isContentDirty_ = true;
    node->UpdateSurfaceCacheContentStatic(activeNodeIds);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), false);
    ASSERT_EQ(node->IsContentDirtyNodeLimited(), true);
}

/**
 * @tc.name: SkipLayerTest
 * @tc.desc: test SetSkipLayer and GetSkipLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SkipLayerTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetSkipLayer(true);
    ASSERT_TRUE(node->GetSkipLayer());
}

/**
 * @tc.name: SnapshotSkipLayerTest
 * @tc.desc: test SetSnapshotSkipLayer and GetSnapshotSkipLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SnapshotSkipLayerTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetSnapshotSkipLayer(true);
    ASSERT_TRUE(node->GetSnapshotSkipLayer());
}

/**
 * @tc.name: SecurityLayerTest
 * @tc.desc: test SetSecurityLayer and GetSecurityLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SecurityLayerTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetSecurityLayer(true);
    ASSERT_TRUE(node->GetSecurityLayer());
}

/**
 * @tc.name: StoreMustRenewedInfoTest001
 * @tc.desc: Test StoreMustRenewedInfo while has filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, StoreMustRenewedInfoTest001, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    node->SetChildHasVisibleFilter(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest002
 * @tc.desc: Test StoreMustRenewedInfo while has effect node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, StoreMustRenewedInfoTest002, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->InitRenderParams();
    node->SetChildHasVisibleEffect(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest003
 * @tc.desc: Test StoreMustRenewedInfo while has hardware node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, StoreMustRenewedInfoTest003, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetHasHardwareNode(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest004
 * @tc.desc: Test StoreMustRenewedInfo while is skip layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, StoreMustRenewedInfoTest004, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetSkipLayer(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest005
 * @tc.desc: Test StoreMustRenewedInfo while is security layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, StoreMustRenewedInfoTest005, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetSecurityLayer(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: StoreMustRenewedInfoTest006
 * @tc.desc: Test StoreMustRenewedInfo while is protected layer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, StoreMustRenewedInfoTest006, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->SetProtectedLayer(true);
    node->RSRenderNode::StoreMustRenewedInfo();
    node->StoreMustRenewedInfo();
    ASSERT_TRUE(node->HasMustRenewedInfo());
}

/**
 * @tc.name: GetFirstLevelNodeIdTest001
 * @tc.desc: Test GetFirstLevelNode for single app window node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, GetFirstLevelNodeIdTest001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    NodeId nodeId = node->GetId();
    pid_t pid = ExtractPid(nodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[pid][nodeId] = node;
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->SetIsOnTheTree(true);
    ASSERT_EQ(node->GetFirstLevelNodeId(), node->GetId());
}

/**
 * @tc.name: GetFirstLevelNodeIdTest002
 * @tc.desc: Test GetFirstLevelNode for app window node which parent is leash window node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, GetFirstLevelNodeIdTest002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);

    NodeId childNodeId = childNode->GetId();
    NodeId parentNodeId = parentNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    pid_t parentNodePid = ExtractPid(parentNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[childNodePid][childNodeId] = childNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    childNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    parentNode->AddChild(childNode);
    parentNode->SetIsOnTheTree(true);
    ASSERT_EQ(childNode->GetFirstLevelNodeId(), parentNode->GetId());
}

/**
 * @tc.name: HasSharedTransitionNodeTest
 * @tc.desc: Test SetHasSharedTransitionNode and GetHasSharedTransitionNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetHasSharedTransitionNode, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    node->SetHasSharedTransitionNode(true);
    
    ASSERT_EQ(node->GetHasSharedTransitionNode(), true);
}


/**
 * @tc.name: SetForceHardwareAndFixRotationTest001
 * @tc.desc: Test SetForceHardwareAndFixRotation true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetForceHardwareAndFixRotationTest001, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->InitRenderParams();
    node->SetForceHardwareAndFixRotation(true);
    ASSERT_EQ(node->isFixRotationByUser_, true);
}

/**
 * @tc.name: SetForceHardwareAndFixRotationTest002
 * @tc.desc: Test SetForceHardwareAndFixRotation false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetForceHardwareAndFixRotationTest002, TestSize.Level2)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);

    node->InitRenderParams();
    node->SetForceHardwareAndFixRotation(false);
    ASSERT_EQ(node->isFixRotationByUser_, false);
}

/**
 * @tc.name: UpdateHwcDisabledBySrcRectTest
 * @tc.desc: test results of UpdateHwcDisabledBySrcRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, UpdateHwcDisabledBySrcRectTest, TestSize.Level1)
{
    bool hasRotation = false;
    auto buffer = SurfaceBuffer::Create();
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    renderNode->UpdateHwcDisabledBySrcRect(hasRotation);

    renderNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    renderNode->UpdateHwcDisabledBySrcRect(hasRotation);
    ASSERT_FALSE(renderNode->isHardwareForcedDisabledBySrcRect_);
}

/**
 * @tc.name: IsYUVBufferFormatTest
 * @tc.desc: test results of IsYUVBufferFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, IsYUVBufferFormatTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    EXPECT_FALSE(testNode->IsYUVBufferFormat());

    auto buffer = SurfaceBuffer::Create();
    testNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    EXPECT_NE(testNode->GetRSSurfaceHandler()->GetBuffer(), nullptr);
    EXPECT_FALSE(testNode->IsYUVBufferFormat());
}

/**
 * @tc.name: DirtyRegionDumpTest
 * @tc.desc: test results of DirtyRegionDump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, DirtyRegionDumpTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->DirtyRegionDump();

    node->dirtyManager_.reset();
    std::string dump = node->DirtyRegionDump();
    ASSERT_NE(dump, "");
}

/**
 * @tc.name: PrepareRenderBeforeChildrenTest
 * @tc.desc: test results of PrepareRenderBeforeChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, PrepareRenderBeforeChildrenTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas rsPaintFilterCanvas(&canvas);
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->InitRenderParams();
    node->PrepareRenderBeforeChildren(rsPaintFilterCanvas);
    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);
}

/**
 * @tc.name: CollectSurfaceTest
 * @tc.desc: test results of CollectSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, CollectSurfaceTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);

    testNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    Drawing::Canvas canvasArgs;
    RSPaintFilterCanvas canvas(&canvasArgs);
    std::vector<std::shared_ptr<RSRenderNode>> vec;
    testNode->CollectSurface(node, vec, true, false);

    testNode->nodeType_ = RSSurfaceNodeType::STARTING_WINDOW_NODE;
    testNode->CollectSurface(node, vec, true, false);
    testNode->CollectSurface(node, vec, false, false);
    testNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    testNode->CollectSurface(node, vec, true, false);
    testNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    testNode->CollectSurface(node, vec, true, true);
}

/**
 * @tc.name: SetIsNotifyUIBufferAvailableTest
 * @tc.desc: test results of SetIsNotifyUIBufferAvailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetIsNotifyUIBufferAvailableTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetIsNotifyUIBufferAvailable(true);
    ASSERT_TRUE(node->isNotifyUIBufferAvailable_.load());
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest
 * @tc.desc: test results of IsSubTreeNeedPrepare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, IsSubTreeNeedPrepareTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->IsSubTreeNeedPrepare(false, false);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    EXPECT_TRUE(node->IsSubTreeNeedPrepare(true, true));
}

/**
 * @tc.name: ProcessRenderAfterChildrenTest
 * @tc.desc: test results of ProcessRenderAfterChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, ProcessRenderAfterChildrenTest, TestSize.Level1)
{
    Drawing::Canvas canvasArgs;
    auto canvas = std::make_shared<RSPaintFilterCanvas>(&canvasArgs);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->ProcessRenderAfterChildren(*canvas);
    EXPECT_FALSE(node->needDrawAnimateProperty_);
}

/**
 * @tc.name: SetContextAlphaTest
 * @tc.desc: test results of SetContextAlpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetContextAlphaTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetContextAlpha(1.0f, true);
    testNode->SetContextAlpha(0.5f, true);
    testNode->SetContextAlpha(0.5f, false);
    EXPECT_EQ(testNode->contextAlpha_, 0.5f);
}

/**
 * @tc.name: SetContextClipRegionTest
 * @tc.desc: test results of GetContextClipRegion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetContextClipRegionTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    Drawing::Rect rect(0, 0, 100, 100);
    testNode->SetContextClipRegion(rect, true);
    testNode->SetContextClipRegion(rect, false);
    EXPECT_EQ(testNode->contextClipRect_->left_, rect.left_);
    testNode->SetContextClipRegion(Drawing::Rect(1, 1, 1, 1), true);
    EXPECT_NE(testNode->contextClipRect_->left_, rect.left_);
}

/**
 * @tc.name: SetSkipLayerTest
 * @tc.desc: test results of SetSkipLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetSkipLayerTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetSkipLayer(true);
    EXPECT_TRUE(node->isSkipLayer_);
    node->SetSkipLayer(false);
    EXPECT_FALSE(node->isSkipLayer_);
}

/**
 * @tc.name: SetSnapshotSkipLayerTest
 * @tc.desc: test results of SetSnapshotSkipLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SetSnapshotSkipLayerTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetSnapshotSkipLayer(true);
    EXPECT_TRUE(node->isSnapshotSkipLayer_);
    node->SetSnapshotSkipLayer(false);
    EXPECT_FALSE(node->isSnapshotSkipLayer_);
}

/**
 * @tc.name: SyncSecurityInfoToFirstLevelNodeTest
 * @tc.desc: test results of SyncSecurityInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SyncSecurityInfoToFirstLevelNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SyncSecurityInfoToFirstLevelNode();
    EXPECT_FALSE(node->isSkipLayer_);
}

/**
 * @tc.name: SyncSkipInfoToFirstLevelNode
 * @tc.desc: test results of SyncSkipInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SyncSkipInfoToFirstLevelNode, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SyncSkipInfoToFirstLevelNode();
    EXPECT_FALSE(node->isSkipLayer_);
}

/**
 * @tc.name: SyncSnapshotSkipInfoToFirstLevelNode
 * @tc.desc: test results of SyncSnapshotSkipInfoToFirstLevelNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, SyncSnapshotSkipInfoToFirstLevelNode, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SyncSnapshotSkipInfoToFirstLevelNode();
    EXPECT_FALSE(node->isSnapshotSkipLayer_);
}

/**
 * @tc.name: NotifyRTBufferAvailable
 * @tc.desc: test results of NotifyRTBufferAvailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, NotifyRTBufferAvailable, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->NotifyRTBufferAvailable(false);
    ASSERT_FALSE(testNode->isNotifyRTBufferAvailablePre_);
    testNode->NotifyRTBufferAvailable(true);
    testNode->isRefresh_ = true;
    testNode->NotifyRTBufferAvailable(true);
    ASSERT_FALSE(testNode->isNotifyRTBufferAvailable_);
}

/**
 * @tc.name: NotifyRTBufferAvailable
 * @tc.desc: test results of NotifyRTBufferAvailable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, NotifyUIBufferAvailable, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->NotifyUIBufferAvailable();
    testNode->isNotifyUIBufferAvailable_ = false;
    testNode->NotifyUIBufferAvailable();
    ASSERT_TRUE(testNode->isNotifyUIBufferAvailable_);
}

/**
 * @tc.name: UpdateDirtyIfFrameBufferConsumed
 * @tc.desc: test results of UpdateDirtyIfFrameBufferConsumed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeUnitTest, UpdateDirtyIfFrameBufferConsumed, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->GetRSSurfaceHandler()->isCurrentFrameBufferConsumed_ = true;
    bool resultOne = testNode->UpdateDirtyIfFrameBufferConsumed();
    EXPECT_TRUE(resultOne);

    testNode->GetRSSurfaceHandler()->isCurrentFrameBufferConsumed_ = false;
    bool resultTwo = testNode->UpdateDirtyIfFrameBufferConsumed();
    EXPECT_FALSE(resultTwo);
}
} // namespace Rosen
} // namespace OHOS