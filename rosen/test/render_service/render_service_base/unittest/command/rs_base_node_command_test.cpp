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
 * @tc.name: TextRSBaseNodeCommand008
 * @tc.desc: ClearSurfaceNodeChildren test.
 * @tc.type: FUNC
 */
HWTEST_F(RSBaseNodeCommandText, TextRSBaseNodeCommand008, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::ClearSurfaceNodeChildren(context, nodeId);
}
} // namespace OHOS::Rosen
