/*
 * Copyright (c) 2022-2026 Huawei Device Co., Ltd.
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
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr pid_t CALLING_PID = 100;
constexpr pid_t TARGET_PID = 200;
constexpr uint64_t CALLING_UNIQUE_ID = 1;
constexpr uint64_t TARGET_UNIQUE_ID = 2;

class RSProxyNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RSContext context;
};

void RSProxyNodeCommandTest::SetUpTestCase() {}
void RSProxyNodeCommandTest::TearDownTestCase() {}
void RSProxyNodeCommandTest::SetUp() {}
void RSProxyNodeCommandTest::TearDown() {}

/**
 * @tc.name: ProxyNodeCommandHelper_Create_SamePid
 * @tc.desc: Verify Create with same pid, proxy node should be registered
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, ProxyNodeCommandHelper_Create_SamePid, TestSize.Level1)
{
    NodeId id = static_cast<NodeId>(1);
    NodeId targetId = static_cast<NodeId>(2);
    ProxyNodeCommandHelper::Create(context, id, targetId);
    auto node = context.GetNodeMap().GetRenderNode<RSProxyRenderNode>(id);
    EXPECT_NE(node, nullptr);
    ProxyNodeCommandHelper::ResetContextVariableCache(context, id);
}

/**
 * @tc.name: ProxyNodeCommandHelper_Create_CrossPidDenied
 * @tc.desc: Verify Create denied when cross-pid without UIExtension or resident
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, ProxyNodeCommandHelper_Create_CrossPidDenied, TestSize.Level1)
{
    NodeId id = (static_cast<uint64_t>(CALLING_PID) << 32) | CALLING_UNIQUE_ID;
    NodeId targetId = (static_cast<uint64_t>(TARGET_PID) << 32) | TARGET_UNIQUE_ID;
    ProxyNodeCommandHelper::Create(context, id, targetId);
    auto node = context.GetNodeMap().GetRenderNode<RSProxyRenderNode>(id);
    EXPECT_EQ(node, nullptr);
}

/**
 * @tc.name: ProxyNodeCommandHelper_Create_CrossPidUIExtensionAllowed
 * @tc.desc: Verify Create allowed when cross-pid with UIExtension target
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, ProxyNodeCommandHelper_Create_CrossPidUIExtensionAllowed, TestSize.Level1)
{
    NodeId id = (static_cast<uint64_t>(CALLING_PID) << 32) | CALLING_UNIQUE_ID;
    NodeId targetId = (static_cast<uint64_t>(TARGET_PID) << 32) | TARGET_UNIQUE_ID;
    context.GetMutableNodeMap().uiExtensionSurfaceNodes_.insert(targetId);
    ProxyNodeCommandHelper::Create(context, id, targetId);
    auto node = context.GetNodeMap().GetRenderNode<RSProxyRenderNode>(id);
    EXPECT_NE(node, nullptr);
}

/**
 * @tc.name: ProxyNodeCommandHelper_Create_CrossPidResidentAllowed
 * @tc.desc: Verify Create allowed when cross-pid with resident process caller
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, ProxyNodeCommandHelper_Create_CrossPidResidentAllowed, TestSize.Level1)
{
    NodeId id = (static_cast<uint64_t>(CALLING_PID) << 32) | CALLING_UNIQUE_ID;
    NodeId targetId = (static_cast<uint64_t>(TARGET_PID) << 32) | TARGET_UNIQUE_ID;
    auto residentNode = std::make_shared<RSSurfaceRenderNode>(id, context.weak_from_this());
    context.GetMutableNodeMap().residentSurfaceNodeMap_.emplace(id, residentNode);
    ProxyNodeCommandHelper::Create(context, id, targetId);
    auto node = context.GetNodeMap().GetRenderNode<RSProxyRenderNode>(id);
    EXPECT_NE(node, nullptr);
}

/**
 * @tc.name: ProxyNodeCommandHelper_ResetContextVariableCache
 * @tc.desc: Verify ResetContextVariableCache with non-existent node
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, ProxyNodeCommandHelper_ResetContextVariableCache, TestSize.Level1)
{
    NodeId id = static_cast<NodeId>(999);
    ProxyNodeCommandHelper::ResetContextVariableCache(context, id);
    EXPECT_TRUE(id == 999);
}

} // namespace OHOS::Rosen
