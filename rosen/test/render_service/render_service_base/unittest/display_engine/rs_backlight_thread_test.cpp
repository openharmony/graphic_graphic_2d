/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <chrono>
#include <future>
#include <memory>

#include "gtest/gtest.h"

#include "display_engine/rs_backlight_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBacklightThreadTest : public testing::Test {};

/**
 * @tc.name: PostTask
 * @tc.desc: Verify asynchronous tasks are executed when runner and handler are valid
 * @tc.type: FUNC
 */
HWTEST_F(RSBacklightThreadTest, PostTask, TestSize.Level1)
{
    constexpr auto waitTimeout = std::chrono::seconds(1);
    auto& backlightThread = RSBacklightThread::Instance();
    ASSERT_NE(backlightThread.runner_, nullptr);
    ASSERT_NE(backlightThread.handler_, nullptr);

    auto taskFinished = std::make_shared<std::promise<void>>();
    auto taskFuture = taskFinished->get_future();
    backlightThread.PostTask([taskFinished]() { taskFinished->set_value(); });
    EXPECT_EQ(taskFuture.wait_for(waitTimeout), std::future_status::ready);
}

/**
 * @tc.name: InitWithNullRunner
 * @tc.desc: Verify Init keeps handler null when runner creation fails
 * @tc.type: FUNC
 */
HWTEST_F(RSBacklightThreadTest, InitWithNullRunner, TestSize.Level1)
{
    RSBacklightThread backlightThread;
    backlightThread.Init(nullptr);
    EXPECT_EQ(backlightThread.runner_, nullptr);
    EXPECT_EQ(backlightThread.handler_, nullptr);
}

/**
 * @tc.name: PostTaskWithNullHandler
 * @tc.desc: Verify PostTask safely returns when handler is null
 * @tc.type: FUNC
 */
HWTEST_F(RSBacklightThreadTest, PostTaskWithNullHandler, TestSize.Level1)
{
    auto& backlightThread = RSBacklightThread::Instance();
    auto handler = backlightThread.handler_;
    backlightThread.handler_ = nullptr;

    bool taskExecuted = false;
    backlightThread.PostTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
    EXPECT_EQ(backlightThread.handler_, nullptr);

    backlightThread.handler_ = handler;
}

/**
 * @tc.name: PostTaskWithInvalidHandlerRunner
 * @tc.desc: Verify PostTask returns safely when handler cannot post to a valid runner
 * @tc.type: FUNC
 */
HWTEST_F(RSBacklightThreadTest, PostTaskWithInvalidHandlerRunner, TestSize.Level1)
{
    auto& backlightThread = RSBacklightThread::Instance();
    auto handler = backlightThread.handler_;
    backlightThread.handler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);

    bool taskExecuted = false;
    backlightThread.PostTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);

    backlightThread.handler_ = handler;
}
} // namespace OHOS::Rosen