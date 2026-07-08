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
#include "pipeline/rs_ui_render_director.h"

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

    RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, pid, token);
    auto director = rsContext.GetUIRenderDirector(pid, token);
    ASSERT_NE(director, nullptr);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    // Duplicate create should be ignored and not crash.
    RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    RSUIDirectorCommandHelper::GoResume(rsContext, nodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);

    RSUIDirectorCommandHelper::GoForeground(rsContext, nodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);

    RSUIDirectorCommandHelper::GoBackground(rsContext, nodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);

    RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);

    RSUIDirectorCommandHelper::GoDestroy(rsContext, nodeId, pid, token);
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

    RSUIDirectorCommandHelper::GoResume(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoForeground(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoBackground(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoDestroy(rsContext, nodeId, pid, token);

    EXPECT_EQ(rsContext.GetUIRenderDirector(pid, token), nullptr);
}

/**
 * @tc.name: UIDirectorCommandHelperNodeIdPidMismatchTest
 * @tc.desc: Test RSUIDirectorCommandHelper rejects commands whose nodeId high 32 bits do not match pid.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorCommandTest, UIDirectorCommandHelperNodeIdPidMismatchTest, TestSize.Level1)
{
    RSContext rsContext;
    constexpr pid_t pid = 400;
    constexpr uint64_t token = 401;
    constexpr NodeId nodeId = MakeNodeId(pid + 1, 0);

    RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, pid, token);
    EXPECT_EQ(rsContext.GetUIRenderDirector(pid, token), nullptr);

    RSUIDirectorCommandHelper::GoResume(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoForeground(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoBackground(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, pid, token);
    RSUIDirectorCommandHelper::GoDestroy(rsContext, nodeId, pid, token);
    EXPECT_EQ(rsContext.GetUIRenderDirector(pid, token), nullptr);
}

/**
 * @tc.name: UIDirectorCommandHelperMismatchWithExistingDirectorTest
 * @tc.desc: Test mismatched nodeId does not affect an already created director.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorCommandTest, UIDirectorCommandHelperMismatchWithExistingDirectorTest, TestSize.Level1)
{
    RSContext rsContext;
    constexpr pid_t pid = 500;
    constexpr uint64_t token = 501;
    constexpr NodeId validNodeId = MakeNodeId(pid, 0);
    constexpr NodeId invalidNodeId = MakeNodeId(pid + 1, 0);

    RSUIDirectorCommandHelper::GoCreate(rsContext, validNodeId, pid, token);
    auto director = rsContext.GetUIRenderDirector(pid, token);
    ASSERT_NE(director, nullptr);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    RSUIDirectorCommandHelper::GoResume(rsContext, invalidNodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    RSUIDirectorCommandHelper::GoForeground(rsContext, invalidNodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    RSUIDirectorCommandHelper::GoBackground(rsContext, invalidNodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    RSUIDirectorCommandHelper::GoStop(rsContext, invalidNodeId, pid, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    RSUIDirectorCommandHelper::GoDestroy(rsContext, invalidNodeId, pid, token);
    EXPECT_NE(rsContext.GetUIRenderDirector(pid, token), nullptr);
}

} // namespace OHOS::Rosen
