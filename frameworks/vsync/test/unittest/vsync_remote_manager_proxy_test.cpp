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

#include "vsync_remote_manager_proxy_test.h"

#include <chrono>
#include <thread>
#include <unistd.h>

#include <iservice_registry.h>

#include "return_value_tester.h"

using namespace std::chrono_literals;

namespace OHOS {
namespace Vsync {
void VsyncManagerTest::SetUp()
{
}

void VsyncManagerTest::TearDown()
{
}

void VsyncManagerTest::SetUpTestCase()
{
    pipe(pipeFd);

    pid_ = fork();
    if (pid_ < 0) {
        exit(1);
    }

    if (pid_ == 0) {
        std::this_thread::sleep_for(50ms);
        sptr<VsyncManager> vcqp = new VsyncManager();
        ASSERT_NE(vcqp, nullptr);
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sam->AddSystemAbility(IPC_VSYNCMANAGER_SAID, vcqp);
        char buf[10] = "start";
        write(pipeFd[1], buf, sizeof(buf));
        sleep(0);

        read(pipeFd[0], buf, sizeof(buf));

        sam->RemoveSystemAbility(IPC_VSYNCMANAGER_SAID);

        exit(0);
    } else {
        char buf[10];
        read(pipeFd[0], buf, sizeof(buf));
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj_ = sam->GetSystemAbility(IPC_VSYNCMANAGER_SAID);
        vc_ = iface_cast<IVsyncManager>(robj_);
    }
}

void VsyncManagerTest::TearDownTestCase()
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
HWTEST_F(VsyncManagerTest, IsProxy, testing::ext::TestSize.Level0)
{
    ASSERT_TRUE(robj_->IsProxyObject());
}

HWTEST_F(VsyncManagerTest, ListenVsync1, testing::ext::TestSize.Level0)
{
    sptr<IVsyncCallback> cb = nullptr;
    auto ret = vc_ ->ListenVsync(cb);
    ASSERT_EQ(ret, VSYNC_ERROR_NULLPTR);
}

HWTEST_F(VsyncManagerTest, ListenVsync2, testing::ext::TestSize.Level0)
{
    sptr<IVsyncCallback> cb = new VsyncCallback();
    ReturnValueTester::Set<bool>(0, false);
    auto ret = vc_ ->ListenVsync(cb);
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_ARGUMENTS);
}

HWTEST_F(VsyncManagerTest, ListenVsync3, testing::ext::TestSize.Level0)
{
    sptr<IVsyncCallback> cb = new VsyncCallback();
    ReturnValueTester::Set<int>(1, VSYNC_ERROR_BINDER_ERROR);
    auto ret = vc_ ->ListenVsync(cb);
    ASSERT_EQ(ret, VSYNC_ERROR_BINDER_ERROR);
}

HWTEST_F(VsyncManagerTest, ListenVsync4, testing::ext::TestSize.Level0)
{
    sptr<IVsyncCallback> cb = new VsyncCallback();
    ReturnValueTester::Set<int>(2, VSYNC_ERROR_API_FAILED);
    auto ret = vc_ ->ListenVsync(cb);
    ASSERT_EQ(ret, VSYNC_ERROR_API_FAILED);
}

HWTEST_F(VsyncManagerTest, GetVsyncFrequency1, testing::ext::TestSize.Level0)
{
    ReturnValueTester::Set<bool>(0, false);
    uint32_t freq= 30;
    auto ret = vc_ ->GetVsyncFrequency(freq);
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_ARGUMENTS);
}

HWTEST_F(VsyncManagerTest, GetVsyncFrequency2, testing::ext::TestSize.Level0)
{
    ReturnValueTester::Set<int>(1, VSYNC_ERROR_BINDER_ERROR);
    uint32_t freq= 30;
    auto ret = vc_ ->GetVsyncFrequency(freq);
    ASSERT_EQ(ret, VSYNC_ERROR_BINDER_ERROR);
}

HWTEST_F(VsyncManagerTest, GetVsyncFrequency3, testing::ext::TestSize.Level0)
{
    ReturnValueTester::Set<int>(2, VSYNC_ERROR_API_FAILED);
    uint32_t freq= 30;
    auto ret = vc_ ->GetVsyncFrequency(freq);
    ASSERT_EQ(ret, VSYNC_ERROR_API_FAILED);
}
} // namespace
VsyncError VsyncCallback::OnVsync(int64_t timestamp)
{
    (void)timestamp;
    return VSYNC_ERROR_OK;
}
} // namespace Vsync
} // namespace OHOS
