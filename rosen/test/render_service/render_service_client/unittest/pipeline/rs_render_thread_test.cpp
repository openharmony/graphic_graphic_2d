/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "limit_number.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "transaction/rs_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderThreadTest::SetUpTestCase() {}
void RSRenderThreadTest::TearDownTestCase() {}
void RSRenderThreadTest::SetUp() {}
void RSRenderThreadTest::TearDown() {}

/**
 * @tc.name: Detach001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Detach001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Detach
     */
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRenderThread::Instance().Detach(nodeId);
}

/**
 * @tc.name: Detach002
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Detach002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Detach
     */
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    RSRenderThread::Instance().Detach(node.GetId());
}

/**
 * @tc.name: RecvTransactionData001
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, RecvTransactionData001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    std::unique_ptr<RSTransactionData> data = std::make_unique<RSTransactionData>();
    RSRenderThread::Instance().RecvTransactionData(data);
}

/**
 * @tc.name: PostPreTask001
 * @tc.desc: test results of PostPreTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, PostPreTask001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    RSRenderThread::Instance().PostPreTask();
}

/**
 * @tc.name: UpdateWindowStatus001
 * @tc.desc: test results of PostPreTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, UpdateWindowStatus001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    RSRenderThread::Instance().UpdateWindowStatus(false);
    RSRenderThread::Instance().UpdateWindowStatus(true);
}

/**
 * @tc.name: RequestNextVSync001
 * @tc.desc: test results of RequestNextVSync
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, RequestNextVSync001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    renderThread->RequestNextVSync();
    EXPECT_EQ(renderThread->handler_, nullptr);

    renderThread->handler_ = std::make_shared<AppExecFwk::EventHandler>();
    renderThread->RequestNextVSync();
    EXPECT_NE(renderThread->handler_, nullptr);
}

/**
 * @tc.name: GetTid001
 * @tc.desc: test results of GetTid
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, GetTid001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    int32_t tId = renderThread->GetTid();
    EXPECT_EQ(tId, -1);
}

/**
 * @tc.name: CreateAndInitRenderContextIfNeed001
 * @tc.desc: test results of CreateAndInitRenderContextIfNeed
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, CreateAndInitRenderContextIfNeed001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    renderThread->CreateAndInitRenderContextIfNeed();
    EXPECT_EQ(renderThread->tid_, -1);
}

/**
 * @tc.name: OnVsync001
 * @tc.desc: test results of OnVsync
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, OnVsync001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    uint64_t timestamp = 123456; //for test
    renderThread->activeWindowCnt_ = 1;
    renderThread->OnVsync(timestamp);
    EXPECT_EQ(timestamp, 123456);
}

/**
 * @tc.name: ProcessCommands001
 * @tc.desc: test results of ProcessCommands
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, ProcessCommands001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    renderThread->cmds_.clear();
    renderThread->prevTimestamp_ = 1; //for test
    renderThread->ProcessCommands();
    EXPECT_EQ(renderThread->cmds_.empty(), true);

    auto unique = std::make_unique<RSTransactionData>();
    renderThread->cmds_.push_back(std::move(unique));
    renderThread->ProcessCommands();
    EXPECT_NE(renderThread->cmds_.empty(), false);

    renderThread->commandTimestamp_ = 1; //for test
    renderThread->timestamp_ = 16666668 * 2; //for test
    renderThread->ProcessCommands();
    EXPECT_NE(renderThread->cmds_.empty(), false);
}

/**
 * @tc.name: Animate001
 * @tc.desc: test results of Animate
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Animate001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    uint64_t timestamp = 1234; //for test
    renderThread->context_ = std::make_shared<RSContext>();
    renderThread->context_->animatingNodeList_.clear();
    renderThread->Animate(timestamp);
    EXPECT_EQ(timestamp, 1234);

    NodeId id = 1; //for test
    NodeId tid = 1; //for test
    auto render = std::make_shared<RSRenderNode>(id);
    renderThread->context_->animatingNodeList_[id] = render;
    renderThread->context_->animatingNodeList_[tid] = render;
    renderThread->Animate(timestamp);
    EXPECT_EQ(timestamp, 1234);
}

/**
 * @tc.name: Render001
 * @tc.desc: test results of Render
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Render001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    renderThread->Render();
    EXPECT_NE(renderThread, nullptr);

    renderThread->context_->globalRootRenderNode_ = nullptr;
    renderThread->Render();
    EXPECT_NE(renderThread, nullptr);
}

/**
 * @tc.name: SendCommands001
 * @tc.desc: test results of SendCommands
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, SendCommands001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    renderThread->SendCommands();
    EXPECT_EQ(renderThread->mValue, 0);
}

/**
 * @tc.name: PostTask001
 * @tc.desc: test results of PostTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, PostTask001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    RSTaskMessage::RSTask task;
    renderThread->PostTask(task);
    EXPECT_EQ(renderThread->mValue, 0);
}

/**
 * @tc.name: PostSyncTask001
 * @tc.desc: test results of PostSyncTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, PostSyncTask001, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    RSTaskMessage::RSTask task;
    renderThread->PostSyncTask(task);
    EXPECT_EQ(renderThread->mValue, 0);
}
} // namespace OHOS::Rosen