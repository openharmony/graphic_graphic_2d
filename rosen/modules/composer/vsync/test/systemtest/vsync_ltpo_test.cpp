/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "vsync_type.h"

#include <iostream>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t MAX_SIZE = 32;
typedef struct VSyncTimeStamps {
    int64_t appTimestamps[MAX_SIZE] = {0};
    int32_t appIndex = 0;
    int64_t rsTimestamps[MAX_SIZE] = {0};
    int32_t rsIndex = 0;
} VSyncTimeStamps;
VSyncTimeStamps g_timeStamps = {};
int32_t g_appVSyncFlag = 0;
int32_t g_rsVSyncFlag = 0;
constexpr int32_t SAMPLER_NUMBER = 6;
static void OnVSyncApp(int64_t time, void *data)
{
    g_appVSyncFlag = 1;
    g_timeStamps.appTimestamps[g_timeStamps.appIndex++] = time;
    g_timeStamps.appIndex %= MAX_SIZE;
}
static void OnVSyncRs(int64_t time, void *data)
{
    g_rsVSyncFlag = 1;
    g_timeStamps.rsTimestamps[g_timeStamps.rsIndex++] = time;
    g_timeStamps.rsIndex %= MAX_SIZE;
}
}
class VSyncLTPOTest : public testing::Test {
public:
    int32_t JudgeRefreshRate(int64_t period);
    void Process1();
    void Process2();

    sptr<VSyncSampler> vsyncSampler = nullptr;
    sptr<VSyncController> appController = nullptr;
    sptr<VSyncController> rsController = nullptr;
    sptr<VSyncDistributor> appDistributor = nullptr;
    sptr<VSyncDistributor> rsDistributor = nullptr;
    sptr<VSyncGenerator> vsyncGenerator = nullptr;
    sptr<VSyncReceiver> receiverApp = nullptr;
    sptr<VSyncReceiver> receiverRs = nullptr;

    static inline pid_t pid = 0;
    static inline int pipeFd[2] = {};
    static inline int pipe1Fd[2] = {};
    static inline int32_t ipcSystemAbilityIDApp = 34156;
    static inline int32_t ipcSystemAbilityIDRs = 34157;
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

int32_t VSyncLTPOTest::JudgeRefreshRate(int64_t period)
{
    if (period <= 0) {
        return 0;
    }
    int32_t actualRefreshRate = round(1.0 / (static_cast<double>(period) / 1000000000.0)); // 1.0s == 1000000000.0ns
    int32_t refreshRate = actualRefreshRate;
    int32_t diff = 0;
    while ((abs(refreshRate - actualRefreshRate) < 5) && // ±5Hz
           (VSYNC_MAX_REFRESHRATE % refreshRate != 0)) {
        if (diff < 0) {
            diff = -diff;
        } else {
            diff = -diff - 1;
        }
        refreshRate = actualRefreshRate + diff;
    }
    return refreshRate;
}

void VSyncLTPOTest::Process1()
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
    vsyncGenerator->SetVSyncMode(VSYNC_MODE_LTPO);
    std::cout << "pulse:" << vsyncGenerator->GetVSyncPulse() << std::endl;
    appController = new VSyncController(vsyncGenerator, 0);
    rsController = new VSyncController(vsyncGenerator, 0);
    appDistributor = new VSyncDistributor(appController, "app");
    rsDistributor = new VSyncDistributor(rsController, "rs");
    sptr<VSyncConnection> connServerApp = new VSyncConnection(appDistributor, "app", nullptr, 1); // id:1
    sptr<VSyncConnection> connServerRs = new VSyncConnection(rsDistributor, "rs", nullptr, 2); // id:2
    appDistributor->AddConnection(connServerApp);
    rsDistributor->AddConnection(connServerRs);
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityIDApp, connServerApp->AsObject());
    sam->AddSystemAbility(ipcSystemAbilityIDRs, connServerRs->AsObject());
    VSyncTimeStamps timeStamps = {};

    close(pipeFd[1]);
    close(pipe1Fd[0]);
    char buf[10] = "start";
    write(pipe1Fd[1], buf, sizeof(buf));

    // change refresh rate to 120hz
    int changeRefreshRate = 0;
    read(pipeFd[0], &changeRefreshRate, sizeof(changeRefreshRate));
    std::vector<std::pair<uint64_t, uint32_t>> refreshRates = {{1, 120}}; // 120hz
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {
        .cb = appController,
        .refreshRates = refreshRates
    };
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {
        .cb = appController,
        .phaseByPulseNum = 0
    };
    uint32_t generatorRefreshRate = 120; // 120hz
    vsyncGenerator->ChangeGeneratorRefreshRateModel(listenerRefreshRates, listenerPhaseOffset, generatorRefreshRate);
    // checkout 120hz
    read(pipeFd[0], &timeStamps, sizeof(timeStamps));
    int64_t appTimestampPrev = timeStamps.appTimestamps[(timeStamps.appIndex - 2) % MAX_SIZE]; // prev should minus 2
    int64_t appTimestampCur = timeStamps.appTimestamps[(timeStamps.appIndex - 1) % MAX_SIZE]; // cur should minus 1
    int64_t rsTimestampPrev = timeStamps.rsTimestamps[(timeStamps.rsIndex - 2) % MAX_SIZE]; // prev should minus 2
    int64_t rsTimestampCur = timeStamps.rsTimestamps[(timeStamps.rsIndex - 1) % MAX_SIZE]; // cur should minus 1
    int64_t appPeriod = appTimestampCur - appTimestampPrev;
    int64_t rsPeriod = rsTimestampCur - rsTimestampPrev;
    int32_t appRefreshRate = JudgeRefreshRate(appPeriod);
    int32_t rsRefreshRate = JudgeRefreshRate(rsPeriod);
    EXPECT_EQ(appRefreshRate, 120); // 120hz
    EXPECT_EQ(rsRefreshRate, 120); // 120hz
    if (appRefreshRate != 120) { // 120hz
        std::string appTimestamps = "appTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            appTimestamps += std::to_string(g_timeStamps.appTimestamps[i]) + ",";
        }
        appTimestamps += "]";
        std::cout << appTimestamps << std::endl;
    }
    if (rsRefreshRate != 120) { // 120hz
        std::string rsTimestamps = "rsTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            rsTimestamps += std::to_string(g_timeStamps.rsTimestamps[i]) + ",";
        }
        rsTimestamps += "]";
        std::cout << rsTimestamps << std::endl;
    }
    std::cout << "appPeriod:" << appPeriod <<
            ", appRefreshRate:" << appRefreshRate <<
            ", rsPeriod:" << rsPeriod <<
            ", rsRefreshRate:" << rsRefreshRate << std::endl;

    // change refresh rate to 90hz
    changeRefreshRate = 0;
    read(pipeFd[0], &changeRefreshRate, sizeof(changeRefreshRate));
    refreshRates = {{1, 90}}; // 90hz
    listenerRefreshRates = {
        .cb = appController,
        .refreshRates = refreshRates
    };
    listenerPhaseOffset = {
        .cb = appController,
        .phaseByPulseNum = 1 // phase is 1 pulse
    };
    generatorRefreshRate = 90; // 90hz
    vsyncGenerator->ChangeGeneratorRefreshRateModel(listenerRefreshRates, listenerPhaseOffset, generatorRefreshRate);
    // checkout 90hz
    read(pipeFd[0], &timeStamps, sizeof(timeStamps));
    appTimestampPrev = timeStamps.appTimestamps[(timeStamps.appIndex - 2) % MAX_SIZE]; // prev should minus 2
    appTimestampCur = timeStamps.appTimestamps[(timeStamps.appIndex - 1) % MAX_SIZE]; // cur should minus 1
    rsTimestampPrev = timeStamps.rsTimestamps[(timeStamps.rsIndex - 2) % MAX_SIZE]; // prev should minus 2
    rsTimestampCur = timeStamps.rsTimestamps[(timeStamps.rsIndex - 1) % MAX_SIZE]; // cur should minus 1
    appPeriod = appTimestampCur - appTimestampPrev;
    rsPeriod = rsTimestampCur - rsTimestampPrev;
    appRefreshRate = JudgeRefreshRate(appPeriod);
    rsRefreshRate = JudgeRefreshRate(rsPeriod);
    EXPECT_EQ(appRefreshRate, 90); // 90hz
    EXPECT_EQ(rsRefreshRate, 90); // 90hz
    if (appRefreshRate != 90) { // 90hz
        std::string appTimestamps = "appTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            appTimestamps += std::to_string(g_timeStamps.appTimestamps[i]) + ",";
        }
        appTimestamps += "]";
        std::cout << appTimestamps << std::endl;
    }
    if (rsRefreshRate != 90) { // 90hz
        std::string rsTimestamps = "rsTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            rsTimestamps += std::to_string(g_timeStamps.rsTimestamps[i]) + ",";
        }
        rsTimestamps += "]";
        std::cout << rsTimestamps << std::endl;
    }
    std::cout << "appPeriod:" << appPeriod <<
            ", appRefreshRate:" << appRefreshRate <<
            ", rsPeriod:" << rsPeriod <<
            ", rsRefreshRate:" << rsRefreshRate << std::endl;

    // change refresh rate to 60hz
    changeRefreshRate = 0;
    read(pipeFd[0], &changeRefreshRate, sizeof(changeRefreshRate));
    refreshRates = {{1, 60}}; // 60hz
    listenerRefreshRates = {
        .cb = appController,
        .refreshRates = refreshRates
    };
    listenerPhaseOffset = {
        .cb = appController,
        .phaseByPulseNum = 3 // phase is 3 pulse
    };
    generatorRefreshRate = 60; // 60hz
    vsyncGenerator->ChangeGeneratorRefreshRateModel(listenerRefreshRates, listenerPhaseOffset, generatorRefreshRate);
    // checkout 60hz
    read(pipeFd[0], &timeStamps, sizeof(timeStamps));
    appTimestampPrev = timeStamps.appTimestamps[(timeStamps.appIndex - 2) % MAX_SIZE]; // prev should minus 2
    appTimestampCur = timeStamps.appTimestamps[(timeStamps.appIndex - 1) % MAX_SIZE]; // cur should minus 1
    rsTimestampPrev = timeStamps.rsTimestamps[(timeStamps.rsIndex - 2) % MAX_SIZE]; // prev should minus 2
    rsTimestampCur = timeStamps.rsTimestamps[(timeStamps.rsIndex - 1) % MAX_SIZE]; // cur should minus 1
    appPeriod = appTimestampCur - appTimestampPrev;
    rsPeriod = rsTimestampCur - rsTimestampPrev;
    appRefreshRate = JudgeRefreshRate(appPeriod);
    rsRefreshRate = JudgeRefreshRate(rsPeriod);
    EXPECT_EQ(appRefreshRate, 60); // 60hz
    EXPECT_EQ(rsRefreshRate, 60); // 60hz
    if (appRefreshRate != 60) { // 60hz
        std::string appTimestamps = "appTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            appTimestamps += std::to_string(g_timeStamps.appTimestamps[i]) + ",";
        }
        appTimestamps += "]";
        std::cout << appTimestamps << std::endl;
    }
    if (rsRefreshRate != 60) { // 60hz
        std::string rsTimestamps = "rsTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            rsTimestamps += std::to_string(g_timeStamps.rsTimestamps[i]) + ",";
        }
        rsTimestamps += "]";
        std::cout << rsTimestamps << std::endl;
    }
    std::cout << "appPeriod:" << appPeriod <<
            ", appRefreshRate:" << appRefreshRate <<
            ", rsPeriod:" << rsPeriod <<
            ", rsRefreshRate:" << rsRefreshRate << std::endl;

    // change refresh rate to 30hz
    changeRefreshRate = 0;
    read(pipeFd[0], &changeRefreshRate, sizeof(changeRefreshRate));
    refreshRates = {{1, 30}}; // 30hz
    listenerRefreshRates = {
        .cb = appController,
        .refreshRates = refreshRates
    };
    listenerPhaseOffset = {
        .cb = appController,
        .phaseByPulseNum = 9 // phase is 9 pulse
    };
    generatorRefreshRate = 30; // 30hz
    vsyncGenerator->ChangeGeneratorRefreshRateModel(listenerRefreshRates, listenerPhaseOffset, generatorRefreshRate);
    // checkout 30hz
    read(pipeFd[0], &timeStamps, sizeof(timeStamps));
    appTimestampPrev = timeStamps.appTimestamps[(timeStamps.appIndex - 2) % MAX_SIZE]; // prev should minus 2
    appTimestampCur = timeStamps.appTimestamps[(timeStamps.appIndex - 1) % MAX_SIZE]; // cur should minus 1
    rsTimestampPrev = timeStamps.rsTimestamps[(timeStamps.rsIndex - 2) % MAX_SIZE]; // prev should minus 2
    rsTimestampCur = timeStamps.rsTimestamps[(timeStamps.rsIndex - 1) % MAX_SIZE]; // cur should minus 1
    appPeriod = appTimestampCur - appTimestampPrev;
    rsPeriod = rsTimestampCur - rsTimestampPrev;
    appRefreshRate = JudgeRefreshRate(appPeriod);
    rsRefreshRate = JudgeRefreshRate(rsPeriod);
    EXPECT_EQ(appRefreshRate, 30); // 30hz
    EXPECT_EQ(rsRefreshRate, 30); // 30hz
    if (appRefreshRate != 30) { // 30hz
        std::string appTimestamps = "appTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            appTimestamps += std::to_string(g_timeStamps.appTimestamps[i]) + ",";
        }
        appTimestamps += "]";
        std::cout << appTimestamps << std::endl;
    }
    if (rsRefreshRate != 30) { // 30hz
        std::string rsTimestamps = "rsTimestamps:[";
        for (int i = 0; i < 15; i++) { // check last 15 samples
            rsTimestamps += std::to_string(g_timeStamps.rsTimestamps[i]) + ",";
        }
        rsTimestamps += "]";
        std::cout << rsTimestamps << std::endl;
    }
    std::cout << "appPeriod:" << appPeriod <<
            ", appRefreshRate:" << appRefreshRate <<
            ", rsPeriod:" << rsPeriod <<
            ", rsRefreshRate:" << rsRefreshRate << std::endl;

    read(pipeFd[0], buf, sizeof(buf));
    sam->RemoveSystemAbility(ipcSystemAbilityIDApp);
    sam->RemoveSystemAbility(ipcSystemAbilityIDRs);
    close(pipeFd[0]);
    close(pipe1Fd[1]);
    exit(0);
}

void VSyncLTPOTest::Process2()
{
    close(pipeFd[0]);
    close(pipe1Fd[1]);
    char buf[10];
    read(pipe1Fd[0], buf, sizeof(buf));
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto robjApp = sam->GetSystemAbility(ipcSystemAbilityIDApp);
    auto robjRs = sam->GetSystemAbility(ipcSystemAbilityIDRs);
    std::cout << "(robjApp == nullptr):" << (robjApp == nullptr) << std::endl;
    std::cout << "(robjRs == nullptr):" << (robjRs == nullptr) << std::endl;
    auto connApp = iface_cast<IVSyncConnection>(robjApp);
    auto connRs = iface_cast<IVSyncConnection>(robjRs);
    receiverApp = new VSyncReceiver(connApp);
    receiverRs = new VSyncReceiver(connRs);
    receiverApp->Init();
    receiverRs->Init();
    VSyncReceiver::FrameCallback fcbApp = {
        .userData_ = nullptr,
        .callback_ = OnVSyncApp,
    };
    VSyncReceiver::FrameCallback fcbRs = {
        .userData_ = nullptr,
        .callback_ = OnVSyncRs,
    };

    // change refresh rate to 120hz
    int changeRefreshRate = 0;
    write(pipeFd[1], &changeRefreshRate, sizeof(changeRefreshRate));
    int num = 3; // RequestNextVSync 3 times
    while (num--) {
        receiverApp->RequestNextVSync(fcbApp);
        receiverRs->RequestNextVSync(fcbRs);
        while (g_appVSyncFlag == 0 || g_rsVSyncFlag == 0) {
            usleep(100); // 100us
        }
        g_appVSyncFlag = 0;
        g_rsVSyncFlag = 0;
    }
    write(pipeFd[1], &g_timeStamps, sizeof(g_timeStamps));

    // change refresh rate to 90hz
    changeRefreshRate = 0;
    write(pipeFd[1], &changeRefreshRate, sizeof(changeRefreshRate));
    num = 3; // RequestNextVSync 3 times
    while (num--) {
        receiverApp->RequestNextVSync(fcbApp);
        receiverRs->RequestNextVSync(fcbRs);
        while (g_appVSyncFlag == 0 || g_rsVSyncFlag == 0) {
            usleep(100); // 100us
        }
        g_appVSyncFlag = 0;
        g_rsVSyncFlag = 0;
    }
    write(pipeFd[1], &g_timeStamps, sizeof(g_timeStamps));

    // change refresh rate to 60hz
    changeRefreshRate = 0;
    write(pipeFd[1], &changeRefreshRate, sizeof(changeRefreshRate));
    num = 3; // RequestNextVSync 3 times
    while (num--) {
        receiverApp->RequestNextVSync(fcbApp);
        receiverRs->RequestNextVSync(fcbRs);
        while (g_appVSyncFlag == 0 || g_rsVSyncFlag == 0) {
            usleep(100); // 100us
        }
        g_appVSyncFlag = 0;
        g_rsVSyncFlag = 0;
    }
    write(pipeFd[1], &g_timeStamps, sizeof(g_timeStamps));

    // change refresh rate to 30hz
    changeRefreshRate = 0;
    write(pipeFd[1], &changeRefreshRate, sizeof(changeRefreshRate));
    num = 3; // RequestNextVSync 3 times
    while (num--) {
        receiverApp->RequestNextVSync(fcbApp);
        receiverRs->RequestNextVSync(fcbRs);
        while (g_appVSyncFlag == 0 || g_rsVSyncFlag == 0) {
            usleep(100); // 100us
        }
        g_appVSyncFlag = 0;
        g_rsVSyncFlag = 0;
    }
    write(pipeFd[1], &g_timeStamps, sizeof(g_timeStamps));
}

HWTEST_F(VSyncLTPOTest, ChangeRefreshRateTest, Function | MediumTest | Level2)
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
