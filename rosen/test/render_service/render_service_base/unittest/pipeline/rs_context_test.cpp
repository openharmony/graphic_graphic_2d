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
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSContextTest::SetUpTestCase() {}
void RSContextTest::TearDownTestCase() {}
void RSContextTest::SetUp() {}
void RSContextTest::TearDown() {}

/**
 * @tc.name: AnNodeRecordOrDeleteTest001
 * @tc.desc: AnimationNodeRecordOrDelete test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSContextTest, AnNodeRecordOrDeleteTest001, TestSize.Level1)
{
    RSContext rSContext;
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(1);
    EXPECT_TRUE(nodePtr);
    rSContext.RegisterAnimatingRenderNode(nodePtr);
    rSContext.UnregisterAnimatingRenderNode(1);
}
/**
 * @tc.name: ActivityNodesRecordTest002
 * @tc.desc: ActivityNodesRecord test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSContextTest, ActivityNodesRecordTest002, TestSize.Level1)
{
    RSContext rSContext;

    // is nodePtr nullptr handling
    std::shared_ptr<RSRenderNode> nodePtr = nullptr;
    rSContext.AddActiveNode(nodePtr);
    EXPECT_FALSE(rSContext.HasActiveNode(nodePtr));

    // is nodePtr invalid value handling
    nodePtr = std::make_shared<RSRenderNode>(INVALID_NODEID);
    EXPECT_TRUE(nodePtr);
    rSContext.AddActiveNode(nodePtr);
    EXPECT_FALSE(rSContext.HasActiveNode(nodePtr));

    // is nodePtr effective value handling
    nodePtr = std::make_shared<RSRenderNode>(1);
    EXPECT_TRUE(nodePtr);
    rSContext.AddActiveNode(nodePtr);
    EXPECT_TRUE(rSContext.HasActiveNode(nodePtr));
}

/**
 * @tc.name: PendingSyncNodesRecordTest003
 * @tc.desc: PendingSyncNodesRecord test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSContextTest, PendingSyncNodesRecordTest003, TestSize.Level1)
{
    RSContext rSContext;

    // is nodePtr nullptr handling
    std::shared_ptr<RSRenderNode> nodePtr = nullptr;
    rSContext.AddPendingSyncNode(nodePtr);

    // is nodePtr invalid value handling
    nodePtr = std::make_shared<RSRenderNode>(INVALID_NODEID);
    EXPECT_TRUE(nodePtr);
    rSContext.AddPendingSyncNode(nodePtr);

    // is nodePtr effective value handling
    nodePtr = std::make_shared<RSRenderNode>(1);
    EXPECT_TRUE(nodePtr);
    rSContext.AddPendingSyncNode(nodePtr);
}

/**
 * @tc.name: TaskRunnerTest004
 * @tc.desc: TaskRunner test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSContextTest, TaskRunnerTest004, TestSize.Level1)
{
    RSContext rSContext;

    // Set Task Runner
    auto taskRunner = [](const std::function<void()>& task, bool isSyncTask) {};
    rSContext.SetTaskRunner(taskRunner);
    auto taskRunnerTest = []() {};
    rSContext.PostTask(taskRunnerTest, true);

    // Set RTTask Runner
    auto rtTaskRunner = [](const std::function<void()>& task) {};
    rSContext.SetRTTaskRunner(rtTaskRunner);
    rSContext.PostRTTask(taskRunnerTest);
}

/**
 * @tc.name: InitializeTest005
 * @tc.desc: Initialize test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSContextTest, InitializeTest005, TestSize.Level1)
{
    RSContext rSContext;
    rSContext.Initialize();
}
} // namespace OHOS::Rosen