/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "rs_thread_looper_impl.h"
#include <thread>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSThreadLooperImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<ThreadLooperImpl> threadlooperimpl_;
};

void RSThreadLooperImplTest::SetUpTestCase() {}
void RSThreadLooperImplTest::TearDownTestCase() {}
void RSThreadLooperImplTest::SetUp()
{
    threadlooperimpl_ = std::make_shared<ThreadLooperImpl> ();
}
void RSThreadLooperImplTest::TearDown() {}

class ThreadLooperMessageTest final : public ThreadLooperMessage {
public:
    explicit ThreadLooperMessageTest() {};

    ~ThreadLooperMessageTest() override = default;

    void Process(int param) const override {}
};

/**
 * @tc.name: CreateThreadInstance001
 * @tc.desc: test results of CreateThreadInstance
 * @tc.type:FUNC
 * @tc.require:issueI5NMHT
 */
HWTEST_F(RSThreadLooperImplTest, CreateThreadInstance001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create CreateThreadInstance
     */
    auto threadlooperimpl = threadlooperimpl_->CreateThreadInstance();
    ASSERT_TRUE(threadlooperimpl != nullptr);
}

/**
 * @tc.name: CreateThreadInstance002
 * @tc.desc: test results of CreateThreadInstance
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThreadLooperImplTest, CreateThreadInstance002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create CreateThreadInstance
     */
    auto threadlooperimpl = threadlooperimpl_->CreateThreadInstance();
    ASSERT_TRUE(threadlooperimpl != nullptr);
    threadlooperimpl = threadlooperimpl_->CreateThreadInstance();
    ASSERT_TRUE(threadlooperimpl != nullptr);
}

/**
 * @tc.name: GetThreadInstance001
 * @tc.desc: test results of GetThreadInstance
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThreadLooperImplTest, GetThreadInstance001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create GetThreadInstance
     */
    auto threadlooperimpl = threadlooperimpl_->CreateThreadInstance();
    ASSERT_TRUE(threadlooperimpl != nullptr);
    threadlooperimpl = threadlooperimpl_->GetThreadInstance();
    ASSERT_TRUE(threadlooperimpl != nullptr);
}

/**
 * @tc.name: ProcessOneMessageTest
 * @tc.desc: test results of ProcessOneMessage
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThreadLooperImplTest, ProcessOneMessageTest, TestSize.Level1)
{
    int timeoutMillis = 1;
    threadlooperimpl_->ProcessOneMessage(timeoutMillis);
}

/**
 * @tc.name: ProcessAllMessagesTest
 * @tc.desc: test results of ProcessAllMessages
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThreadLooperImplTest, ProcessAllMessagesTest, TestSize.Level1)
{
    int timeoutMillis = 0;
    threadlooperimpl_->ProcessAllMessages(timeoutMillis);
}

/**
 * @tc.name: RemoveMessagesTest
 * @tc.desc: Verify function RemoveMessages
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperImplTest, RemoveMessagesTest, TestSize.Level1)
{
    auto message = std::make_shared<ThreadLooperMessageTest>();
    auto message2 = std::make_shared<ThreadLooperMessageTest>();
    threadlooperimpl_->PostMessage(message, 0);
    threadlooperimpl_->PostMessage(0, message, 0);
    threadlooperimpl_->RemoveMessages(message2);
    threadlooperimpl_->RemoveMessages(message);
    EXPECT_TRUE(threadlooperimpl_->queue_.empty());
}

/**
 * @tc.name: HaveDelayedMessageToProcessTest
 * @tc.desc: Verify function HaveDelayedMessageToProcess
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperImplTest, HaveDelayedMessageToProcessTest, TestSize.Level1)
{
    auto message = std::make_shared<ThreadLooperMessageTest>();
    EXPECT_FALSE(threadlooperimpl_->HaveDelayedMessageToProcess());
    threadlooperimpl_->PostMessage(0, message, 0);
    EXPECT_TRUE(threadlooperimpl_->HaveDelayedMessageToProcess());
}

/**
 * @tc.name: WaitForMessageTest
 * @tc.desc: Verify function WaitForMessage
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperImplTest, WaitForMessageTest, TestSize.Level1)
{
    auto message = std::make_shared<ThreadLooperMessageTest>();
    std::thread wakeUpThread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        threadlooperimpl_->wakeup_ = true;
        threadlooperimpl_->cv_.notify_one();
    });
    threadlooperimpl_->WaitForMessage(-1);
    wakeUpThread.join();
    threadlooperimpl_->WaitForMessage(1);
    threadlooperimpl_->PostMessage(message, 0);
    threadlooperimpl_->WaitForMessage(1);
    threadlooperimpl_->queue_.clear();
    threadlooperimpl_->PostMessage(0, message, 0);
    threadlooperimpl_->WaitForMessage(1);
    EXPECT_TRUE(threadlooperimpl_->queue_.empty());
}

/**
 * @tc.name: ProcessOneMessageInternalTest
 * @tc.desc: Verify function ProcessOneMessageInternal
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperImplTest, ProcessOneMessageInternalTest, TestSize.Level1)
{
    auto message = std::make_shared<ThreadLooperMessageTest>();
    threadlooperimpl_->PostMessage(0, message, 0);
    EXPECT_TRUE(threadlooperimpl_->ProcessOneMessageInternal());
    threadlooperimpl_->PostMessage(message, 0);
    EXPECT_TRUE(threadlooperimpl_->ProcessOneMessageInternal());
}

/**
 * @tc.name: ProcessAllMessagesTest002
 * @tc.desc: Verify function ProcessAllMessages
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperImplTest, ProcessAllMessagesTest002, TestSize.Level1)
{
    threadlooperimpl_->ProcessAllMessages(1);
    std::thread wakeUpThread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        threadlooperimpl_->wakeup_ = true;
        threadlooperimpl_->cv_.notify_one();
    });
    threadlooperimpl_->ProcessAllMessages(-1);
    wakeUpThread.join();
    EXPECT_TRUE(threadlooperimpl_->wakeup_);
}

/**
 * @tc.name: WakeUpTest
 * @tc.desc: Verify function WakeUp
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadLooperImplTest, WakeUpTest, TestSize.Level1)
{
    threadlooperimpl_->WakeUp();
    EXPECT_TRUE(threadlooperimpl_->wakeup_);
}
} // namespace OHOS::Rosen