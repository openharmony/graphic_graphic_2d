/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node.h"
#include "pipeline/rs_ui_render_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSUIRenderDirectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIRenderDirectorTest::SetUpTestCase() {}
void RSUIRenderDirectorTest::TearDownTestCase() {}
void RSUIRenderDirectorTest::SetUp() {}
void RSUIRenderDirectorTest::TearDown() {}

/**
 * @tc.name: OnStateSyncSameState
 * @tc.desc: Cover branch: OnStateSync returns early when the state is unchanged.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIRenderDirectorTest, OnStateSyncSameState, TestSize.Level1)
{
    constexpr uint64_t token = 1;
    RSUIRenderDirector director(token);
    EXPECT_EQ(director.GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    director.OnStateSync(RSUIDirectorLifecycleState::CREATE);
    EXPECT_EQ(director.GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
}

/**
 * @tc.name: OnStateSyncDifferentState
 * @tc.desc: Cover branch: OnStateSync updates the state when it changes.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIRenderDirectorTest, OnStateSyncDifferentState, TestSize.Level1)
{
    constexpr uint64_t token = 1;
    RSUIRenderDirector director(token);

    director.OnStateSync(RSUIDirectorLifecycleState::FOREGROUND);
    EXPECT_EQ(director.GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);
}

/**
 * @tc.name: GetTokenTest
 * @tc.desc: Test GetToken returns the token passed to the constructor.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIRenderDirectorTest, GetTokenTest, TestSize.Level1)
{
    constexpr uint64_t token = 42;
    RSUIRenderDirector director(token);
    EXPECT_EQ(director.GetToken(), token);
}

/**
 * @tc.name: RenderNodeRegisterAndGet
 * @tc.desc: Cover branches: GetRenderNode returns the registered node or nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIRenderDirectorTest, RenderNodeRegisterAndGet, TestSize.Level1)
{
    constexpr uint64_t token = 1;
    RSUIRenderDirector director(token);
    constexpr NodeId nodeId = 1;

    // Branch: node not found.
    EXPECT_EQ(director.GetRenderNode(nodeId), nullptr);

    auto node = std::make_shared<RSRenderNode>(nodeId);
    director.RegisterRenderNode(nodeId, node);

    // Branch: node found.
    EXPECT_EQ(director.GetRenderNode(nodeId), node);
}

/**
 * @tc.name: RenderNodeUnregisterAndClear
 * @tc.desc: Test UnregisterRenderNode and ClearRenderNodes remove registered nodes.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIRenderDirectorTest, RenderNodeUnregisterAndClear, TestSize.Level1)
{
    constexpr uint64_t token = 1;
    RSUIRenderDirector director(token);
    constexpr NodeId nodeId = 1;

    auto node = std::make_shared<RSRenderNode>(nodeId);
    director.RegisterRenderNode(nodeId, node);
    EXPECT_EQ(director.GetRenderNode(nodeId), node);

    director.UnregisterRenderNode(nodeId);
    EXPECT_EQ(director.GetRenderNode(nodeId), nullptr);

    director.RegisterRenderNode(nodeId, node);
    director.ClearRenderNodes();
    EXPECT_EQ(director.GetRenderNode(nodeId), nullptr);
}

} // namespace OHOS::Rosen
