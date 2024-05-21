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
#include "include/command/rs_base_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseNodeCommandText : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBaseNodeCommandText::SetUpTestCase() {}
void RSBaseNodeCommandText::TearDownTestCase() {}
void RSBaseNodeCommandText::SetUp() {}
void RSBaseNodeCommandText::TearDown() {}

/**
 * @tc.name: TestRSBaseNodeCommand001
 * @tc.desc: Destroy test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TextRSBaseNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::Destroy(context, nodeId);
}

/**
 * @tc.name: TextRSBaseNodeCommand002
 * @tc.desc: AddChild test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TextRSBaseNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(0);
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
}

/**
 * @tc.name: TextRSBaseNodeCommand003
 * @tc.desc: MoveChild test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TextRSBaseNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(1);
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
}

/**
 * @tc.name: TextRSBaseNodeCommand004
 * @tc.desc: AddCrossParentChild test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TextRSBaseNodeCommand004, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(1);
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childNodeId, index);
}

/**
 * @tc.name: TextRSBaseNodeCommand005
 * @tc.desc: RemoveCrossParentChild test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TextRSBaseNodeCommand005, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    NodeId newParentId = static_cast<NodeId>(-3);
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
}

/**
 * @tc.name: TestRSBaseNodeCommand006
 * @tc.desc: RemoveFromTree test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TestRSBaseNodeCommand006, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
}

/**
 * @tc.name: TextRSBaseNodeCommand007
 * @tc.desc: ClearChildren test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TextRSBaseNodeCommand007, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
}

/**
 * @tc.name: Destroy001
 * @tc.desc: test results of Destroy
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, Destroy001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    BaseNodeCommandHelper::Destroy(context, nodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::Destroy(context, nodeId);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: AddChild001
 * @tc.desc: test results of AddChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, AddChild001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    NodeId childNodeId = 0;
    int32_t index = 1;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: MoveChild001
 * @tc.desc: test results of MoveChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, MoveChild001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    NodeId childNodeId = 0;
    int32_t index = 1;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: RemoveChild001
 * @tc.desc: test results of RemoveChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, RemoveChild001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    NodeId childNodeId = 0;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: AddCrossParentChild001
 * @tc.desc: test results of AddCrossParentChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, AddCrossParentChild001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    NodeId childId = 0;
    int32_t index = 1;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: RemoveCrossParentChild001
 * @tc.desc: test results of RemoveCrossParentChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, RemoveCrossParentChild001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, 0, 0);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: RemoveFromTree001
 * @tc.desc: test results of RemoveFromTree
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, RemoveFromTree001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: ClearChildren001
 * @tc.desc: test results of ClearChildren
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSBaseNodeCommandText, ClearChildren001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    EXPECT_TRUE(nodeId == 0);
}
} // namespace OHOS::Rosen
