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
#include <chrono>
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include "vsync_receiver.h"
#include "vsync_controller.h"
#include "vsync_sampler.h"
#include "vsync_generator.h"
#include "vsync_distributor.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include <iostream>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
int32_t appVSyncFlag = 0;
constexpr int32_t SAMPLER_NUMBER = 6;
static void OnVSyncApp(int64_t time, void *data)
{
    std::cout << "OnVSyncApp in\n";
    appVSyncFlag = 1;
}
}
class VSyncTest : public testing::Test {
public:
    void Process1();
    void Process2();

    sptr<VSyncSampler> vsyncSampler = nullptr;
    sptr<VSyncGenerator> vsyncGenerator = nullptr;
    sptr<VSyncController> vsyncController = nullptr;
    sptr<VSyncDistributor> vsyncDistributor = nullptr;
    sptr<VSyncConnection> vsyncConnection = nullptr;

    static inline pid_t pid = 0;
    static inline int pipeFd[2] = {};
    static inline int pipe1Fd[2] = {};
    static inline int32_t systemAbilityID = 345135;
};

static void InitNativeTokenInfo()
{
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = "ohos.permission.DISTRIBUTED_DATASYNC";
    perms[1] = "ohos.permission.CAMERA";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "dcamera_client_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    int32_t ret = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    ASSERT_EQ(ret, Security::AccessToken::RET_SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // wait 50ms
}

void VSyncTest::Process1()
{
    InitNativeTokenInfo();
    vsyncGenerator = CreateVSyncGenerator();
    vsyncSampler = CreateVSyncSampler();
    int32_t count = 0;
    int64_t timestamp = 16666667; // 16666667ns
    while (count <= SAMPLER_NUMBER) {
        vsyncSampler->AddSample(timestamp);
        usleep(1000); // 1000us
        timestamp += 16666667; // 16666667ns
        count++;
    }
    vsyncController = new VSyncController(vsyncGenerator, 0);
    vsyncDistributor = new VSyncDistributor(vsyncController, "test");
    vsyncConnection = new VSyncConnection(vsyncDistributor, "test");
    vsyncDistributor->AddConnection(vsyncConnection);
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(systemAbilityID, vsyncConnection);
    close(pipeFd[1]);
    close(pipe1Fd[0]);
    char buf[10] = "start";
    write(pipe1Fd[1], buf, sizeof(buf));
    read(pipeFd[0], buf, sizeof(buf));
    sam->RemoveSystemAbility(systemAbilityID);
    close(pipeFd[0]);
    close(pipe1Fd[1]);
    exit(0);
}

void VSyncTest::Process2()
{
    close(pipeFd[0]);
    close(pipe1Fd[1]);
    char buf[10];
    read(pipe1Fd[0], buf, sizeof(buf));
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto robj = sam->GetSystemAbility(systemAbilityID);
    std::cout << "(robj==nullptr):" << (robj==nullptr) << std::endl;
    auto conn = iface_cast<IVSyncConnection>(robj);
    sptr<VSyncReceiver> receiver = new VSyncReceiver(conn);
    receiver->Init();
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = nullptr,
        .callback_ = OnVSyncApp,
    };
    std::cout << "RequestNextVSync\n";
    receiver->RequestNextVSync(fcb);
    sleep(1);
    std::cout << "ChildProcessMain appVSyncFlag is " << appVSyncFlag << std::endl;
    EXPECT_EQ(appVSyncFlag, 1);
    int64_t period;
    int64_t timeStamp;
    EXPECT_EQ(receiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp), VSYNC_ERROR_OK);
}

/*
* Function: RequestNextVSyncTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: RequestNextVSyncTest
 */
HWTEST_F(VSyncTest, RequestNextVSyncTest, Function | MediumTest | Level2)
{
    if (pipe(pipeFd) < 0) {
        exit(1);
    }
    if (pipe(pipe1Fd) < 0) {
        exit(0);
    }
    pid = fork();
    if (pid < 0) {
        exit(1);
    }
    if (pid != 0) {
        Process1();
    } else {
        Process2();
    }
}
}
