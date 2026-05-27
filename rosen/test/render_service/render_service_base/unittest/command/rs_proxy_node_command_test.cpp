/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "include/command/rs_proxy_node_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProxyNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProxyNodeCommandTest::SetUpTestCase() {}
void RSProxyNodeCommandTest::TearDownTestCase() {}
void RSProxyNodeCommandTest::SetUp() {}
void RSProxyNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSProxyNodeCommand001
 * @tc.desc: ResetContextVariableCache test.
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, TestRSProxyNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    ProxyNodeCommandHelper::ResetContextVariableCache(context, id);

    NodeId targetId = static_cast<NodeId>(2);
    ProxyNodeCommandHelper::Create(context, id, targetId);
    ProxyNodeCommandHelper::ResetContextVariableCache(context, id);
    EXPECT_TRUE(id != -2);
}

/**
 * @tc.name: CreateDOSProtectionTest
 * @tc.desc: Verify Create is blocked when node count exceeds MAX_NODE_COUNT_PER_PID
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, CreateDOSProtectionTest, TestSize.Level1)
{
    RSContext context;
    pid_t pid = 1;
    for (uint32_t i = 0; i <= MAX_NODE_COUNT_PER_PID; i++) {
        NodeId existId = MakeNodeId(pid, i);
        context.nodeMap.renderNodeMap_[pid][existId] = std::make_shared<RSRenderNode>(existId);
    }
    NodeId newNodeId = MakeNodeId(pid, MAX_NODE_COUNT_PER_PID + 1);
    NodeId targetId = static_cast<NodeId>(9999);
    ProxyNodeCommandHelper::Create(context, newNodeId, targetId);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSProxyRenderNode>(newNodeId), nullptr);
}
} // namespace OHOS::Rosen
