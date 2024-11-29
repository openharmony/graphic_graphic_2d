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
#include "include/command/rs_base_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BaseNodeCommandText : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BaseNodeCommandText::SetUpTestCase() {}
void BaseNodeCommandText::TearDownTestCase() {}
void BaseNodeCommandText::SetUp() {}
void BaseNodeCommandText::TearDown() {}

/**
 * @tc.name: TextRSBaseNodeCommand002
 * @tc.desc: AddChild test.
 * @tc.type: FUNC
 */
HWTEST_F(BaseNodeCommandText, TextRSBaseNodeCommand1, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(0);
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_NE(nodeId, nullptr);
}

/**
 * @tc.name: Destroy001
 * @tc.desc: test results of Destroy
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, Destroy01, TestSize.Level1)
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
 * @tc.name: TextRSBaseNodeCommand005
 * @tc.desc: RemoveCrossParentChild test.
 * @tc.type: FUNC
 */
HWTEST_F(BaseNodeCommandText, TextRSBaseNodeCommand3, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    NodeId newParentId = static_cast<NodeId>(-3);
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_NE(context, nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand006
 * @tc.desc: RemoveFromTree test.
 * @tc.type: FUNC
 */
HWTEST_F(BaseNodeCommandText, TestRSBaseNodeCommand4, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    EXPECT_NE(nodeId, nullptr);
}

/**
 * @tc.name: TextRSBaseNodeCommand007
 * @tc.desc: ClearChildren test.
 * @tc.type: FUNC
 */
HWTEST_F(BaseNodeCommandText, TextRSBaseNodeCommand5, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    EXPECT_NE(nodeId, nullptr);
}

/**
 * @tc.name: AddChild001
 * @tc.desc: test results of AddChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, AddChild1, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    int32_t index = 1;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);
    nodeId = 1;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);
    nodeId = 0;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);
    childNodeId = 0;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: AddCrossParentChild001
 * @tc.desc: test results of AddCrossParentChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, AddCrossParentChild7, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childId = 1;
    int32_t index = 1;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId);

    nodeId = 1;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId == 0);

    childId = 0;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: MoveChild001
 * @tc.desc: test results of MoveChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, MoveChild, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    int32_t index = 1;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);

    childNodeId = 0;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);

    nodeId = 0;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);

    nodeId = 1;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);
}

/**
 * @tc.name: TextRSBaseNodeCommand004
 * @tc.desc: AddCrossParentChild test.
 * @tc.type: FUNC
 */
HWTEST_F(BaseNodeCommandText, TextRSBaseNodeCommand8, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(1);
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childNodeId, index);
    EXPECT_NE(context, nullptr);
}

/**
 * @tc.name: RemoveChild001
 * @tc.desc: test results of RemoveChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, RemoveChild001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId == 0);

    childNodeId = 0;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId == 0);

    nodeId = 1;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId);
}

/**
 * @tc.name: ClearChildren001
 * @tc.desc: test results of ClearChildren
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, ClearChildren001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    EXPECT_TRUE(!nodeId);
}

/**
 * @tc.name: RemoveCrossParentChild001
 * @tc.desc: test results of RemoveCrossParentChild
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, RemoveCrossParentChild001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    NodeId newParentId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!nodeId);

    nodeId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(nodeId);

    childNodeId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);

    childNodeId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);

    newParentId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);

    childNodeId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(childNodeId);

    newParentId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);
}

/**
 * @tc.name: RemoveFromTree001
 * @tc.desc: test results of RemoveFromTree
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(BaseNodeCommandText, RemoveFromTree001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: TestRSBaseNodeCommand001
 * @tc.desc: Destroy test.
 * @tc.type: FUNC
 */
HWTEST_F(BaseNodeCommandText, TextRSBaseNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::Destroy(context, nodeId);
    EXPECT_NE(nodeId, nullptr);
}
} // namespace OHOS::Rosen
