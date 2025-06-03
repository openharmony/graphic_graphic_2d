/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_data.h"
#include "rs_frame_report.h"
#include "pipeline/rs_root_render_node.h"

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
    RSRenderThread::Instance().PostPreTask();
    EXPECT_TRUE(RSRenderThread::Instance().handler_ == nullptr);
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
    RSRenderThread::Instance().PostPreTask();
    EXPECT_TRUE(RSRenderThread::Instance().handler_ == nullptr);
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
    EXPECT_TRUE(data == nullptr);
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
    EXPECT_EQ(RSRenderThread::Instance().handler_, nullptr);
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
    EXPECT_EQ(RSRenderThread::Instance().activeWindowCnt_, 0);
}

/**
 * @tc.name: RequestNextVSync001
 * @tc.desc: test results of RequestNextVSync
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, RequestNextVSync001, TestSize.Level1)
{
    RSRenderThread::Instance().RequestNextVSync();
    EXPECT_TRUE(RSRenderThread::Instance().hasSkipVsync_);

    RSRenderThread::Instance().handler_ = std::make_shared<AppExecFwk::EventHandler>();
    RSRenderThread::Instance().RequestNextVSync();
    EXPECT_TRUE(RSRenderThread::Instance().hasSkipVsync_);
}

/**
 * @tc.name: RequestNextVSync002
 * @tc.desc: test results of RequestNextVSync
 * @tc.type:FUNC
 * @tc.require: issueIAJ76O
 */
HWTEST_F(RSRenderThreadTest, RequestNextVSync002, TestSize.Level1)
{
    auto renderThread = std::make_shared<RSRenderThread>();
    EXPECT_FALSE(renderThread->hasSkipVsync_);
    renderThread->runner_ = AppExecFwk::EventRunner::Create(false);
    renderThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(renderThread->runner_);
    auto rsClient = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    std::string name = "RSRenderThread_1";
    renderThread->receiver_ = rsClient->CreateVSyncReceiver(name, renderThread->handler_);
    EXPECT_TRUE(renderThread->receiver_ != nullptr);
    renderThread->RequestNextVSync();
    EXPECT_FALSE(renderThread->hasSkipVsync_);
}

/**
 * @tc.name: GetTid001
 * @tc.desc: test results of GetTid
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, GetTid001, TestSize.Level1)
{
    int32_t tId = RSRenderThread::Instance().GetTid();
    EXPECT_EQ(tId, -1);
}

/**
 * @tc.name: OnVsync001
 * @tc.desc: test results of OnVsync
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, OnVsync001, TestSize.Level1)
{
    uint64_t timestamp = 123456; // for test update
    uint64_t frameCount = 1; // for test
    RSRenderThread::Instance().activeWindowCnt_ = 1;
    RSRenderThread::Instance().OnVsync(timestamp, frameCount);
    EXPECT_EQ(RSRenderThread::Instance().timestamp_, timestamp);

    RSRenderThread::Instance().activeWindowCnt_ = 0;
    RSRenderThread::Instance().OnVsync(timestamp, frameCount);
    EXPECT_EQ(RSRenderThread::Instance().timestamp_, timestamp);
}

/**
 * @tc.name: ProcessCommands001
 * @tc.desc: test results of ProcessCommands
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, ProcessCommands001, TestSize.Level1)
{
    RSRenderThread::Instance().cmds_.clear();
    RSRenderThread::Instance().prevTimestamp_ = 1; // for test
    RSRenderThread::Instance().ProcessCommands();
    EXPECT_EQ(RSRenderThread::Instance().cmds_.empty(), true);

    auto unique = std::make_unique<RSTransactionData>();
    RSRenderThread::Instance().cmds_.push_back(std::move(unique));
    RSRenderThread::Instance().ProcessCommands();
    EXPECT_NE(RSRenderThread::Instance().cmds_.empty(), false);

    RsFrameReport::GetInstance().frameSchedSoLoaded_ = true;
    RsFrameReport::GetInstance().frameGetEnableFunc_ = []() { return 1; };
    RSRenderThread::Instance().ProcessCommands();
    EXPECT_NE(RSRenderThread::Instance().cmds_.empty(), false);

    RSRenderThread::Instance().commandTimestamp_ = 1; // for test
    RSRenderThread::Instance().timestamp_ = 17666669 * 2; // for test
    RSRenderThread::Instance().ProcessCommands();
    EXPECT_NE(RSRenderThread::Instance().cmds_.empty(), false);
}

/**
 * @tc.name: Animate001
 * @tc.desc: test results of Animate
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Animate001, TestSize.Level1)
{
    uint64_t timestamp = 1; // for test update
    RSRenderThread::Instance().context_ = std::make_shared<RSContext>();
    RSRenderThread::Instance().context_->animatingNodeList_.clear();
    RSRenderThread::Instance().Animate(timestamp);

    NodeId id = 1; // for test
    NodeId tid = 1; // for test
    auto render = std::make_shared<RSRenderNode>(id);
    RSRenderThread::Instance().context_->animatingNodeList_[id] = render;
    RSRenderThread::Instance().context_->animatingNodeList_[tid] = render;
    RsFrameReport::GetInstance().frameSchedSoLoaded_ = false;
    RSRenderThread::Instance().Animate(timestamp);

    RSRenderThread::Instance().context_->animatingNodeList_.clear();
    render = nullptr;
    RSRenderThread::Instance().context_->animatingNodeList_[id] = render;
    RSRenderThread::Instance().context_->animatingNodeList_[tid] = render;
    RSRenderThread::Instance().Animate(timestamp);
    EXPECT_EQ(RSRenderThread::Instance().lastAnimateTimestamp_, timestamp);
}

/**
 * @tc.name: Render001
 * @tc.desc: test results of Render
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, Render001, TestSize.Level1)
{
    RSRenderThread::Instance().visitor_ = nullptr;
    RSRenderThread::Instance().Render();
    EXPECT_NE(RSRenderThread::Instance().visitor_, nullptr);

    RsFrameReport::GetInstance().frameSchedSoLoaded_ = true;
    RSRenderThread::Instance().Render();
    EXPECT_NE(RSRenderThread::Instance().context_->globalRootRenderNode_, nullptr);

    RSRenderThread::Instance().context_->globalRootRenderNode_ = nullptr;
    RSRenderThread::Instance().Render();
    EXPECT_EQ(RSRenderThread::Instance().context_->globalRootRenderNode_, nullptr);
}

/**
 * @tc.name: SendCommands001
 * @tc.desc: test results of SendCommands
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, SendCommands001, TestSize.Level1)
{
    RSRenderThread::Instance().SendCommands();
    EXPECT_EQ(RSRenderThread::Instance().mValue, 0);

    RsFrameReport::GetInstance().frameSchedSoLoaded_ = false;
    RSRenderThread::Instance().SendCommands();
    EXPECT_EQ(RSRenderThread::Instance().mValue, 0);
}

/**
 * @tc.name: PostTask001
 * @tc.desc: test results of PostTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, PostTask001, TestSize.Level1)
{
    RSTaskMessage::RSTask task;
    RSRenderThread::Instance().PostTask(task);
    EXPECT_TRUE(RSRenderThread::Instance().handler_ != nullptr);

    RSRenderThread::Instance().handler_ = nullptr;
    RSRenderThread::Instance().PostTask(task);
    EXPECT_TRUE(RSRenderThread::Instance().handler_ == nullptr);
}

/**
 * @tc.name: PostSyncTask001
 * @tc.desc: test results of PostSyncTask
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadTest, PostSyncTask001, TestSize.Level1)
{
    RSTaskMessage::RSTask task;
    RSRenderThread::Instance().PostSyncTask(task);
    EXPECT_TRUE(RSRenderThread::Instance().handler_ == nullptr);

    RSRenderThread::Instance().handler_ = std::make_shared<AppExecFwk::EventHandler>();
    RSRenderThread::Instance().PostSyncTask(task);
    EXPECT_TRUE(RSRenderThread::Instance().handler_ != nullptr);
}

/**
 * @tc.name: Detach
 * @tc.desc: test results of Detach
 * @tc.type: FUNC
 * @tc.require: issueI9TXX3
 */
HWTEST_F(RSRenderThreadTest, Detach, TestSize.Level1)
{
    NodeId id = 0;
    auto renderNode = std::make_shared<RSRootRenderNode>(0);
    RSRenderThread::Instance().Detach(id);
    EXPECT_EQ(RSRenderThread::Instance().mValue, 0);
}

/**
 * @tc.name: PostPreTask
 * @tc.desc: test results of PostPreTask
 * @tc.type:FUNC
 * @tc.require: issueI9TXX3
 */
HWTEST_F(RSRenderThreadTest, PostPreTask, TestSize.Level1)
{
    RSRenderThread::Instance().PostPreTask();
    EXPECT_TRUE(RSRenderThread::Instance().handler_ != nullptr);

    RSRenderThread::Instance().handler_ = nullptr;
    RSRenderThread::Instance().PostPreTask();
    EXPECT_TRUE(RSRenderThread::Instance().handler_ == nullptr);

    RSRenderThread::Instance().preTask_ = []() {};
    RSRenderThread::Instance().PostPreTask();
    EXPECT_TRUE(RSRenderThread::Instance().handler_ == nullptr);

    RSRenderThread::Instance().handler_ = std::make_shared<AppExecFwk::EventHandler>();
    RSRenderThread::Instance().PostPreTask();
    EXPECT_TRUE(RSRenderThread::Instance().handler_ != nullptr);
}

/**
 * @tc.name: StartTest
 * @tc.desc: test results of Start
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderThreadTest, StartTest, TestSize.Level1)
{
    RSRenderThread::Instance().thread_ = nullptr;
    std::thread wakeUpThread([&]() {
        RSRenderThread::Instance().Start();
    });
    wakeUpThread.join();
    EXPECT_TRUE(RSRenderThread::Instance().thread_);

    std::thread wakeUpThreadStart([&]() {
        RSRenderThread::Instance().Start();
    });
    wakeUpThreadStart.join();
    EXPECT_TRUE(RSRenderThread::Instance().thread_);
}

/**
 * @tc.name: CreateAndInitRenderContextIfNeedTest
 * @tc.desc: test results of CreateAndInitRenderContextIfNeed
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderThreadTest, CreateAndInitRenderContextIfNeedTest, TestSize.Level1)
{
    RSRenderThread::Instance().CreateAndInitRenderContextIfNeed();
    EXPECT_TRUE(RSRenderThread::Instance().thread_);

    RSRenderThread::Instance().CreateAndInitRenderContextIfNeed();
    EXPECT_TRUE(RSRenderThread::Instance().thread_);

    RSRenderThread::Instance().CreateAndInitRenderContextIfNeed();
    EXPECT_TRUE(RSRenderThread::Instance().thread_);

    RSRenderThread::Instance().CreateAndInitRenderContextIfNeed();
    EXPECT_TRUE(RSRenderThread::Instance().thread_);
}

/**
 * @tc.name: HighContrastSettingsAndChanges
 * @tc.desc: test results of HighContrastSettingsAndChanges
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderThreadTest, HighContrastSettingsAndChanges, TestSize.Level1)
{
    RSRenderThread::Instance().SetHighContrast(true);
    EXPECT_TRUE(RSRenderThread::Instance().IsHighContrastEnabled());
    EXPECT_TRUE(RSRenderThread::Instance().IsHighContrastChanged());

    RSRenderThread::Instance().ResetHighContrastChanged();
    EXPECT_FALSE(RSRenderThread::Instance().IsHighContrastChanged());
}

/**
 * @tc.name: GetIsRunning001
 * @tc.desc: test GetIsRunning while start thread
 * @tc.type: FUNC
 * @tc.require: issueICB7BS
 */
HWTEST_F(RSRenderThreadTest, GetIsRunning001, TestSize.Level1)
{
    RSRenderThread::Instance().running_.store(true);
    ASSERT_TRUE(RSRenderThread::Instance().GetIsRunning());
}
} // namespace OHOS::Rosen