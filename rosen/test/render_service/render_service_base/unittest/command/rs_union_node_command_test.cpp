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
#include "command/rs_union_node_command.h"
#include "command/rs_ui_director_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_union_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUnionNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUnionNodeCommandTest::SetUpTestCase() {}
void RSUnionNodeCommandTest::TearDownTestCase() {}
void RSUnionNodeCommandTest::SetUp() {}
void RSUnionNodeCommandTest::TearDown() {}

/**
 * @tc.name: CreateTest
 * @tc.desc: Verify Create works normally
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeCommandTest, CreateTest, TestSize.Level1)
{
    RSContext context;
    NodeId id = MakeNodeId(1, 1);
    UnionNodeCommandHelper::Create(context, id, false);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSUnionRenderNode>(id), nullptr);
}

/**
 * @tc.name: CreateDOSProtectionTest
 * @tc.desc: Verify Create is blocked when node count exceeds MAX_NODE_COUNT_PER_PID
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeCommandTest, CreateDOSProtectionTest, TestSize.Level1)
{
    RSContext context;
    pid_t pid = 1;
    for (uint32_t i = 0; i <= MAX_NODE_COUNT_PER_PID; i++) {
        NodeId existId = MakeNodeId(pid, i);
        context.nodeMap.renderNodeMap_[pid][existId] =
            std::make_shared<RSUnionRenderNode>(existId, context.weak_from_this(), false);
    }
    NodeId newNodeId = MakeNodeId(pid, MAX_NODE_COUNT_PER_PID + 1);
    UnionNodeCommandHelper::Create(context, newNodeId, false);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSUnionRenderNode>(newNodeId), nullptr);
}


/**
 * @tc.name: CreateSceneBoardBranchTest001
 * @tc.desc: Create with a marked token covers the SceneBoard branch.
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeCommandTest, CreateSceneBoardBranchTest001, TestSize.Level1)
{
    constexpr pid_t pid = 814;
    constexpr uint64_t token = 815;
    constexpr NodeId directorNodeId = MakeNodeId(pid, 0);
    constexpr NodeId nodeId = MakeNodeId(pid, 1);
    RSContext context;
    RSUIDirectorCommandHelper::GoCreate(context, directorNodeId, token);
    RSUIDirectorCommandHelper::MarkSceneBoard(context, directorNodeId, token);
    UnionNodeCommandHelper::Create(context, nodeId, false, token);
    auto node = context.GetNodeMap().GetRenderNode<RSUnionRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->IsSceneBoard());
    EXPECT_EQ(node->GetUIContextToken(), token);
}

} // namespace OHOS::Rosen
