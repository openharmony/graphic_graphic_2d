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

#include "wl_display_test.h"

namespace OHOS {
void WlDisplayTest::SetUp()
{
}

void WlDisplayTest::TearDown()
{
}

void WlDisplayTest::SetUpTestCase()
{
    display = WlDisplay::GetInstance();
}

void WlDisplayTest::TearDownTestCase()
{
}

void WlDisplayTest::SyncDone(void *, struct wl_callback *, uint32_t)
{
    callCount++;
}

namespace {
/*
 * Feature: WlDisplay connect default socket
 * Function: WlDisplay
 * SubFunction: connect
 * FunctionPoints: WlDisplay connect
 * EnvConditions: weston is avaliable
 * CaseDescription: 1. check fd
 *                  2. display connect to socket
 *                  3. check fd, error
 */
HWTEST_F(WlDisplayTest, Connect, testing::ext::TestSize.Level0)
{
    // 1. check fd
    int32_t fd = display->GetFd();
    ASSERT_EQ(fd, -1) << "CaseDescription: 1. check fd (fd == -1)";

    // 2. display connect to socket
    bool bret = display->Connect();
    ASSERT_EQ(bret, true) << "CaseDescription: 2. display connect to socket (bret == true)";

    // 3. check fd, error
    fd = display->GetFd();
    ASSERT_GE(fd, 0) << "CaseDescription: 3. check fd, error (fd >= 0)";
}

/*
 * Feature: WlDisplay SyncPromise
 * Function: WlDisplay
 * SubFunction: Sync With DispatchThread
 * FunctionPoints: WlDisplay Sync, StartDispatchThread, StopDispatchThread
 * EnvConditions: WlDisplay connected
 * CaseDescription: 1. StartDispatchThread
 *                  2. wl_display_sync
 *                  3. Sync
 *                  4. check sync is called
 *                  5. StopDispatchThread
 */
HWTEST_F(WlDisplayTest, SyncPromise, testing::ext::TestSize.Level0)
{
    // WlDisplay connected
    ASSERT_GE(display->GetFd(), 0) << "EnvConditions: WlDisplay connected (display->GetFd() >= 0)";

    // 1. StartDispatchThread
    display->StartDispatchThread();

    // 2. wl_display_sync
    callCount = 0;
    auto callback = wl_display_sync(display->GetRawPtr());
    ASSERT_NE(callback, nullptr) << "CaseDescription: 2. wl_display_sync (callback != nullptr)";

    const struct wl_callback_listener listener = { &WlDisplayTest::SyncDone };
    auto ret = wl_callback_add_listener(callback, &listener, nullptr);
    ASSERT_NE(ret, -1) << "CaseDescription: 2. wl_display_sync (ret != -1)";

    // 3. Sync
    display->Sync();

    // 4. check sync is called
    ASSERT_EQ(callCount, 1) << "CaseDescription: 4. check sync is called (callCount == 1)";

    // 5. StopDispatchThread
    display->StopDispatchThread();
}

/*
 * Feature: WlDisplay disconnect
 * Function: WlDisplay
 * SubFunction: disconnect
 * FunctionPoints: WlDisplay disconnect
 * EnvConditions: WlDisplay connected
 * CaseDescription: 1. disconnect
 *                  2. check fd
 */
HWTEST_F(WlDisplayTest, Disconnect, testing::ext::TestSize.Level0)
{
    const auto& display = WlDisplay::GetInstance();

    // 1. disconnect
    display->Disconnect();

    // 2. check fd
    int32_t fd = display->GetFd();
    ASSERT_EQ(fd, -1) << "CaseDescription 2: check fd (fd == -1)";
}
} // namespace
} // namespace OHOS
