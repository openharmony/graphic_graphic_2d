/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
class RSDisplayNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayNodeCommandUnitTest::SetUpTestCase() {}
void RSDisplayNodeCommandUnitTest::TearDownTestCase() {}
void RSDisplayNodeCommandUnitTest::SetUp() {}
void RSDisplayNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestRSDisplayNodeCommand01
 * @tc.desc: Verify function RSDisplayNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestRSDisplayNodeCommand01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    uint64_t screenId = static_cast<uint64_t>(0);
    DisplayNodeCommandHelper::SetScreenId(context, id, screenId);
    ASSERT_EQ(id, static_cast<NodeId>(-1));

    NodeId id2 = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id2, config);
    DisplayNodeCommandHelper::SetScreenId(context, id2, screenId);
    ASSERT_EQ(id2, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestRSDisplayNodeCommand02
 * @tc.desc: Verify function RSDisplayNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestRSDisplayNodeCommand02, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    int32_t offsetX = static_cast<int32_t>(1);
    int32_t offsetY = static_cast<int32_t>(1);
    DisplayNodeCommandHelper::SetDisplayOffset(context, id, offsetX, offsetY);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayOffset(context, id, offsetX, offsetY);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestRSDisplayNodeCommand03
 * @tc.desc: Verify function RSDisplayNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestRSDisplayNodeCommand03, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    bool isSecurityDisplay = false;
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestRSDisplayNodeCommand04
 * @tc.desc: Verify function RSDisplayNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestRSDisplayNodeCommand04, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestCreate01
 * @tc.desc: Verify function Create
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestCreate01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    ASSERT_EQ(id, static_cast<NodeId>(1));

    NodeId id2 = static_cast<NodeId>(2);
    config.isMirrored = true;
    DisplayNodeCommandHelper::Create(context, id2, config);
    ASSERT_EQ(id2, static_cast<NodeId>(2));

    NodeId id3 = static_cast<NodeId>(3);
    config.mirrorNodeId = id2;
    DisplayNodeCommandHelper::Create(context, id3, config);
    ASSERT_EQ(id3, static_cast<NodeId>(3));
}

/**
 * @tc.name: TestAddDisplayNodeToTree01
 * @tc.desc: Verify function AddDisplayNodeToTree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestAddDisplayNodeToTree01, TestSize.Level1)
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
 * @tc.name: TestRemoveDisplayNodeFromTree01
 * @tc.desc: Verify function RemoveDisplayNodeFromTree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestRemoveDisplayNodeFromTree01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(context, id);
    ASSERT_EQ(id, static_cast<NodeId>(1));

    NodeId id2 = static_cast<NodeId>(2);
    DisplayNodeCommandHelper::RemoveDisplayNodeFromTree(context, id2);
    ASSERT_EQ(id2, static_cast<NodeId>(2));
}

/**
 * @tc.name: TestSetBootAnimation01
 * @tc.desc: Verify function SetBootAnimation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestSetBootAnimation01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetBootAnimation(context, id, true);

    DisplayNodeCommandHelper::SetBootAnimation(context, 5, true);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestSetRogSize01
 * @tc.desc: Verify function SetRogSize
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestSetRogSize01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::SetScreenId(context, id, 1);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetScreenId(context, id, 1);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestSetScreenRotation01
 * @tc.desc: Verify function SetScreenRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestSetScreenRotation01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    DisplayNodeCommandHelper::SetScreenRotation(context, id, ScreenRotation::ROTATION_0);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetScreenRotation(context, id, ScreenRotation::ROTATION_0);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestSetDisplayMode01
 * @tc.desc: Verify function SetDisplayMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestSetDisplayMode01, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    ASSERT_EQ(id, static_cast<NodeId>(1));

    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    config.isMirrored = true;
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    NodeId mirrorNodeId = static_cast<NodeId>(2);
    config.mirrorNodeId = mirrorNodeId;
    DisplayNodeCommandHelper::Create(context, mirrorNodeId, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    ASSERT_EQ(mirrorNodeId, static_cast<NodeId>(2));
}

/**
 * @tc.name: TestSetScbNodePid
 * @tc.desc: Verify function SetScbNodePid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestSetScbNodePid, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    std::vector<int32_t> oldScbPids = {};
    int32_t currentScbPid = -1;
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    oldScbPids.push_back(1);
    oldScbPids.push_back(2);
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestSetRogSize
 * @tc.desc: Verify function SetRogSize
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestSetRogSize, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    int32_t offsetX = static_cast<int32_t>(1);
    int32_t offsetY = static_cast<int32_t>(1);
    DisplayNodeCommandHelper::SetRogSize(context, id, offsetX, offsetY);

    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetRogSize(context, id, offsetX, offsetY);
    ASSERT_EQ(id, static_cast<NodeId>(1));
}

/**
 * @tc.name: TestSetScbNodePid01
 * @tc.desc: Verify function SetScbNodePid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayNodeCommandUnitTest, TestSetScbNodePid01, TestSize.Level1)
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
    ASSERT_EQ(id, static_cast<NodeId>(1));
}
} // namespace OHOS::Rosen
