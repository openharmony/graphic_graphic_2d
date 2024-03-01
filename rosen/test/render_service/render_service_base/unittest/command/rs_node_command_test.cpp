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
#include "include/command/rs_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeCommandTest::SetUpTestCase() {}
void RSNodeCommandTest::TearDownTestCase() {}
void RSNodeCommandTest::SetUp() {}
void RSNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSBaseNodeCommand001
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, TestRSNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    PropertyId propertyId = static_cast<PropertyId>(1);
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
}

/**
 * @tc.name: TestRSBaseNodeCommand002
 * @tc.desc: AddModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, TestRSBaseNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSNodeCommandHelper::AddModifier(context, nodeId, modifier);
}

/**
 * @tc.name: TestRSBaseNodeCommand003
 * @tc.desc: SetFreeze test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, TestRSBaseNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
}

/**
 * @tc.name: MarkNodeGroupTest
 * @tc.desc: MarkNodeGroup test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, MarkNodeGroupTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    bool isNodeGroup = false;
    RSNodeCommandHelper::MarkNodeGroup(context, nodeId, isNodeGroup, true, false);
}

/**
 * @tc.name: MarkContentChangedTest
 * @tc.desc: MarkContentChanged test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, MarkContentChangedTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    bool isChanged = false;
    RSNodeCommandHelper::MarkContentChanged(context, nodeId, isChanged);
}

/**
 * @tc.name: SetDrawRegionTest
 * @tc.desc: SetDrawRegion test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, SetDrawRegionTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    std::shared_ptr<RectF> rect = nullptr;
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
}

/**
 * @tc.name: RegisterGeometryTransitionPairTest
 * @tc.desc: RegisterGeometryTransitionPair test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, RegisterGeometryTransitionPairTest, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = static_cast<NodeId>(1);
    NodeId outNodeId = static_cast<NodeId>(1);
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
}
} // namespace OHOS::Rosen
