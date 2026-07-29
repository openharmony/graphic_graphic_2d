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

#include "command/rs_ui_director_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_ui_render_director.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIDirectorCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDirectorCommandTest::SetUpTestCase() {}
void RSUIDirectorCommandTest::TearDownTestCase() {}
void RSUIDirectorCommandTest::SetUp() {}
void RSUIDirectorCommandTest::TearDown() {}

/**
 * @tc.name: UIDirectorCommandHelperLifecycleTest
 * @tc.desc: Test RSUIDirectorCommandHelper lifecycle commands create and transition director state.
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSUIDirectorCommandTest, UIDirectorCommandHelperLifecycleTest, TestSize.Level1)
{
    RSContext rsContext;
    constexpr pid_t pid = 200;
    constexpr uint64_t token = 201;
    constexpr NodeId nodeId = MakeNodeId(pid, 0);

    RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, token);
    auto director = rsContext.GetUIRenderDirector(pid, token);
    ASSERT_NE(director, nullptr);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    // Duplicate create should be ignored and not crash.
    RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    RSUIDirectorCommandHelper::GoResume(rsContext, nodeId, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);

    RSUIDirectorCommandHelper::GoForeground(rsContext, nodeId, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);

    RSUIDirectorCommandHelper::GoBackground(rsContext, nodeId, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);

    RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);

    RSUIDirectorCommandHelper::GoDestroy(rsContext, nodeId, token);
    EXPECT_EQ(rsContext.GetUIRenderDirector(pid, token), nullptr);
}

/**
 * @tc.name: UIDirectorCommandHelperMissingDirectorTest
 * @tc.desc: Test RSUIDirectorCommandHelper handles missing director gracefully.
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSUIDirectorCommandTest, UIDirectorCommandHelperMissingDirectorTest, TestSize.Level1)
{
    RSContext rsContext;
    constexpr pid_t pid = 300;
    constexpr uint64_t token = 301;
    constexpr NodeId nodeId = MakeNodeId(pid, 0);

    RSUIDirectorCommandHelper::GoResume(rsContext, nodeId, token);
    RSUIDirectorCommandHelper::GoForeground(rsContext, nodeId, token);
    RSUIDirectorCommandHelper::GoBackground(rsContext, nodeId, token);
    RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, token);
    RSUIDirectorCommandHelper::GoDestroy(rsContext, nodeId, token);

    EXPECT_EQ(rsContext.GetUIRenderDirector(pid, token), nullptr);
}

/**
 * @tc.name: UIDirectorCommandHelperGoStopDestroyTokenNodeGated
 * @tc.desc: Test GoStop only calls DestroyTokenNode when GetBackgroundRebuildEnabled is true.
 * @tc.type: FUNC
 * @tc.require: issueI590LM
 */
HWTEST_F(RSUIDirectorCommandTest, UIDirectorCommandHelperGoStopDestroyTokenNodeGated, TestSize.Level1)
{
    constexpr pid_t pid = 400;
    constexpr uint64_t token = 401;
    constexpr NodeId nodeId = MakeNodeId(pid, 0);

    // Case 1: GetBackgroundRebuildEnabled == false -> DestroyTokenNode should NOT be called
    {
        RSContext rsContext;
        RSSystemProperties::isBackgroundRebuildEnabled_ = false;
        RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, token);
        // Register a node with this token so DestroyTokenNode would remove it if called
        auto renderNode = std::make_shared<RSRenderNode>(nodeId);
        renderNode->SetUIContextToken(token);
        rsContext.GetMutableNodeMap().RegisterRenderNode(renderNode);
        ASSERT_NE(rsContext.GetMutableNodeMap().GetRenderNode(nodeId), nullptr);

        RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, token);

        // Node should still exist because DestroyTokenNode was not called
        EXPECT_NE(rsContext.GetMutableNodeMap().GetRenderNode(nodeId), nullptr);
        auto director = rsContext.GetUIRenderDirector(pid, token);
        ASSERT_NE(director, nullptr);
        EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
    }

    // Case 2: GetBackgroundRebuildEnabled == true -> DestroyTokenNode should be called
    {
        RSContext rsContext;
        RSSystemProperties::isBackgroundRebuildEnabled_ = true;
        RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, token);
        auto renderNode = std::make_shared<RSRenderNode>(nodeId);
        renderNode->SetUIContextToken(token);
        rsContext.GetMutableNodeMap().RegisterRenderNode(renderNode);
        ASSERT_NE(rsContext.GetMutableNodeMap().GetRenderNode(nodeId), nullptr);
 
        RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, token);
 
        // Node should be removed because DestroyTokenNode was called
        EXPECT_EQ(rsContext.GetMutableNodeMap().GetRenderNode(nodeId), nullptr);
        auto director = rsContext.GetUIRenderDirector(pid, token);
        ASSERT_NE(director, nullptr);
        EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
    }

    // Restore default
    RSSystemProperties::isBackgroundRebuildEnabled_ = false;
}

} // namespace OHOS::Rosen
