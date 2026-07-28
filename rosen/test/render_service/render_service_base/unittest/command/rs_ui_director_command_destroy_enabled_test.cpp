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
#include "parameters.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_ui_render_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIDirectorCommandDestroyEnabledTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDirectorCommandDestroyEnabledTest::SetUpTestCase()
{
    // IsDestroyTokenNodeOnStopEnabled is statically cached on first call, so the enabled branch
    // is covered in a dedicated test binary with the switch forced on.
    OHOS::system::SetParameter("persist.sys.graphic.destroytokennodeonstop.enabled", "true");
}
void RSUIDirectorCommandDestroyEnabledTest::TearDownTestCase()
{
    OHOS::system::SetParameter("persist.sys.graphic.destroytokennodeonstop.enabled", "");
}
void RSUIDirectorCommandDestroyEnabledTest::SetUp() {}
void RSUIDirectorCommandDestroyEnabledTest::TearDown() {}

/**
 * @tc.name: UIDirectorCommandHelperGoStopDestroyEnabledTest
 * @tc.desc: Test GoStop destroys token node when destroy switch is enabled and syncs STOP state.
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSUIDirectorCommandDestroyEnabledTest, UIDirectorCommandHelperGoStopDestroyEnabledTest, TestSize.Level1)
{
    RSContext rsContext;
    constexpr pid_t pid = 500;
    constexpr uint64_t token = 501;
    constexpr NodeId nodeId = MakeNodeId(pid, 0);

    auto node = std::make_shared<RSRenderNode>(nodeId);
    node->SetUIContextToken(token);
    rsContext.GetMutableNodeMap().RegisterRenderNode(node);

    RSUIDirectorCommandHelper::GoCreate(rsContext, nodeId, token);
    auto director = rsContext.GetUIRenderDirector(pid, token);
    ASSERT_NE(director, nullptr);

    RSUIDirectorCommandHelper::GoStop(rsContext, nodeId, token);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
    EXPECT_EQ(rsContext.GetMutableNodeMap().GetRenderNode(nodeId), nullptr);
}

} // namespace OHOS::Rosen
