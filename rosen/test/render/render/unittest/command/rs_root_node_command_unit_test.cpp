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
#include "include/command/rs_root_node_command.h"
#include "include/command/rs_surface_node_command.h"
#include "include/pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::Text;

namespace OHOS::Rosen {
class RSRootNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRootNodeCommandUnitTest::SetUpTestCase() {}
void RSRootNodeCommandUnitTest::TearDownTestCase() {}
void RSRootNodeCommandUnitTest::SetUp() {}
void RSRootNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestRSRootNodeCommand01
 * @tc.desc: Verify function AttachRSSurfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandUnitTest, TestRSRootNodeCommand01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    NodeId surfaceNodeId = static_cast<NodeId>(-2);
    RootNodeCommandHelper::AttachRSSurfaceNode(testContext, id, surfaceNodeId);
    RootNodeCommandHelper::Create(testContext, id);
    RootNodeCommandHelper::AttachRSSurfaceNode(testContext, id, surfaceNodeId);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
    ASSERT_EQ(surfaceNodeId, static_cast<NodeId>(-2));
}

/**
 * @tc.name: TestRSRootNodeCommand02
 * @tc.desc: Verify function SetEnableRender
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandUnitTest, TestRSRootNodeCommand02, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    bool flag = false;
    RootNodeCommandHelper::SetEnableRender(testContext, id, flag);
    RootNodeCommandHelper::Create(testContext, id);
    RootNodeCommandHelper::SetEnableRender(testContext, id, flag);
    ASSERT_EQ(id, static_cast<NodeId>(-1));
}

/**
 * @tc.name: TestRSRootNodeCommand03
 * @tc.desc: Verify function AttachToUniSurfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandUnitTest, TestRSRootNodeCommand03, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    NodeId surfaceNodeId = static_cast<NodeId>(-2);
    RootNodeCommandHelper::AttachToUniSurfaceNode(testContext, id, surfaceNodeId);
    RootNodeCommandHelper::Create(testContext, id);
    SurfaceNodeCommandHelper::Create(testContext, surfaceNodeId);
    RootNodeCommandHelper::AttachToUniSurfaceNode(testContext, id, surfaceNodeId);
}

/**
 * @tc.name: TestRSRootNodeCommand04
 * @tc.desc: Verify function UpdateSuggestedBufferSize
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandUnitTest, TestRSRootNodeCommand04, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(-1);
    int32_t width = static_cast<int32_t>(0);
    int32_t height = static_cast<int32_t>(0);
    RootNodeCommandHelper::UpdateSuggestedBufferSize(testContext, id, width, height);
    RootNodeCommandHelper::Create(testContext, id);
    RootNodeCommandHelper::UpdateSuggestedBufferSize(testContext, id, width, height);
}

/**
 * @tc.name: CreateTest
 * @tc.desc: Verify function Create
 * @tc.type:FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSRootNodeCommandUnitTest, CreateTest, TestSize.Level1)
{
    RSContext testContext;
    RootNodeCommandHelper::Create(testContext, 1, false);
}
} // namespace OHOS::Rosen
