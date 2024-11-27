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
class RsRenderThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsRenderThreadTest::SetUpTestCase() {}
void RsRenderThreadTest::TearDownTestCase() {}
void RsRenderThreadTest::SetUp() {}
void RsRenderThreadTest::TearDown() {}

/**
 * @tc.name: Detach01
 * @tc.desc: test results of Detach
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadTest, Detach01, TestSize.Level1)
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
HWTEST_F(RsRenderThreadTest, Detach02, TestSize.Level1)
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
HWTEST_F(RsRenderThreadTest, RecvTransactionData01, TestSize.Level1)
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
HWTEST_F(RsRenderThreadTest, PostPreTask01, TestSize.Level1)
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
HWTEST_F(RsRenderThreadTest, UpdateWindowStatus01, TestSize.Level1)
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
HWTEST_F(RsRenderThreadTest, RequestNextVSync01, TestSize.Level1)
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
HWTEST_F(RsRenderThreadTest, RequestNextVSync02, TestSize.Level1)
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

} // namespace OHOS::Rosen