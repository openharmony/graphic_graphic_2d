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

#include "vsync_remote_callback_test.h"

#include <chrono>
#include <thread>
#include <unistd.h>

#include <iservice_registry.h>

#include "return_value_tester.h"

using namespace std::chrono_literals;

namespace OHOS {
namespace Vsync {
void VsyncCallbackTest::SetUp()
{
}

void VsyncCallbackTest::TearDown()
{
}

void VsyncCallbackTest::SetUpTestCase()
{
    pipe(pipeFd);

    pid_ = fork();
    if (pid_ < 0) {
        exit(1);
    }

    if (pid_ == 0) {
        std::this_thread::sleep_for(50ms);
        sptr<VsyncCallback> vcqp = new VsyncCallback();
        ASSERT_NE(vcqp, nullptr);
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sam->AddSystemAbility(IPC_VSYNCCALLBACK_SAID, vcqp);
        char buf[10] = "start";
        write(pipeFd[1], buf, sizeof(buf));
        sleep(0);

        read(pipeFd[0], buf, sizeof(buf));

        sam->RemoveSystemAbility(IPC_VSYNCCALLBACK_SAID);

        exit(0);
    } else {
        char buf[10];
        read(pipeFd[0], buf, sizeof(buf));
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj_ = sam->GetSystemAbility(IPC_VSYNCCALLBACK_SAID);
        vc_ = iface_cast<IVsyncCallback>(robj_);
    }
}

void VsyncCallbackTest::TearDownTestCase()
{
    robj_ = nullptr;
    vc_ = nullptr;

    char buf[10] = "over";
    write(pipeFd[1], buf, sizeof(buf));

    int32_t ret = 0;
    do {
        waitpid(pid_, nullptr, 0);
    } while (ret == -1 && errno == EINTR);
}

namespace {
HWTEST_F(VsyncCallbackTest, IsProxy, testing::ext::TestSize.Level0)
{
    ASSERT_TRUE(robj_->IsProxyObject());
}

HWTEST_F(VsyncCallbackTest, OnVsync1, testing::ext::TestSize.Level0)
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    int64_t timestamp = (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    ReturnValueTester::Set<bool>(0, false);
    auto ret = vc_->OnVsync(timestamp);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}

HWTEST_F(VsyncCallbackTest, OnVsync2, testing::ext::TestSize.Level0)
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    int64_t timestamp = (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    ReturnValueTester::Set<bool>(1, false);
    auto ret = vc_->OnVsync(timestamp);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}

HWTEST_F(VsyncCallbackTest, OnVsync3, testing::ext::TestSize.Level0)
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    int64_t timestamp = (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    ReturnValueTester::Set<int>(2, GSERROR_BINDER);
    auto ret = vc_->OnVsync(timestamp);
    ASSERT_EQ(ret, GSERROR_BINDER);
}

HWTEST_F(VsyncCallbackTest, OnVsync4, testing::ext::TestSize.Level0)
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    int64_t timestamp = (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    ReturnValueTester::Set<int>(3, GSERROR_API_FAILED);
    auto ret = vc_->OnVsync(timestamp);
    ASSERT_EQ(ret, GSERROR_API_FAILED);
}
} // namespace
GSError VsyncCallback::OnVsync(int64_t timestamp)
{
    (void)timestamp;
    return GSERROR_OK;
}
} // namespace Vsync
} // namespace OHOS
