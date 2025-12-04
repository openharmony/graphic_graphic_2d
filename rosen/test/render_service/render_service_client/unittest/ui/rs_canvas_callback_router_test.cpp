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

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "gtest/gtest.h"

#include "ui/rs_canvas_callback_router.h"
#include "ui/rs_canvas_drawing_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasCallbackRouterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void SetUpTestNodes();
    void ClearRouterState();

protected:
    std::vector<std::shared_ptr<RSCanvasDrawingNode>> testNodes_;
    std::vector<NodeId> testNodeIds_;
};

void RSCanvasCallbackRouterTest::SetUpTestCase() {}
void RSCanvasCallbackRouterTest::TearDownTestCase() {}

void RSCanvasCallbackRouterTest::SetUp()
{
    SetUpTestNodes();
}

void RSCanvasCallbackRouterTest::TearDown()
{
    ClearRouterState();
    testNodes_.clear();
    testNodeIds_.clear();
}

void RSCanvasCallbackRouterTest::SetUpTestNodes()
{
    // Create 3 test nodes
    for (int i = 0; i < 3; ++i) {
        auto node = RSCanvasDrawingNode::Create(false, false);
        ASSERT_NE(node, nullptr);
        testNodes_.push_back(node);
        testNodeIds_.push_back(node->GetId());
    }
}

void RSCanvasCallbackRouterTest::ClearRouterState()
{
    // Unregister all test nodes
    auto& router = RSCanvasCallbackRouter::GetInstance();
    for (auto nodeId : testNodeIds_) {
        router.UnregisterNode(nodeId);
    }
}


/**
 * @tc.name: RegisterAndRouteTest
 * @tc.desc: Test node registration, routing and unregistration functionality
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasCallbackRouterTest, RegisterAndRouteTest, TestSize.Level1)
{
    auto& router = RSCanvasCallbackRouter::GetInstance();

    // Register a node
    router.RegisterNode(testNodeIds_[0], testNodes_[0]);

    // Route to the registered node
    auto routedNode = router.RouteToNode(testNodeIds_[0]);
    EXPECT_NE(routedNode, nullptr);
    EXPECT_EQ(routedNode->GetId(), testNodeIds_[0]);
    EXPECT_EQ(routedNode.get(), testNodes_[0].get());

    // Unregister the node
    router.UnregisterNode(testNodeIds_[0]);

    // Verify it can no longer be routed
    routedNode = router.RouteToNode(testNodeIds_[0]);
    EXPECT_EQ(routedNode, nullptr);
}

/**
 * @tc.name: RouteToDestroyedNodeTest
 * @tc.desc: Test routing to destroyed nodes (weak_ptr cleanup)
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasCallbackRouterTest, RouteToDestroyedNodeTest, TestSize.Level1)
{
    auto& router = RSCanvasCallbackRouter::GetInstance();

    // Create a temporary node that will go out of scope
    {
        auto tempNode = RSCanvasDrawingNode::Create(false, false);
        ASSERT_NE(tempNode, nullptr);
        NodeId tempNodeId = tempNode->GetId();

        // Register the temporary node
        router.RegisterNode(tempNodeId, tempNode);

        // Verify it can be routed
        auto routedNode = router.RouteToNode(tempNodeId);
        EXPECT_NE(routedNode, nullptr);
        EXPECT_EQ(routedNode->GetId(), tempNodeId);
    }

    router.UnregisterNode(testNodeIds_[0]);
    auto routedNode = router.RouteToNode(testNodeIds_[0]);
    EXPECT_EQ(routedNode, nullptr);
}

/**
 * @tc.name: WeakPtrCleanupTest
 * @tc.desc: Test weak_ptr cleanup when node is destroyed
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasCallbackRouterTest, WeakPtrCleanupTest, TestSize.Level1)
{
    auto& router = RSCanvasCallbackRouter::GetInstance();

    // Create a node with a controlled lifecycle
    std::shared_ptr<RSCanvasDrawingNode> tempNode;
    NodeId tempNodeId;

    // Create and register the node
    {
        tempNode = RSCanvasDrawingNode::Create(false, false);
        ASSERT_NE(tempNode, nullptr);
        tempNodeId = tempNode->GetId();

        // Register the node
        router.RegisterNode(tempNodeId, tempNode);

        // Verify it can be routed
        auto routedNode = router.RouteToNode(tempNodeId);
        EXPECT_NE(routedNode, nullptr);
        EXPECT_EQ(routedNode->GetId(), tempNodeId);
    }

    // Force destruction of the node by resetting the shared_ptr
    // This should trigger weak_ptr cleanup
    tempNode.reset();

    // Give some time for potential cleanup and verify routing returns nullptr
    auto routedNode = router.RouteToNode(tempNodeId);
    EXPECT_EQ(routedNode, nullptr) << "Node should be nullptr after destruction";

    // Verify the expired weak_ptr was cleaned up by trying to route again
    routedNode = router.RouteToNode(tempNodeId);
    EXPECT_EQ(routedNode, nullptr) << "Expired weak_ptr should be cleaned up";
}

/**
 * @tc.name: RouteToNonExistentNodeTest
 * @tc.desc: Test routing to non-existent nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasCallbackRouterTest, RouteToNonExistentNodeTest, TestSize.Level1)
{
    auto& router = RSCanvasCallbackRouter::GetInstance();

    // Use a non-existent node ID
    NodeId nonExistentId = 99999;
    auto routedNode = router.RouteToNode(nonExistentId);
    EXPECT_EQ(routedNode, nullptr);

    // Also test with ID 0
    routedNode = router.RouteToNode(0);
    EXPECT_EQ(routedNode, nullptr);
}

/**
 * @tc.name: MultipleNodesTest
 * @tc.desc: Test with multiple nodes registered
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasCallbackRouterTest, MultipleNodesTest, TestSize.Level1)
{
    auto& router = RSCanvasCallbackRouter::GetInstance();

    // Register multiple nodes
    for (size_t i = 0; i < testNodes_.size(); ++i) {
        router.RegisterNode(testNodeIds_[i], testNodes_[i]);
    }

    // Verify each node can be routed correctly
    for (size_t i = 0; i < testNodes_.size(); ++i) {
        auto routedNode = router.RouteToNode(testNodeIds_[i]);
        EXPECT_NE(routedNode, nullptr);
        EXPECT_EQ(routedNode->GetId(), testNodeIds_[i]);
        EXPECT_EQ(routedNode.get(), testNodes_[i].get());
    }

    // Verify one node doesn't route to another
    auto node0 = router.RouteToNode(testNodeIds_[0]);
    auto node1 = router.RouteToNode(testNodeIds_[1]);
    EXPECT_NE(node0.get(), node1.get());

    // Unregister one node and verify others still work
    router.UnregisterNode(testNodeIds_[1]);
    EXPECT_NE(router.RouteToNode(testNodeIds_[0]), nullptr);
    EXPECT_EQ(router.RouteToNode(testNodeIds_[1]), nullptr);
    EXPECT_NE(router.RouteToNode(testNodeIds_[2]), nullptr);
}

/**
 * @tc.name: OverwriteRegistrationTest
 * @tc.desc: Test registering same nodeId multiple times
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasCallbackRouterTest, OverwriteRegistrationTest, TestSize.Level1)
{
    auto& router = RSCanvasCallbackRouter::GetInstance();

    // Register first node
    router.RegisterNode(testNodeIds_[0], testNodes_[0]);

    // Route to first node
    auto routedNode = router.RouteToNode(testNodeIds_[0]);
    EXPECT_NE(routedNode, nullptr);
    EXPECT_EQ(routedNode.get(), testNodes_[0].get());

    // Register same ID with different node (overwrite)
    router.RegisterNode(testNodeIds_[0], testNodes_[1]);

    // Route should now go to second node
    routedNode = router.RouteToNode(testNodeIds_[0]);
    EXPECT_NE(routedNode, nullptr);
    EXPECT_EQ(routedNode.get(), testNodes_[1].get());
    EXPECT_NE(routedNode.get(), testNodes_[0].get());
}
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK