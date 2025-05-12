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

#include "vsync_generator.h"
#include "vsync_controller.h"
#include "vsync_distributor.h"
#include "vsync_receiver.h"
#include "vsync_iconnection_token.h"
#include <event_handler.h>

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

class VSyncGeneratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline void OnVSync(int64_t now, void* data) {}

    static inline sptr<VSyncGenerator> vsyncGenerator_;
    static constexpr const int32_t WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS = 5;
    static inline sptr<VSyncController> appController;
    static inline sptr<VSyncController> rsController;
    static inline sptr<VSyncDistributor> appDistributor;
    static inline sptr<VSyncDistributor> rsDistributor;
    static inline std::shared_ptr<VSyncReceiver> receiver;
    static inline std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    static inline std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};

void VSyncGeneratorTest::SetUpTestCase()
{
    vsyncGenerator_ = CreateVSyncGenerator();
    appController = new VSyncController(vsyncGenerator_, 0);
    rsController = new VSyncController(vsyncGenerator_, 0);
    appDistributor = new VSyncDistributor(appController, "app");
    rsDistributor = new VSyncDistributor(rsController, "rs");
    vsyncGenerator_->SetRSDistributor(rsDistributor);
    vsyncGenerator_->SetAppDistributor(appDistributor);
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<VSyncConnection> conn = new VSyncConnection(rsDistributor, "generator_test", token->AsObject());
    rsDistributor->AddConnection(conn);
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    receiver = std::make_shared<VSyncReceiver>(conn, token->AsObject(), handler_, "generator_test");
    receiver->Init();
}

void VSyncGeneratorTest::TearDownTestCase()
{
    sleep(WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS);
    vsyncGenerator_ = nullptr;
    DestroyVSyncGenerator();
    receiver->looper_->RemoveFileDescriptorListener(receiver->fd_);
    receiver->looper_ = nullptr;
    receiver->fd_ = -1;
}

class VSyncGeneratorTestCallback : public VSyncGenerator::Callback {
public:
    void OnVSyncEvent(int64_t now, int64_t period,
        uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate) override;
    void OnPhaseOffsetChanged(int64_t phaseOffset) override;
    void OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates) override;
};

void VSyncGeneratorTestCallback::OnVSyncEvent(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate)
{
}

void VSyncGeneratorTestCallback::OnPhaseOffsetChanged(int64_t phaseOffset)
{
}

void VSyncGeneratorTestCallback::OnConnsRefreshRateChanged(
    const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates)
{
}

namespace {
/*
* Function: CheckAndUpdateReferenceTimeTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest001, Function | MediumTest| Level0)
{
    ASSERT_EQ(vsyncGenerator_->CheckAndUpdateReferenceTime(-1, -1), VSYNC_ERROR_INVALID_ARGUMENTS);
    // 2000000000ns
    ASSERT_EQ(vsyncGenerator_->CheckAndUpdateReferenceTime(-1, 2000000000), VSYNC_ERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(vsyncGenerator_->CheckAndUpdateReferenceTime(8333333, -1), VSYNC_ERROR_INVALID_ARGUMENTS); // 8333333ns
    // 8333333ns, 2000000000ns
    ASSERT_EQ(vsyncGenerator_->CheckAndUpdateReferenceTime(8333333, 2000000000), VSYNC_ERROR_API_FAILED);
}

/*
* Function: CheckAndUpdateReferenceTimeTest002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest002, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(16666667, 1000000000); // 16666667ns, 1000000000ns
    ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000); // 8333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(8333333, 2000000000); // 8333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest003, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(8333333, 1000000000); // 8333333ns, 1000000000ns
    ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000); // 8333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 60; // 60hz
    int64_t rsVsyncCount = 0;
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(8333333, 2000000000); // 8333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest004, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(16666667, 1000000000); // 16666667ns, 1000000000ns
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000); // 8333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(8333333, 2000000000); // 8333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest005
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime and set pendingPeriod_ 8.3ms
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest005, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(8333333, 1000000000); // 8333333ns, 1000000000ns
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000); // 8333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(8333333, 2000000000); // 8333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest006
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime and set pendingPeriod_ 11.1ms
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest006, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(11111111, 1000000000); // 11111111ns, 1000000000ns
    int64_t refreshRate = 90; // 90hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(11111111, 0, 1000000000); // 11111111ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(11111111, 2000000000); // 11111111ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest007
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime and set pendingPeriod_ 16.6ms
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest007, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(16666667, 1000000000); // 16666667ns, 1000000000ns
    int64_t refreshRate = 60; // 60hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(16666667, 0, 1000000000); // 16666667ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(16666667, 2000000000); // 16666667ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest008
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime and set pendingPeriod_ 33.3ms
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest008, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(33333333, 1000000000); // 33333333ns, 1000000000ns
    int64_t refreshRate = 30; // 30hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(33333333, 0, 1000000000); // 33333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(33333333, 2000000000); // 33333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest009
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime and StartRefresh
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest009, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(33333333, 1000000000); // 33333333ns, 1000000000ns
    int64_t refreshRate = 30; // 30hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(33333333, 0, 1000000000); // 33333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->StartRefresh();
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(33333333, 2000000000); // 33333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest010
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime and SetVSyncPhaseByPulseNum
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest010, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(33333333, 1000000000); // 33333333ns, 1000000000ns
    int64_t refreshRate = 30; // 30hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(33333333, 0, 1000000000); // 33333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->SetReferenceTimeOffset(1);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->SetVSyncPhaseByPulseNum(1);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(33333333, 2000000000); // 33333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: CheckAndUpdateReferenceTimeTest011
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test CheckAndUpdateReferenceTime and StartRefresh, SetVSyncPhaseByPulseNum
 */
HWTEST_F(VSyncGeneratorTest, CheckAndUpdateReferenceTimeTest011, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    vsyncGenerator_->SetPendingMode(33333333, 1000000000); // 33333333ns, 1000000000ns
    int64_t refreshRate = 30; // 30hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(33333333, 0, 1000000000); // 33333333ns, 1000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->SetReferenceTimeOffset(1);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->SetVSyncPhaseByPulseNum(1);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->StartRefresh();
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->CheckAndUpdateReferenceTime(33333333, 2000000000); // 33333333ns, 2000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: PeriodCheckLockedTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test PeriodCheckLocked
 */
HWTEST_F(VSyncGeneratorTest, PeriodCheckLockedTest, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    for (int i = 0; i < 20; i++) { // test 20 times
        int64_t testPeriod = i < 2 ? 8333333 : 16666667; // 2, 8333333ns, 16666667ns
        vsyncGenerator_->SetPendingMode(testPeriod, 1000000000); // 1000000000ns
        int64_t refreshRate = 120; // 120hz
        int64_t rsVsyncCount = 0;
        VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
        VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
        ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
            listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        VSyncReceiver::FrameCallback fcb = {
            .userData_ = this,
            .callback_ = OnVSync,
        };
        ret = receiver->RequestNextVSync(fcb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        usleep(100000); // 100000us
        ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000); // 8333333ns, 1000000000ns
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        ret = vsyncGenerator_->CheckAndUpdateReferenceTime(testPeriod, 2000000000); // 2000000000ns
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
    }
}

/*
* Function: NowLessThanReferenceTimeTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test now is less than referenceTime
 */
HWTEST_F(VSyncGeneratorTest, NowLessThanReferenceTimeTest, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPS);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ASSERT_EQ(vsyncGenerator_->GetVSyncMode(), VSYNC_MODE_LTPS);
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_NOT_SUPPORT);
    ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000000000); // 8333333ns, 1000000000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
}

/*
* Function: ChangeGeneratorRefreshRateModelTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test ChangeGeneratorRefreshRateModel for LTPO
 */
HWTEST_F(VSyncGeneratorTest, ChangeGeneratorRefreshRateModelTest001, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    std::vector<std::pair<uint64_t, uint32_t>> refreshRates = {};
    refreshRates.push_back({0, 60});
    refreshRates.push_back({1, 120});
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {
        .cb = appController,
        .refreshRates = refreshRates,
    };
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {
        .cb = rsController,
        .phaseByPulseNum = 3,
    };
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000000); // 8333333ns, 1000000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ASSERT_EQ(vsyncGenerator_->GetVSyncMode(), VSYNC_MODE_LTPO);
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: ChangeGeneratorRefreshRateModelTest002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test ChangeGeneratorRefreshRateModel for LTPS
 */
HWTEST_F(VSyncGeneratorTest, ChangeGeneratorRefreshRateModelTest002, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPS);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ASSERT_EQ(vsyncGenerator_->GetVSyncMode(), VSYNC_MODE_LTPS);
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    std::vector<std::pair<uint64_t, uint32_t>> refreshRates = {};
    refreshRates.push_back({0, 60});
    refreshRates.push_back({1, 120});
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {
        .cb = appController,
        .refreshRates = refreshRates,
    };
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {
        .cb = rsController,
        .phaseByPulseNum = 3,
    };
    ret = vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_NOT_SUPPORT);
    ret = vsyncGenerator_->UpdateMode(8333333, 0, 1000000000000); // 8333333ns, 1000000000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: SetVSyncModeTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test SetVSyncMode and GetVSyncMode
 */
HWTEST_F(VSyncGeneratorTest, SetVSyncModeTest, Function | MediumTest| Level0)
{
    VsyncError ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPS);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ASSERT_EQ(vsyncGenerator_->GetVSyncMode(), VSYNC_MODE_LTPS);
    ret = vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: SetVSyncMaxRefreshRateTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test SetVSyncMaxRefreshRateTest and GetVSyncMaxRefreshRateTest
 */
HWTEST_F(VSyncGeneratorTest, SetVSyncMaxRefreshRateTest, Function | MediumTest| Level0)
{
    // set invalid value 30 smaller than VSYNC_MAX_REFRESHRATE_RANGE_MIN
    VsyncError ret = vsyncGenerator_->SetVSyncMaxRefreshRate(30);
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_ARGUMENTS);

    // set invalid value 600 greater than VSYNC_MAX_REFRESHRATE_RANGE_MAX
    ret = vsyncGenerator_->SetVSyncMaxRefreshRate(600);
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_ARGUMENTS);

    // set valid value
    ret = vsyncGenerator_->SetVSyncMaxRefreshRate(240);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    uint32_t vsyncMaxRefreshRate = vsyncGenerator_->GetVSyncMaxRefreshRate();
    ASSERT_EQ(vsyncMaxRefreshRate, 240);
}

/*
* Function: SetHighPriorityVSyncRateTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Test SetHighPriorityVSyncRate
 */
HWTEST_F(VSyncGeneratorTest, SetHighPriorityVSyncRateTest, Function | MediumTest| Level0)
{
    sptr<VSyncConnection> conn = new VSyncConnection(rsDistributor, "SetHighPriorityVSyncRateTest");
    rsDistributor->AddConnection(conn);
    VsyncError ret = rsDistributor->SetHighPriorityVSyncRate(2, conn); // rate is 2
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    for (int i = 0; i < 10; i++) {
        ret = receiver->RequestNextVSync(fcb);
        ASSERT_EQ(ret, VSYNC_ERROR_OK);
        usleep(10000); // 10000us
    }
    usleep(100000); // 100000us
}

/*
* Function: UpdateMode001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateMode
 */
HWTEST_F(VSyncGeneratorTest, UpdateMode001, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(2, 0, 0), VSYNC_ERROR_OK);
}

/*
* Function: UpdateMode002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateMode
 */
HWTEST_F(VSyncGeneratorTest, UpdateMode002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(2, 0, -1), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: UpdateMode003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateMode
 */
HWTEST_F(VSyncGeneratorTest, UpdateMode003, Function | MediumTest| Level0)
{
    VSyncGeneratorTest::vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(0, 0, 0), VSYNC_ERROR_OK);
    // 25000000 is period, refreshRate is 40hz，for JudgeRefreshRateLocked test
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->UpdateMode(25000000, 0, 0), VSYNC_ERROR_OK);
}

/*
* Function: AddListener001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddListener
 */
HWTEST_F(VSyncGeneratorTest, AddListener001, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback1 = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback1), VSYNC_ERROR_OK);
}

/*
* Function: AddListener002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddListener
 */
HWTEST_F(VSyncGeneratorTest, AddListener002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(2, nullptr), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: AddListener003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddListener
 */
HWTEST_F(VSyncGeneratorTest, AddListener003, Function | MediumTest| Level0)
{
    VSyncGeneratorTest::vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    VSyncGeneratorTest::vsyncGenerator_->UpdateMode(2, 0, 0);
    sptr<VSyncGeneratorTestCallback> callback = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback), VSYNC_ERROR_OK);
}

/*
* Function: AddListener004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddListener
 */
HWTEST_F(VSyncGeneratorTest, AddListener004, Function | MediumTest| Level0)
{
    auto generatorImpl = static_cast<impl::VSyncGenerator*>(VSyncGeneratorTest::vsyncGenerator_.GetRefPtr());
    generatorImpl->listeners_.clear();
    sptr<VSyncGeneratorTestCallback> callback = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(0, callback), VSYNC_ERROR_OK);
    ASSERT_EQ(generatorImpl->listeners_.size(), 1);
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->AddListener(0, callback), VSYNC_ERROR_OK);
    ASSERT_EQ(generatorImpl->listeners_.size(), 1);
}

/*
* Function: RemoveListener001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveListener
 */
HWTEST_F(VSyncGeneratorTest, RemoveListener001, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback2 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback2);
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->RemoveListener(callback2), VSYNC_ERROR_OK);
}

/*
* Function: RemoveListener002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveListener
 */
HWTEST_F(VSyncGeneratorTest, RemoveListener002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->RemoveListener(nullptr), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RemoveListener003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveListener
 */
HWTEST_F(VSyncGeneratorTest, RemoveListener003, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback3 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback3);
    sptr<VSyncGeneratorTestCallback> callback4 = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->RemoveListener(callback4), VSYNC_ERROR_OK);
}

/*
* Function: ChangePhaseOffset001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ChangePhaseOffset
 */
HWTEST_F(VSyncGeneratorTest, ChangePhaseOffset001, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback5 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback5);
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->ChangePhaseOffset(callback5, 1), VSYNC_ERROR_OK);
}

/*
* Function: ChangePhaseOffset002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ChangePhaseOffset
 */
HWTEST_F(VSyncGeneratorTest, ChangePhaseOffset002, Function | MediumTest| Level0)
{
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->ChangePhaseOffset(nullptr, 1), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: ChangePhaseOffset003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ChangePhaseOffset
 */
HWTEST_F(VSyncGeneratorTest, ChangePhaseOffset003, Function | MediumTest| Level0)
{
    sptr<VSyncGeneratorTestCallback> callback6 = new VSyncGeneratorTestCallback;
    VSyncGeneratorTest::vsyncGenerator_->AddListener(2, callback6);
    sptr<VSyncGeneratorTestCallback> callback7 = new VSyncGeneratorTestCallback;
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->ChangePhaseOffset(callback7, 1), VSYNC_ERROR_INVALID_OPERATING);
}

/*
* Function: expectNextVsyncTimeTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime 0
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest001, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    auto ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0); // expectNextVsyncTime 0
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: expectNextVsyncTimeTest002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime -1
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest002, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    auto ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, -1); // expectNextVsyncTime -1
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: expectNextVsyncTimeTest003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime earlier than referenceTime.
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest003, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    VsyncError ret = vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    int64_t refreshRate = 120; // 120hz
    int64_t rsVsyncCount = 0;
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 0);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ret = receiver->RequestNextVSync(fcb);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    usleep(100000); // 100000us
    ret = vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
    ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, 10000000); // 10ms == 10000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: expectNextVsyncTimeTest004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime current system time.
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest004, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t now = SystemTime();
    int64_t rsVsyncCount = 0;
    auto ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, now);
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: expectNextVsyncTimeTest005
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime current system time plus 5ms.
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest005, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t now = SystemTime();
    int64_t rsVsyncCount = 0;
    auto ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, now + 5000000); // 5ms == 5000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: expectNextVsyncTimeTest006
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime current system time plus 5.5ms.
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest006, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t now = SystemTime();
    int64_t rsVsyncCount = 0;
    auto ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, now + 5500000); // 5.5ms == 5500000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: expectNextVsyncTimeTest007
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime current system time plus 90ms.
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest007, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t now = SystemTime();
    int64_t rsVsyncCount = 0;
    auto ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, now + 90000000); // 90ms == 90000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

/*
* Function: expectNextVsyncTimeTest008
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test expectNextVsyncTime current system time plus 110ms.
 */
HWTEST_F(VSyncGeneratorTest, expectNextVsyncTimeTest008, Function | MediumTest| Level0)
{
    int64_t period = 8333333; // 8333333ns
    int64_t referenceTime = SystemTime();
    vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    vsyncGenerator_->UpdateMode(period, 0, referenceTime);
    VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
    int64_t refreshRate = 120; // 120hz
    int64_t now = SystemTime();
    int64_t rsVsyncCount = 0;
    auto ret = VSyncGeneratorTest::vsyncGenerator_->ChangeGeneratorRefreshRateModel(
        listenerRefreshRates, listenerPhaseOffset, refreshRate, rsVsyncCount, now + 110000000); // 110ms == 110000000ns
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetCurrentRefreshRateTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test SetCurrentRefreshRate
 */
HWTEST_F(VSyncGeneratorTest, SetCurrentRefreshRateTest001, Function | MediumTest| Level0)
{
    uint32_t currRefreshRate = 60;
    uint32_t lastRefreshRate = 30;
    int64_t delayTime = VSyncGeneratorTest::vsyncGenerator_->SetCurrentRefreshRate(currRefreshRate, lastRefreshRate);
    ASSERT_EQ(delayTime, 0);
}

/*
* Function: DVSyncRateChangedTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test DVSyncRateChanged
 */
HWTEST_F(VSyncGeneratorTest, DVSyncRateChangedTest001, Function | MediumTest| Level0)
{
    uint32_t currRefreshRate = 60;
    bool frameRateChanged = false;
    VSyncGeneratorTest::vsyncGenerator_->DVSyncRateChanged(currRefreshRate, frameRateChanged);
    ASSERT_EQ(frameRateChanged, false);
}

/*
* Function: AddDVSyncListenerTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test AddDVSyncListener
 */
HWTEST_F(VSyncGeneratorTest, AddDVSyncListenerTest001, Function | MediumTest| Level0)
{
    sptr<VSyncGenerator::Callback> cb = nullptr;
    int64_t phase = 0;
    VsyncError error = VSyncGeneratorTest::vsyncGenerator_->AddDVSyncListener(phase, cb);
    ASSERT_EQ(error, VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: ComputeDVSyncListenerNextVSyncTimeStampTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ComputeDVSyncListenerNextVSyncTimeStamp
 */
HWTEST_F(VSyncGeneratorTest, ComputeDVSyncListenerNextVSyncTimeStampTest001, Function | MediumTest| Level0)
{
    impl::VSyncGenerator::Listener listener;
    listener.lastTime_ = SystemTime();
    listener.phase_ = 0;
    listener.callback_ = nullptr;
 
    auto vsyncGeneratorImpl = static_cast<impl::VSyncGenerator*>(VSyncGeneratorTest::vsyncGenerator_.GetRefPtr());
    auto ret = vsyncGeneratorImpl->ComputeDVSyncListenerNextVSyncTimeStamp(listener, 0, 0, 0);
    ASSERT_EQ(ret, INT64_MAX);
}
 
/*
* Function: ComputeDVSyncListenerNextVSyncTimeStampTest002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ComputeDVSyncListenerNextVSyncTimeStamp
 */
HWTEST_F(VSyncGeneratorTest, ComputeDVSyncListenerNextVSyncTimeStampTest002, Function | MediumTest| Level0)
{
    impl::VSyncGenerator::Listener listener;
    listener.lastTime_ = SystemTime();
    listener.phase_ = 0;
    listener.callback_ = nullptr;
    int64_t period = 8333333; // 8333333ns (period of 120Hz)
    int64_t now = period * 5 + listener.lastTime_;
    int64_t referenceTime = period + listener.lastTime_;
    auto vsyncGeneratorImpl = static_cast<impl::VSyncGenerator*>(VSyncGeneratorTest::vsyncGenerator_.GetRefPtr());
    vsyncGeneratorImpl->phaseRecord_ = 0;
    vsyncGeneratorImpl->wakeupDelay_ = 0;
 
    auto ret = vsyncGeneratorImpl->ComputeDVSyncListenerNextVSyncTimeStamp(listener, now, referenceTime, period);
    ASSERT_EQ(ret, 6 * period + listener.lastTime_);
}
 
/*
* Function: ComputeDVSyncListenerNextVSyncTimeStampTest003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ComputeDVSyncListenerNextVSyncTimeStamp
 */
HWTEST_F(VSyncGeneratorTest, ComputeDVSyncListenerNextVSyncTimeStampTest003, Function | MediumTest| Level0)
{
    impl::VSyncGenerator::Listener listener;
    listener.lastTime_ = SystemTime();
    listener.phase_ = 0;
    listener.callback_ = nullptr;
    int64_t period = 8333333; // 8333333ns (period of 120Hz)
    int64_t now = listener.lastTime_;
    int64_t referenceTime = period + listener.lastTime_;
    auto vsyncGeneratorImpl = static_cast<impl::VSyncGenerator*>(VSyncGeneratorTest::vsyncGenerator_.GetRefPtr());
    vsyncGeneratorImpl->phaseRecord_ = 0;
    vsyncGeneratorImpl->wakeupDelay_ = 0;
    vsyncGeneratorImpl->vsyncMode_ = VSYNC_MODE_LTPO;
    vsyncGeneratorImpl->refreshRateIsChanged_ = true;
 
    auto ret = vsyncGeneratorImpl->ComputeDVSyncListenerNextVSyncTimeStamp(listener, now, referenceTime, period);
    ASSERT_EQ(ret, referenceTime);
}
 
/*
* Function: ComputeDVSyncListenerNextVSyncTimeStampTest004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ComputeDVSyncListenerNextVSyncTimeStamp
 */
HWTEST_F(VSyncGeneratorTest, ComputeDVSyncListenerNextVSyncTimeStampTest004, Function | MediumTest| Level0)
{
    impl::VSyncGenerator::Listener listener;
    listener.lastTime_ = SystemTime();
    listener.phase_ = 0;
    listener.callback_ = nullptr;
    int64_t period = 8333333; // 8333333ns (period of 120Hz)
    int64_t now = listener.lastTime_;
    int64_t referenceTime = period + listener.lastTime_;
    auto vsyncGeneratorImpl = static_cast<impl::VSyncGenerator*>(VSyncGeneratorTest::vsyncGenerator_.GetRefPtr());
    vsyncGeneratorImpl->phaseRecord_ = 0;
    vsyncGeneratorImpl->wakeupDelay_ = 0;
    vsyncGeneratorImpl->vsyncMode_ = VSYNC_MODE_LTPS;
 
    auto ret = vsyncGeneratorImpl->ComputeDVSyncListenerNextVSyncTimeStamp(listener, now, referenceTime, period);
    ASSERT_EQ(ret, referenceTime);
}

/*
* Function: RemoveDVSyncListenerTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test RemoveDVSyncListener
 */
HWTEST_F(VSyncGeneratorTest, RemoveDVSyncListenerTest001, Function | MediumTest| Level0)
{
    sptr<VSyncGenerator::Callback> cb = nullptr;
    VsyncError error = VSyncGeneratorTest::vsyncGenerator_->RemoveDVSyncListener(cb);
    ASSERT_EQ(error, VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: CheckSampleIsAdaptiveTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test CheckSampleIsAdaptive
 */
HWTEST_F(VSyncGeneratorTest, CheckSampleIsAdaptiveTest001, Function | MediumTest| Level0)
{
    // 20000000ns
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->CheckSampleIsAdaptive(20000000), true);
    // 16500000ns
    ASSERT_EQ(VSyncGeneratorTest::vsyncGenerator_->CheckSampleIsAdaptive(16500000), false);
}

/*
* Function: IsUiDvsyncOnTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test IsUiDvsyncOnTest001
 */
HWTEST_F(VSyncGeneratorTest, IsUiDvsyncOnTest001, Function | MediumTest| Level0)
{
    auto vsyncGeneratorImpl = static_cast<impl::VSyncGenerator*>(VSyncGeneratorTest::vsyncGenerator_.GetRefPtr());
    auto rsVSyncDistributor = vsyncGeneratorImpl->rsVSyncDistributor_;
    vsyncGeneratorImpl->rsVSyncDistributor_ = nullptr;
    bool ret = vsyncGeneratorImpl->IsUiDvsyncOn();
    vsyncGeneratorImpl->rsVSyncDistributor_ = rsVSyncDistributor;
    ASSERT_EQ(ret, false);
}
} // namespace
} // namespace Rosen
} // namespace OHOS