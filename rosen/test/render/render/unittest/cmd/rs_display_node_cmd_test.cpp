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
#include "include/command/rs_display_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayNodeCmdUniTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayNodeCmdUniTest::SetUpTestCase() {}
void RSDisplayNodeCmdUniTest::TearDownTestCase() {}
void RSDisplayNodeCmdUniTest::SetUp() {}
void RSDisplayNodeCmdUniTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, Create001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);

    NodeId id2 = static_cast<NodeId>(2);
    config.isMirrored = true;
    DisplayNodeCommandHelper::Create(context, id2, config);

    NodeId id3 = static_cast<NodeId>(3);
    config.mirrorNodeId = id2;
    DisplayNodeCommandHelper::Create(context, id3, config);
}

/**
 * @tc.name: TestRSBaseNodeCmdUniTest001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, TestRSDisplayNodeCmdUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    uint64_t screenId = static_cast<uint64_t>(0);
    DisplayNodeCommandHelper::SetScreenId(context, id, screenId);

    NodeId id2 = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id2, config);
    DisplayNodeCommandHelper::SetScreenId(context, id2, screenId);
}

/**
 * @tc.name: TestRSDisplayNodeCmdUniTest002
 * @tc.desc: SetDisplayOffset test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, TestRSDisplayNodeCmdUniTest002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    int32_t offsetX = static_cast<int32_t>(1);
    int32_t offsetY = static_cast<int32_t>(1);
    DisplayNodeCommandHelper::SetDisplayOffset(context, id, offsetX, offsetY);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayOffset(context, id, offsetX, offsetY);
}

/**
 * @tc.name: TestRSDisplayNodeCmdUniTest003
 * @tc.desc: SetSecurityDisplay test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, TestRSDisplayNodeCmdUniTest003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    bool isSecurityDisplay = false;
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);
}

/**
 * @tc.name: TestRSDisplayNodeCmdUniTest004
 * @tc.desc: SetDisplayMode test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, TestRSDisplayNodeCmdUniTest004, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
}

/**
 * @tc.name: RemoveDisplayNodeFromTreeUniTest001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, RemoveDisplayNodeFromTreeUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(context, id);

    NodeId id2 = static_cast<NodeId>(2);
    DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(context, id2);
}

/**
 * @tc.name: AddDisplayNodeToTreeUniTest001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, AddDisplayNodeToTreeUniTest001, TestSize.Level1)
{
    RSContext context;
    auto renderNodeChildren = context.GetGlobalRootRenderNode()->children_;

    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::AddDisplayNodeToTree(context, id);
    ASSERT_EQ(renderNodeChildren.size(), 0);

    NodeId id2 = static_cast<NodeId>(2);
    DisplayNodeCommandHelper::AddDisplayNodeToTree(context, id2);
    ASSERT_EQ(renderNodeChildren.size(), 0);
}

/**
 * @tc.name: SetBootAnimationUniTest001
 * @tc.desc: test.
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSDisplayNodeCmdUniTest, SetBootAnimationUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetBootAnimation(context, id, true);

    DisplayNodeCommandHelper::SetBootAnimation(context, 5, true);
}

/**
 * @tc.name: SetScreenRotationUniTest001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, SetScreenRotationUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::SetScreenRotation(context, id, ScreenRotation::ROTATION_0);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetScreenRotation(context, id, ScreenRotation::ROTATION_0);
}

/**
 * @tc.name: SetRogSizeUniTest001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, SetRogSizeUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::SetScreenId(context, id, 1);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetScreenId(context, id, 1);
}

/**
 * @tc.name: SetRogSizeUniTest
 * @tc.desc: SetRogSize test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSDisplayNodeCmdUniTest, SetRogSizeUniTest, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    int32_t offsetX = static_cast<int32_t>(1);
    int32_t offsetY = static_cast<int32_t>(1);
    DisplayNodeCommandHelper::SetRogSize(context, id, offsetX, offsetY);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetRogSize(context, id, offsetX, offsetY);
}

/**
 * @tc.name: SetDisplayModeUniTest001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, SetDisplayModeUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    config.isMirrored = true;
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    NodeId mirrorNodeId = static_cast<NodeId>(2);
    config.mirrorNodeId = mirrorNodeId;
    DisplayNodeCommandHelper::Create(context, mirrorNodeId, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
}

/**
 * @tc.name: SetScbNodePidUniTest
 * @tc.desc: SetScbNodePid test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCmdUniTest, SetScbNodePidUniTest, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    std::vector<int32_t> oldScbPids = {};
    int32_t currentScbPid = -1;
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    oldScbPids.push_back(1);
    oldScbPids.push_back(2);
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
}

/**
 * @tc.name: SetScbNodePidUniTest001
 * @tc.desc: SetScbNodePid test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSDisplayNodeCmdUniTest, SetScbNodePidUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    std::vector<int32_t> oldScbPids = {};
    int32_t currentScbPid = -1;
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);

    oldScbPids.push_back(1);
    oldScbPids.push_back(2);
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    DisplayNodeCommandHelper::SetScbNodePid(context, 5, oldScbPids, currentScbPid);
}
} // namespace OHOS::Rosen