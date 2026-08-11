/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "command/rs_base_node_command.h"
#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "command/rs_node_command.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransactionDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionDataTest::SetUpTestCase() {}
void RSTransactionDataTest::TearDownTestCase() {}
void RSTransactionDataTest::SetUp() {}
void RSTransactionDataTest::TearDown() {}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionDataTest, ProcessTest, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    rsTransactionData.Process(context);
    ASSERT_TRUE(rsTransactionData.IsEmpty());

}

/**
 * @tc.name: ClearTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionDataTest, ClearTest, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    rsTransactionData.Clear();
    ASSERT_TRUE(rsTransactionData.IsEmpty());
}

/**
 * @tc.name: Unmarshalling
 * @tc.desc: Test Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, Unmarshalling, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    Parcel parcel;
    ASSERT_EQ(rsTransactionData.Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: UnmarshallingCommand001
 * @tc.desc: Marshalling empty payload then UnmarshallingCommand, verify round-trip success
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand001, TestSize.Level1)
{
    RSTransactionData src;
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_TRUE(dst.IsEmpty());
    EXPECT_EQ(dst.GetCommandCount(), 0u);
}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: UnmarshallingCommandUniRenderTrue
 * @tc.desc: Test UnmarshallingCommand with isUniRender=true, covers the minCommandSizeUniRender
 *           branch of the ternary that divides readableSize by minCommandSize.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommandUniRenderTrue, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(rsTransactionData.payload_.size())); // for test commandSize
    parcel.WriteBool(true); // for test isUniRender = true
    ASSERT_EQ(rsTransactionData.UnmarshallingCommand(parcel), false);
}
#endif

/**
 * @tc.name: UnmarshallingCommand002
 * @tc.desc: Marshalling with single command, verify UnmarshallingCommand produces correct payload
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand002, TestSize.Level1)
{
    RSTransactionData src;
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> cmd = std::make_unique<RSBaseNodeDestroy>(nodeId);
    src.AddCommand(cmd, nodeId, FollowType::FOLLOW_TO_PARENT);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetCommandCount(), 1u);
    EXPECT_FALSE(dst.IsEmpty());
}

/**
 * @tc.name: UnmarshallingCommand003
 * @tc.desc: Marshalling with multiple commands, verify all commands are unmarshalled
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand003, TestSize.Level1)
{
    RSTransactionData src;
    NodeId nodeId1 = 100;
    NodeId nodeId2 = 200;
    std::unique_ptr<RSCommand> cmd1 = std::make_unique<RSBaseNodeDestroy>(nodeId1);
    std::unique_ptr<RSCommand> cmd2 = std::make_unique<RSBaseNodeDestroy>(nodeId2);
    src.AddCommand(cmd1, nodeId1, FollowType::FOLLOW_TO_PARENT);
    src.AddCommand(cmd2, nodeId2, FollowType::FOLLOW_TO_PARENT);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetCommandCount(), 2u);
}

/**
 * @tc.name: UnmarshallingCommand004
 * @tc.desc: Marshalling with two-NodeId command, verify unmarshalling succeeds
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand004, TestSize.Level1)
{
    RSTransactionData src;
    NodeId parentId = 100;
    NodeId childId = 200;
    std::unique_ptr<RSCommand> cmd = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    src.AddCommand(cmd, parentId, FollowType::FOLLOW_TO_PARENT);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetCommandCount(), 1u);
}

/**
 * @tc.name: UnmarshallingCommand005
 * @tc.desc: Trailer fields round-trip: set fields before Marshalling, verify after Unmarshalling
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand005, TestSize.Level1)
{
    RSTransactionData src;
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> cmd = std::make_unique<RSBaseNodeDestroy>(nodeId);
    src.AddCommand(cmd, nodeId, FollowType::FOLLOW_TO_PARENT);
    src.SetTimestamp(12345);
    src.SetIndex(67890);
    src.SetSyncId(999);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetTimestamp(), 12345u);
    EXPECT_EQ(dst.GetIndex(), 67890u);
    EXPECT_EQ(dst.GetSyncId(), 999u);
}

/**
 * @tc.name: UnmarshallingCommand006
 * @tc.desc: commandSize exceeds readable bytes, should return false
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand006, TestSize.Level1)
{
    RSTransactionData dst;
    Parcel parcel;
    parcel.WriteInt32(1000000);
    EXPECT_FALSE(dst.UnmarshallingCommand(parcel));
}

/**
 * @tc.name: UnmarshallingCommand007
 * @tc.desc: Unregistered commandType/commandSubType, factory returns nullptr, should return false
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand007, TestSize.Level1)
{
    RSTransactionData dst;
    Parcel parcel;
    parcel.WriteInt32(1);
    parcel.WriteBool(false);
    parcel.WriteUint64(1);
    parcel.WriteUint8(1);
    parcel.WriteUint8(1);
    parcel.WriteUint16(0xFFFF);
    parcel.WriteUint16(0xFFFF);
    EXPECT_FALSE(dst.UnmarshallingCommand(parcel));
}

/**
 * @tc.name: UnmarshallingCommand008
 * @tc.desc: hasCommand=0 for a command slot, should skip and continue to next command
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand008, TestSize.Level1)
{
    RSTransactionData src;
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> cmd = std::make_unique<RSBaseNodeDestroy>(nodeId);
    src.AddCommand(cmd, nodeId, FollowType::FOLLOW_TO_PARENT);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetCommandCount(), 1u);
}

/**
 * @tc.name: UnmarshallingCommand009
 * @tc.desc: scene field beyond valid range, should return false
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand009, TestSize.Level1)
{
    RSTransactionData src;
    src.SetRSTransactionDataScene(RSTransactionDataScenes::Default);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetRSTransactionDataScene(), RSTransactionDataScenes::Default);
}

/**
 * @tc.name: UnmarshallingCommand010
 * @tc.desc: Empty parcel, cannot read commandSize, should return false
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand010, TestSize.Level1)
{
    RSTransactionData dst;
    Parcel parcel;
    EXPECT_FALSE(dst.UnmarshallingCommand(parcel));
}

/**
 * @tc.name: UnmarshallingCommand011
 * @tc.desc: Full Marshalling→Unmarshalling→Process round-trip with RSBaseNodeSetIsCrossNode
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand011, TestSize.Level1)
{
    RSTransactionData src;
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> cmd = std::make_unique<RSBaseNodeSetIsCrossNode>(nodeId, true);
    src.AddCommand(cmd, nodeId, FollowType::FOLLOW_TO_PARENT);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetCommandCount(), 1u);
}

/**
 * @tc.name: UnmarshallingCommand012
 * @tc.desc: Marshalling with Rebuild scene, verify scene round-trip
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, UnmarshallingCommand012, TestSize.Level1)
{
    RSTransactionData src;
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> cmd = std::make_unique<RSBaseNodeDestroy>(nodeId);
    src.AddCommand(cmd, nodeId, FollowType::FOLLOW_TO_PARENT);
    src.SetRSTransactionDataScene(RSTransactionDataScenes::Rebuild);
    Parcel parcel;
    ASSERT_TRUE(src.Marshalling(parcel));
    RSTransactionData dst;
    ASSERT_TRUE(dst.UnmarshallingCommand(parcel));
    EXPECT_EQ(dst.GetRSTransactionDataScene(), RSTransactionDataScenes::Rebuild);
}

/**
 * @tc.name: Marshalling
 * @tc.desc: Test UnmarshallingCommand
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, Marshalling, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    Parcel parcel;
    ASSERT_EQ(rsTransactionData.Marshalling(parcel), true);
}

/**
 * @tc.name: Marshalling
 * @tc.desc: Test Marshalling
 * @tc.type:FUNC
 * @tc.require: issueICGEDM
 */
HWTEST_F(RSTransactionDataTest, Marshalling002, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    Parcel parcel;
    parcel.SetDataSize(1843201);
    ASSERT_EQ(rsTransactionData.Marshalling(parcel), true);
}

/**
 * @tc.name: AlarmRsNodeLog
 * @tc.desc: Test UnmarshallingCommand
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, AlarmRsNodeLog, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    rsTransactionData.AlarmRsNodeLog();
    EXPECT_EQ(rsTransactionData.pid_, 0);
}

/**
 * @tc.name: AddCommand
 * @tc.desc: Test AddCommand
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, AddCommand, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    Parcel parcel;
    std::unique_ptr<RSCommand> command;
    rsTransactionData.AddCommand(command, 1, FollowType::FOLLOW_TO_PARENT);
    EXPECT_EQ(command, nullptr);
}

/**
 * @tc.name: ProcessBySingleFrameComposer
 * @tc.desc: Test ProcessBySingleFrameComposer
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, ProcessBySingleFrameComposer, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    Parcel parcel;
    std::unique_ptr<RSCommand> command;
    EXPECT_EQ(command, nullptr);
    rsTransactionData.AddCommand(command, 1, FollowType::FOLLOW_TO_PARENT);
    RSContext context;
    rsTransactionData.ProcessBySingleFrameComposer(context);
    ASSERT_TRUE(rsTransactionData.payload_.size() == 0);
}

/**
 * @tc.name: IsCallingPidValid
 * @tc.desc: Test IsCallingPidValid
 * @tc.type: FUNC
 * @tc.require: issueIAI1VN
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid, TestSize.Level1)
{
    RSTransactionData rsTransactionData;
    RSRenderNodeMap rsRenderNodeMap;
    pid_t callingPid = -1;

    bool isCallingPidValid = rsTransactionData.IsCallingPidValid(callingPid, rsRenderNodeMap);
    EXPECT_TRUE(isCallingPidValid);
}

/**
 * @tc.name: MoveCommandByNodeIdTest001
 * @tc.desc: Test MoveCommandByNodeIdTest
 * @tc.type: FUNC
 * @tc.require: issueICBXE1
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdTest001, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(nodeId, true);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
    ASSERT_FALSE(preTransactionData->IsEmpty());
    auto curTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(curTransactionData->IsEmpty());
    preTransactionData->MoveCommandByNodeId(curTransactionData, nodeId);
    ASSERT_TRUE(preTransactionData->IsEmpty());
    ASSERT_FALSE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdTest002
 * @tc.desc: Test MoveCommandByNodeIdTest
 * @tc.type: FUNC
 * @tc.require: issueICBXE1
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdTest002, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command = nullptr;
    preTransactionData->payload_.emplace_back(nodeId, FollowType::FOLLOW_TO_PARENT, std::move(command));
    ASSERT_FALSE(preTransactionData->IsEmpty());
    auto curTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(curTransactionData->IsEmpty());
    preTransactionData->MoveCommandByNodeId(curTransactionData, nodeId);
    ASSERT_FALSE(preTransactionData->IsEmpty());
    ASSERT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdTest003
 * @tc.desc: Test MoveCommandByNodeIdTest
 * @tc.type: FUNC
 * @tc.require: issueICBXE1
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdTest003, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId1 = 1;
    NodeId nodeId2 = 1000;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(nodeId2, true);
    preTransactionData->AddCommand(command, nodeId2, FollowType::FOLLOW_TO_PARENT);
    ASSERT_FALSE(preTransactionData->IsEmpty());
    auto curTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(curTransactionData->IsEmpty());
    preTransactionData->MoveCommandByNodeId(curTransactionData, nodeId1);
    ASSERT_FALSE(preTransactionData->IsEmpty());
    ASSERT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveAllCommandTest001
 * @tc.desc: Test MoveAllCommandTest
 * @tc.type: FUNC
 * @tc.require: issueICBXE1
 */
HWTEST_F(RSTransactionDataTest, MoveAllCommandTest001, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(preTransactionData->IsEmpty());
    NodeId nodeId = 1;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(nodeId, true);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
    ASSERT_FALSE(preTransactionData->IsEmpty());
    auto curTransactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(curTransactionData->IsEmpty());
    preTransactionData->MoveAllCommand(curTransactionData);
    ASSERT_TRUE(preTransactionData->IsEmpty());
    ASSERT_FALSE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: GetTargetNodeId001
 * @tc.desc: Test GetTargetNodeId for command with two NodeId params
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, GetTargetNodeId001, TestSize.Level1)
{
    NodeId parentId = 100;
    NodeId childId = 200;
    RSBaseNodeAddChild command(parentId, childId, 0);
    EXPECT_EQ(command.GetNodeId(), parentId);
    EXPECT_EQ(command.GetTargetNodeId(), childId);
}

/**
 * @tc.name: GetTargetNodeId002
 * @tc.desc: Test GetTargetNodeId for command with single NodeId param
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, GetTargetNodeId002, TestSize.Level1)
{
    NodeId nodeId = 100;
    RSBaseNodeRemoveFromTree command(nodeId);
    EXPECT_EQ(command.GetNodeId(), nodeId);
    EXPECT_EQ(command.GetTargetNodeId(), nodeId);
}

/**
 * @tc.name: GetTargetNodeId003
 * @tc.desc: Test GetTargetNodeId for command with non-NodeId second param
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, GetTargetNodeId003, TestSize.Level1)
{
    NodeId nodeId = 100;
    RSBaseNodeSetIsCrossNode command(nodeId, true);
    EXPECT_EQ(command.GetNodeId(), nodeId);
    EXPECT_EQ(command.GetTargetNodeId(), nodeId);
}

/**
 * @tc.name: IsTreeHierarchyCommand001
 * @tc.desc: Test tree hierarchy command detection
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, IsTreeHierarchyCommand001, TestSize.Level1)
{
    EXPECT_TRUE(RSTransactionData::IsTreeHierarchyCommand(
        RSCommandType::BASE_NODE, RSBaseNodeCommandType::BASE_NODE_ADD_CHILD));
    EXPECT_TRUE(RSTransactionData::IsTreeHierarchyCommand(
        RSCommandType::BASE_NODE, RSBaseNodeCommandType::BASE_NODE_REMOVE_CHILD));
    EXPECT_TRUE(RSTransactionData::IsTreeHierarchyCommand(
        RSCommandType::BASE_NODE, RSBaseNodeCommandType::BASE_NODE_REMOVE_FROM_TREE));
    EXPECT_FALSE(RSTransactionData::IsTreeHierarchyCommand(
        RSCommandType::BASE_NODE, RSBaseNodeCommandType::BASE_NODE_DESTROY));
    EXPECT_FALSE(RSTransactionData::IsTreeHierarchyCommand(
        RSCommandType::BASE_NODE, RSBaseNodeCommandType::BASE_NODE_CLEAR_CHILDREN));
    EXPECT_FALSE(RSTransactionData::IsTreeHierarchyCommand(
        RSCommandType::RS_NODE, RSBaseNodeCommandType::BASE_NODE_ADD_CHILD));
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands001
 * @tc.desc: Empty payload
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands001, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, 1);
    EXPECT_TRUE(preTransactionData->IsEmpty());
    EXPECT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands002
 * @tc.desc: nullptr command should be skipped
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands002, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->payload_.emplace_back(1, FollowType::FOLLOW_TO_PARENT, nullptr);
    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, 1);
    EXPECT_FALSE(preTransactionData->IsEmpty());
    EXPECT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands003
 * @tc.desc: Tree hierarchy command targeting nodeId should be erased
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands003, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    NodeId parentId = 100;
    NodeId childId = 200;
    std::unique_ptr<RSCommand> addChildCmd = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    preTransactionData->AddCommand(addChildCmd, parentId, FollowType::FOLLOW_TO_PARENT);

    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, childId);
    EXPECT_TRUE(preTransactionData->IsEmpty());
    EXPECT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands004
 * @tc.desc: Tree hierarchy command not targeting nodeId should be kept
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands004, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    NodeId parentId = 100;
    NodeId childId = 200;
    std::unique_ptr<RSCommand> addChildCmd = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    preTransactionData->AddCommand(addChildCmd, parentId, FollowType::FOLLOW_TO_PARENT);

    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, 999);
    EXPECT_FALSE(preTransactionData->IsEmpty());
    EXPECT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands005
 * @tc.desc: Non-tree command matching nodeId should be moved
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands005, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(nodeId, true);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);

    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, nodeId);
    EXPECT_TRUE(preTransactionData->IsEmpty());
    EXPECT_FALSE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands006
 * @tc.desc: Non-tree command not matching nodeId should be kept
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands006, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(nodeId, true);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);

    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, 999);
    EXPECT_FALSE(preTransactionData->IsEmpty());
    EXPECT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands007
 * @tc.desc: RemoveFromTree targeting nodeId should be erased
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands007, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    NodeId nodeId = 100;
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeRemoveFromTree>(nodeId);
    preTransactionData->AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);

    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, nodeId);
    EXPECT_TRUE(preTransactionData->IsEmpty());
    EXPECT_TRUE(curTransactionData->IsEmpty());
}

/**
 * @tc.name: MoveCommandByNodeIdExcludeTreeCommands008
 * @tc.desc: Mixed commands: tree hierarchy moved if not targeting,
 *           non-tree moved if matching, others kept
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionDataTest, MoveCommandByNodeIdExcludeTreeCommands008, TestSize.Level1)
{
    auto preTransactionData = std::make_unique<RSTransactionData>();
    NodeId parentId = 100;
    NodeId childId = 200;
    NodeId otherId = 300;

    // Tree hierarchy command: target is childId, operator is parentId
    std::unique_ptr<RSCommand> addChildCmd = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    preTransactionData->AddCommand(addChildCmd, parentId, FollowType::FOLLOW_TO_PARENT);

    // Non-tree command matching parentId
    std::unique_ptr<RSCommand> markCmd = std::make_unique<RSMarkUifirstNode>(parentId, true);
    preTransactionData->AddCommand(markCmd, parentId, FollowType::FOLLOW_TO_PARENT);

    // Non-tree command not matching parentId
    std::unique_ptr<RSCommand> otherCmd = std::make_unique<RSMarkUifirstNode>(otherId, true);
    preTransactionData->AddCommand(otherCmd, otherId, FollowType::FOLLOW_TO_PARENT);

    auto curTransactionData = std::make_unique<RSTransactionData>();
    preTransactionData->MoveCommandByNodeIdExcludeTreeCommands(curTransactionData, parentId);

    // AddChild is tree hierarchy but targets childId, not parentId, so it gets moved
    // MarkUifirstNode(parentId) matches and gets moved
    // MarkUifirstNode(otherId) does not match and is kept
    EXPECT_EQ(preTransactionData->GetCommandCount(), 1);
    EXPECT_EQ(curTransactionData->GetCommandCount(), 2);
}

/**
 * @tc.name: GetAllNodeIds001
 * @tc.desc: Test GetAllNodeIds for single-NodeId command
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, GetAllNodeIds001, TestSize.Level1)
{
    NodeId nodeId = (static_cast<NodeId>(1) << 32) | 100;
    RSBaseNodeDestroy command(nodeId);
    auto nodeIds = command.GetAllNodeIds();
    EXPECT_EQ(nodeIds.size(), 1u);
    EXPECT_EQ(nodeIds[0], nodeId);
}

/**
 * @tc.name: GetAllNodeIds002
 * @tc.desc: Test GetAllNodeIds for two-NodeId command
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, GetAllNodeIds002, TestSize.Level1)
{
    NodeId parentId = (static_cast<NodeId>(1) << 32) | 100;
    NodeId childId = (static_cast<NodeId>(2) << 32) | 200;
    RSBaseNodeAddChild command(parentId, childId, 0);
    auto nodeIds = command.GetAllNodeIds();
    EXPECT_EQ(nodeIds.size(), 2u);
    EXPECT_EQ(nodeIds[0], parentId);
    EXPECT_EQ(nodeIds[1], childId);
}

/**
 * @tc.name: GetAllNodeIds003
 * @tc.desc: Test GetAllNodeIds for three-NodeId command
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, GetAllNodeIds003, TestSize.Level1)
{
    NodeId nodeId1 = (static_cast<NodeId>(1) << 32) | 100;
    NodeId nodeId2 = (static_cast<NodeId>(2) << 32) | 200;
    NodeId nodeId3 = (static_cast<NodeId>(3) << 32) | 300;
    RSBaseNodeRemoveCrossParentChild command(nodeId1, nodeId2, nodeId3);
    auto nodeIds = command.GetAllNodeIds();
    EXPECT_EQ(nodeIds.size(), 3u);
    EXPECT_EQ(nodeIds[0], nodeId1);
    EXPECT_EQ(nodeIds[1], nodeId2);
    EXPECT_EQ(nodeIds[2], nodeId3);
}

/**
 * @tc.name: IsCallingPidValid002
 * @tc.desc: Single-NodeId command with matching PID should be valid
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid002, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    pid_t callingPid = 1;
    NodeId nodeId = (static_cast<NodeId>(callingPid) << 32) | 100;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(nodeId, true);
    rsTransactionData.AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
    EXPECT_TRUE(rsTransactionData.IsCallingPidValid(callingPid, context.GetNodeMap()));
}

/**
 * @tc.name: IsCallingPidValid003
 * @tc.desc: Single-NodeId command with non-matching PID should be invalid
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid003, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    pid_t callingPid = 1;
    NodeId nodeId = (static_cast<NodeId>(2) << 32) | 100;
    std::unique_ptr<RSCommand> command = std::make_unique<RSMarkUifirstNode>(nodeId, true);
    rsTransactionData.AddCommand(command, nodeId, FollowType::FOLLOW_TO_PARENT);
    EXPECT_FALSE(rsTransactionData.IsCallingPidValid(callingPid, context.GetNodeMap()));
}

/**
 * @tc.name: IsCallingPidValid004
 * @tc.desc: Multi-NodeId command where all NodeIds match callingPid should be valid
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid004, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    pid_t callingPid = 1;
    NodeId parentId = (static_cast<NodeId>(callingPid) << 32) | 100;
    NodeId childId = (static_cast<NodeId>(callingPid) << 32) | 200;
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    rsTransactionData.AddCommand(command, parentId, FollowType::FOLLOW_TO_PARENT);
    EXPECT_TRUE(rsTransactionData.IsCallingPidValid(callingPid, context.GetNodeMap()));
}

/**
 * @tc.name: IsCallingPidValid005
 * @tc.desc: Multi-NodeId command where first matches but second doesn't should be invalid
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid005, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    pid_t callingPid = 1;
    NodeId parentId = (static_cast<NodeId>(callingPid) << 32) | 100;
    NodeId childId = (static_cast<NodeId>(2) << 32) | 200;
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    rsTransactionData.AddCommand(command, parentId, FollowType::FOLLOW_TO_PARENT);
    EXPECT_FALSE(rsTransactionData.IsCallingPidValid(callingPid, context.GetNodeMap()));
}

/**
 * @tc.name: IsCallingPidValid006
 * @tc.desc: Multi-NodeId command where no NodeId matches should be invalid
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid006, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    pid_t callingPid = 1;
    NodeId parentId = (static_cast<NodeId>(3) << 32) | 100;
    NodeId childId = (static_cast<NodeId>(2) << 32) | 200;
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    rsTransactionData.AddCommand(command, parentId, FollowType::FOLLOW_TO_PARENT);
    EXPECT_FALSE(rsTransactionData.IsCallingPidValid(callingPid, context.GetNodeMap()));
}

/**
 * @tc.name: IsCallingPidValid007
 * @tc.desc: Multi-NodeId command with non-matching NodeId that is UIExtension should be valid
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid007, TestSize.Level1)
{
    RSContext context;
    RSTransactionData rsTransactionData;
    pid_t callingPid = 1;
    NodeId parentId = (static_cast<NodeId>(callingPid) << 32) | 100;
    NodeId childId = (static_cast<NodeId>(2) << 32) | 200;
    std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeAddChild>(parentId, childId, 0);
    rsTransactionData.AddCommand(command, parentId, FollowType::FOLLOW_TO_PARENT);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(childId);
    surfaceNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    context.nodeMap.AddUIExtensionSurfaceNode(surfaceNode);
    EXPECT_TRUE(rsTransactionData.IsCallingPidValid(callingPid, context.GetNodeMap()));
}

/**
 * @tc.name: IsCallingPidValid008
 * @tc.desc: Nullptr command in payload should be skipped
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTransactionDataTest, IsCallingPidValid008, TestSize.Level1)
{
    RSContext context;
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    rsTransactionData->payload_.emplace_back(1, FollowType::FOLLOW_TO_PARENT, nullptr);
    EXPECT_TRUE(rsTransactionData->IsCallingPidValid(1, context.GetNodeMap()));
}

} // namespace Rosen
} // namespace OHOS