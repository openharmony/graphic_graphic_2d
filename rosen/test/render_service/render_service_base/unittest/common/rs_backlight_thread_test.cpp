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

#include <atomic>

#include "gtest/gtest.h"

#include "common/rs_backlight_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBacklightThreadTest : public testing::Test {};

/**
 * @tc.name: PostTaskAndPostSyncTask
 * @tc.desc: Verify asynchronous and synchronous tasks are executed
 * @tc.type: FUNC
 */
HWTEST_F(RSBacklightThreadTest, PostTaskAndPostSyncTask, TestSize.Level1)
{
    auto& backlightThread = RSBacklightThread::Instance();
    std::atomic<bool> taskExecuted = false;
    backlightThread.PostTask([&taskExecuted]() { taskExecuted = true; });
    backlightThread.PostSyncTask([]() {});
    EXPECT_TRUE(taskExecuted.load());
}

/**
 * @tc.name: PostTaskAndPostSyncTaskWithNullHandler
 * @tc.desc: Verify task APIs safely return when handler is null
 * @tc.type: FUNC
 */
HWTEST_F(RSBacklightThreadTest, PostTaskAndPostSyncTaskWithNullHandler, TestSize.Level1)
{
    auto& backlightThread = RSBacklightThread::Instance();
    auto handler = backlightThread.handler_;
    backlightThread.handler_ = nullptr;

    backlightThread.PostTask([]() {});
    backlightThread.PostSyncTask([]() {});
    EXPECT_EQ(backlightThread.handler_, nullptr);

    backlightThread.handler_ = handler;
}
} // namespace OHOS::Rosen
