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
#include "gtest/gtest.h"
#include "rs_test_util.h"

#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUniRenderUtilSecUIExtensionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderUtilSecUIExtensionTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderUtilSecUIExtensionTest::TearDownTestCase() {}
void RSUniRenderUtilSecUIExtensionTest::SetUp() {}
void RSUniRenderUtilSecUIExtensionTest::TearDown() {}


/*
 * @tc.name: UIExtensionFindAndTraverseAncestor_001
 * @tc.desc: test when node map is empty or host node is not surface node, expect empty callback data
 * @tc.type: FUNC
 * @tc.require: issueI9SDDH
 */
HWTEST_F(RSUniRenderUtilSecUIExtensionTest, UIExtensionFindAndTraverseAncestor_001, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    UIExtensionCallbackData callbackData;
    NodeId uiExtensionNodeId = 0;
    NodeId hostNodeId = 1;
    RSSurfaceRenderNode::secUIExtensionNodes_.insert(std::pair<NodeId, NodeId>(uiExtensionNodeId, hostNodeId));
    // empty nodeMap
    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(nodeMap, callbackData);
    ASSERT_TRUE(callbackData.empty());
    // hostnode is not surface node
    auto canvasNode = std::make_shared<RSRenderNode>(hostNodeId);
    pid_t hostNodePid = ExtractPid(hostNodeId);
    nodeMap.renderNodeMap_[hostNodePid].insert(std::pair(hostNodeId, canvasNode));
    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(nodeMap, callbackData);
    ASSERT_TRUE(callbackData.empty());
}

/*
 * @tc.name: UIExtensionFindAndTraverseAncestor_002
 * @tc.desc: test when host node is surface node and one uiExtension child, callback data is not empty
 * @tc.type: FUNC
 * @tc.require: issueI9SDDH
 */
HWTEST_F(RSUniRenderUtilSecUIExtensionTest, UIExtensionFindAndTraverseAncestor_002, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    UIExtensionCallbackData callbackData;
    NodeId uiExtensionNodeId = 0;
    NodeId hostNodeId = 1;
    // hostnode is surface node
    auto hostNode = std::make_shared<RSSurfaceRenderNode>(hostNodeId);
    RSSurfaceRenderNodeConfig config = { .id = uiExtensionNodeId,
                                         .nodeType = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE };
    auto uiExtensionNode = std::make_shared<RSSurfaceRenderNode>(config);
    hostNode->AddChild(uiExtensionNode);
    hostNode->GenerateFullChildrenList();
    uiExtensionNode->SetIsOnTheTree(true, hostNodeId, INVALID_NODEID, INVALID_NODEID);
    pid_t hostNodePid = ExtractPid(hostNodeId);
    nodeMap.renderNodeMap_[hostNodePid].insert(std::pair(hostNodeId, hostNode));

    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(nodeMap, callbackData);
    ASSERT_FALSE(callbackData.empty());
}

/*
 * @tc.name: TraverseAndCollectUIExtensionInfo_001
 * @tc.desc: test when host node is nullptr, callback data is empty
 * @tc.type: FUNC
 * @tc.require: issueI9SDDH
 */
HWTEST_F(RSUniRenderUtilSecUIExtensionTest, TraverseAndCollectUIExtensionInfo_001, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    UIExtensionCallbackData callbackData;
    NodeId hostNodeId = 0;
    // hostnode is surface node
    auto hostNode = std::make_shared<RSSurfaceRenderNode>(hostNodeId);
    hostNode->AddChild(nullptr);
    hostNode->GenerateFullChildrenList();
    pid_t hostNodePid = ExtractPid(hostNodeId);
    nodeMap.renderNodeMap_[hostNodePid].insert(std::pair(hostNodeId, hostNode));

    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(nodeMap, callbackData);
    ASSERT_TRUE(callbackData.empty());
}

/*
 * @tc.name: TraverseAndCollectUIExtensionInfo_002
 * @tc.desc: test when the node does not need to be collected, callback data is empty
 * @tc.type: FUNC
 * @tc.require: issueI9SDDH
 */
HWTEST_F(RSUniRenderUtilSecUIExtensionTest, TraverseAndCollectUIExtensionInfo_002, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    UIExtensionCallbackData callbackData;
    NodeId hostNodeId = 0;
    NodeId childNodeId = 1;
    // hostnode is surface node
    auto hostNode = std::make_shared<RSSurfaceRenderNode>(hostNodeId);
    auto childNode = std::make_shared<RSRenderNode>(childNodeId);
    hostNode->AddChild(childNode);
    hostNode->GenerateFullChildrenList();
    pid_t hostNodePid = ExtractPid(hostNodeId);
    nodeMap.renderNodeMap_[hostNodePid].insert(std::pair(hostNodeId, hostNode));

    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(nodeMap, callbackData);
    ASSERT_TRUE(callbackData.empty());
}

/*
 * @tc.name: TraverseAndCollectUIExtensionInfo_003
 * @tc.desc: test when the node collected after UIExtension, callback data is not empty
 * @tc.type: FUNC
 * @tc.require: issueI9SDDH
 */
HWTEST_F(RSUniRenderUtilSecUIExtensionTest, TraverseAndCollectUIExtensionInfo_003, TestSize.Level2)
{
    RSRenderNodeMap nodeMap;
    UIExtensionCallbackData callbackData;
    NodeId uiExtensionNodeId = 0;
    NodeId hostNodeId = 1;
    // hostnode is surface node
    auto hostNode = std::make_shared<RSSurfaceRenderNode>(hostNodeId);
    RSSurfaceRenderNodeConfig config = { .id = uiExtensionNodeId,
                                         .nodeType = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE };
    auto uiExtensionNode = std::make_shared<RSSurfaceRenderNode>(config);
    hostNode->AddChild(uiExtensionNode);
    NodeId canvasNodeId = 2;
    auto upperNode = std::make_shared<RSRenderNode>(canvasNodeId);
    hostNode->AddChild(upperNode);

    hostNode->GenerateFullChildrenList();
    uiExtensionNode->SetIsOnTheTree(true, hostNodeId, INVALID_NODEID, INVALID_NODEID);
    pid_t hostNodePid = ExtractPid(hostNodeId);
    nodeMap.renderNodeMap_[hostNodePid].insert(std::pair(hostNodeId, hostNode));

    RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(nodeMap, callbackData);
    ASSERT_FALSE(callbackData.empty());
    ASSERT_FALSE(callbackData[hostNodeId].empty());
    ASSERT_FALSE(callbackData[hostNodeId][0].upperNodes.empty());
}
} // namespace OHOS::Rosen
