/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "pipeline/buffer_thread/rs_buffer_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBufferThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline RSBufferThread& bufferThread = RSBufferThread::Instance();
};

void RSBufferThreadTest::SetUpTestCase() {}
void RSBufferThreadTest::TearDownTestCase() {}
void RSBufferThreadTest::SetUp() {}
void RSBufferThreadTest::TearDown() {}

HWTEST_F(RSBufferThreadTest, PostSyncTaskTest001, TestSize.Level1)
{
    std::function<void()> task = [](){};
    bufferThread.PostSyncTask(task);
    ASSERT_EQ(bufferThread.handler_, nullptr);
    bufferThread.runner_ = AppExecFwk::EventRunner::Create("RSBufferThread");
    bufferThread.handler_ = std::make_shared<AppExecFwk::EventHandler>(bufferThread.runner_);
    bufferThread.PostSyncTask(task);
    ASSERT_NE(bufferThread.handler_, nullptr);
    bufferThread.runner_ = nullptr;
    bufferThread.handler_ = nullptr;
}

HWTEST_F(RSBufferThreadTest, PostTaskTest001, TestSize.Level1)
{
    std::function<void()> task = [](){};
    bufferThread.PostTask(task);
    ASSERT_EQ(bufferThread.handler_, nullptr);
    bufferThread.runner_ = AppExecFwk::EventRunner::Create("RSBufferThread");
    bufferThread.handler_ = std::make_shared<AppExecFwk::EventHandler>(bufferThread.runner_);
    bufferThread.PostTask(task);
    ASSERT_NE(bufferThread.handler_, nullptr);
    bufferThread.runner_ = nullptr;
    bufferThread.handler_ = nullptr;
}

HWTEST_F(RSBufferThreadTest, PostDelayTaskTest001, TestSize.Level1)
{
    std::function<void()> task = [](){};
    bufferThread.PostDelayTask(task, 0);
    ASSERT_EQ(bufferThread.handler_, nullptr);
    bufferThread.runner_ = AppExecFwk::EventRunner::Create("RSBufferThread");
    bufferThread.handler_ = std::make_shared<AppExecFwk::EventHandler>(bufferThread.runner_);
    bufferThread.PostDelayTask(task, 0);
    ASSERT_NE(bufferThread.handler_, nullptr);
    bufferThread.runner_ = nullptr;
    bufferThread.handler_ = nullptr;
}
}