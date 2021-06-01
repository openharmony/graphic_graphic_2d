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

#include "vsync_manager_remote_test.h"

#include <sys/wait.h>
#include <unistd.h>

#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "vsync_callback_stub.h"
#include "vsync_log.h"
#include "vsync_manager.h"
#include "vsync_manager_proxy.h"

using namespace OHOS;

class GlobalData {
public:
    static GlobalData *GetInstance()
    {
        static GlobalData data;
        return &data;
    }

    pid_t pid_;
    int pipeFd_[2];
    sptr<IVsyncManager> manager_;
    sptr<IRemoteObject> robj;

private:
    GlobalData() = default;
    ~GlobalData() = default;
};

class VsyncManagerRemoteTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        pipe(GlobalData::GetInstance()->pipeFd_);
        GlobalData::GetInstance()->pid_ = fork();
        if (GlobalData::GetInstance()->pid_ < 0) {
            exit(1);
        }

        if (GlobalData::GetInstance()->pid_ == 0) {
            sptr<VsyncManager> remoteVsyncManager_ = new VsyncManager();
            auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            sam->AddSystemAbility(VSYNC_MANAGER_TEST_ID, remoteVsyncManager_);

            char buf[10] = {};
            write(GlobalData::GetInstance()->pipeFd_[1], buf, sizeof(buf));

            sleep(0);

            read(GlobalData::GetInstance()->pipeFd_[0], buf, sizeof(buf));

            sam->RemoveSystemAbility(VSYNC_MANAGER_TEST_ID);

            close(GlobalData::GetInstance()->pipeFd_[0]);
            close(GlobalData::GetInstance()->pipeFd_[1]);
            exit(0);
        }

        char buf[10];
        read(GlobalData::GetInstance()->pipeFd_[0], buf, sizeof(buf));

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        GlobalData::GetInstance()->robj = sam->GetSystemAbility(VSYNC_MANAGER_TEST_ID);
        GlobalData::GetInstance()->manager_ = iface_cast<IVsyncManager>(GlobalData::GetInstance()->robj);
    }

    static void TearDownTestCase()
    {
        GlobalData::GetInstance()->manager_ = nullptr;
        GlobalData::GetInstance()->robj = nullptr;

        char buf[10] = {};
        write(GlobalData::GetInstance()->pipeFd_[1], buf, sizeof(buf));

        waitpid(GlobalData::GetInstance()->pid_, nullptr, 0);

        close(GlobalData::GetInstance()->pipeFd_[0]);
        close(GlobalData::GetInstance()->pipeFd_[1]);
    }
};

class VsyncCallback : public VsyncCallbackStub {
public:
    void OnVsync(int64_t timestamp)
    {
        isCall_ = true;
    }

    bool IsCall() const
    {
        return isCall_;
    }

private:
    bool isCall_ = false;
};

namespace {
HWTEST_F(VsyncManagerRemoteTest, IsProxy, testing::ext::TestSize.Level0)
{
    ASSERT_NE(GlobalData::GetInstance()->manager_, nullptr);
    ASSERT_NE(GlobalData::GetInstance()->manager_->AsObject(), nullptr);
    ASSERT_TRUE(GlobalData::GetInstance()->manager_->AsObject()->IsProxyObject());
}

HWTEST_F(VsyncManagerRemoteTest, ListenNextVsync, testing::ext::TestSize.Level0)
{
    ASSERT_NE(GlobalData::GetInstance()->manager_, nullptr);

    sptr<IVsyncCallback> cbnullptr = nullptr;
    ASSERT_NE(GlobalData::GetInstance()->manager_->ListenNextVsync(cbnullptr), VSYNC_ERROR_OK);

    sptr<IVsyncCallback> cb = new VsyncCallback();
    ASSERT_EQ(GlobalData::GetInstance()->manager_->ListenNextVsync(cb), VSYNC_ERROR_OK);
}
} // namespace
