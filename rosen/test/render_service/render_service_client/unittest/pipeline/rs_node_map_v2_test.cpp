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

#include "gtest/gtest.h"

#include "pipeline/rs_node_map_v2.h"
#include "ui/rs_base_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeMapV2Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeMapV2Test::SetUpTestCase() {}
void RSNodeMapV2Test::TearDownTestCase() {}
void RSNodeMapV2Test::SetUp() {}
void RSNodeMapV2Test::TearDown() {}

/**
 * @tc.name: TraversalNodesTest001
 * @tc.desc: TraversalNodes skips expired weak_ptr entries (branch: node.lock() returns null)
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapV2Test, TraversalNodesTest001, TestSize.Level1)
{
    NodeId nodeId = 7001;
    RSNodeMapV2 nodeMap;
    {
        auto node = std::make_shared<RSNode>(0);
        node->id_ = nodeId;
        ASSERT_TRUE(nodeMap.RegisterNode(node));
    }
    bool called = false;
    nodeMap.TraversalNodes([&called](const std::shared_ptr<RSBaseNode>&) { called = true; });
    EXPECT_FALSE(called);
}

/**
 * @tc.name: TraversalNodesTest002
 * @tc.desc: TraversalNodes invokes func with alive nodes (branch: node.lock() returns non-null)
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSNodeMapV2Test, TraversalNodesTest002, TestSize.Level1)
{
    NodeId nodeId = 7002;
    RSNodeMapV2 nodeMap;
    auto node = std::make_shared<RSNode>(0);
    node->id_ = nodeId;
    ASSERT_TRUE(nodeMap.RegisterNode(node));
    bool called = false;
    std::shared_ptr<RSBaseNode> visited;
    nodeMap.TraversalNodes([&](const std::shared_ptr<RSBaseNode>& n) {
        called = true;
        visited = n;
    });
    EXPECT_TRUE(called);
    EXPECT_EQ(visited, node);
}
} // namespace OHOS::Rosen
