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
} // namespace OHOS::Rosen