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

#include <gtest/gtest.h>
#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_controller.h"
#include "event_runner.h"
#include "event_handler.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class VsyncReceiverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline void OnVSync(int64_t now, void* data);
    static inline void OnVSync2(int64_t now, void* data);

    static inline sptr<VSyncController> vsyncController = nullptr;
    static inline sptr<VSyncDistributor> vsyncDistributor = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator = nullptr;
    static inline sptr<VSyncConnection> conn = nullptr;
    static inline sptr<VSyncReceiver> vsyncReceiver = nullptr;
    static inline int32_t onVsyncCount = 0;
};

void VsyncReceiverTest::OnVSync(int64_t now, void* data)
{
    onVsyncCount ++;
}

void VsyncReceiverTest::OnVSync2(int64_t now, void* data)
{
    usleep(100000); // 100000us
}

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

void VsyncReceiverTest::SetUpTestCase()
{
    vsyncGenerator = CreateVSyncGenerator();
    vsyncController = new VSyncController(vsyncGenerator, 0);
    vsyncDistributor = new VSyncDistributor(vsyncController, "VsyncReceiverTest");
    conn = new VSyncConnection(vsyncDistributor, "VsyncReceiverTest");
    vsyncReceiver = new VSyncReceiver(conn);
}

void VsyncReceiverTest::TearDownTestCase()
{
    vsyncReceiver->looper_->RemoveFileDescriptorListener(vsyncReceiver->fd_);
    vsyncReceiver->looper_ = nullptr;
    vsyncReceiver->fd_ = -1;
    vsyncReceiver = nullptr;
    vsyncController = nullptr;
    vsyncGenerator = nullptr;
    vsyncDistributor = nullptr;
    conn = nullptr;
    DestroyVSyncGenerator();
}

namespace {
/*
* Function: BeforeInit001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync Before Init
                   2. call SetVSyncRate Before Init
                   3. call the one-parameter method of CreateVSyncReceiver
                   4. call GetVSyncPeriodAndLastTimeStamp Before Init of vSync receiver in 3.
                   5. call the two-parameter method of CreateVSyncReceiver
                   6. call GetVSyncPeriodAndLastTimeStamp Before Init of vSync receiver in 5.
                   7. call the four-parameter method of CreateVSyncReceiver
                   8. call GetVSyncPeriodAndLastTimeStamp Before Init of vSync receiver in 7.
 */
HWTEST_F(VsyncReceiverTest, BeforeInit001, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->RequestNextVSync(fcb), VSYNC_ERROR_NOT_INIT);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 0), VSYNC_ERROR_API_FAILED);

    int64_t period;
    int64_t timeStamp;
    auto& rsClient = RSInterfaces::GetInstance();
    auto rsReceiver = rsClient.CreateVSyncReceiver("VsyncReceiverTest");
    ASSERT_EQ(rsReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp), VSYNC_ERROR_NOT_INIT);
    rsReceiver = rsClient.CreateVSyncReceiver("VsyncReceiverTest", nullptr);
    ASSERT_EQ(rsReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp), VSYNC_ERROR_NOT_INIT);
    rsReceiver = rsClient.CreateVSyncReceiver("VsyncReceiverTest", 0, nullptr, 0);
    ASSERT_EQ(rsReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp), VSYNC_ERROR_NOT_INIT);
}

/*
* Function: RequestNextVSyncWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSyncWithMultiCallback before Init.
 */
HWTEST_F(VsyncReceiverTest, RequestNextVSyncWithMultiCallback001, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->RequestNextVSyncWithMultiCallback(fcb), VSYNC_ERROR_NOT_INIT);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: Init001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Init
 */
HWTEST_F(VsyncReceiverTest, Init001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->Init(), VSYNC_ERROR_OK);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->Init(), VSYNC_ERROR_OK);
}

/*
* Function: Init002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Init
 */
HWTEST_F(VsyncReceiverTest, Init002, Function | MediumTest| Level3)
{
    sptr<IVSyncConnection> connection_ = nullptr;
    ASSERT_NE(VsyncReceiverTest::vsyncReceiver->Init(), VSYNC_ERROR_NULLPTR);
}

/*
* Function: IsRequestedNextVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsRequestedNextVSync
 */
HWTEST_F(VsyncReceiverTest, IsRequestedNextVSync001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->IsRequestedNextVSync(), false);
}

/*
* Function: RequestNextVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VsyncReceiverTest, RequestNextVSync001, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->RequestNextVSync(fcb), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: RequestNextVSync002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VsyncReceiverTest, RequestNextVSync002, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->RequestNextVSync(fcb, "unknown", 0), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: RequestNextVSyncWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSyncWithMultiCallback after Init.
 */
HWTEST_F(VsyncReceiverTest, RequestNextVSyncWithMultiCallback002, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->RequestNextVSyncWithMultiCallback(fcb), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: IsRequestedNextVSync002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call IsRequestedNextVSync
 */
HWTEST_F(VsyncReceiverTest, IsRequestedNextVSync002, Function | MediumTest| Level3)
{
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->IsRequestedNextVSync(), true);
}

/*
* Function: GetVSyncPeriodAndLastTimeStamp001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateVSyncReceiver
*                  2. call GetVSyncPeriodAndLastTimeStamp and check result
 */
HWTEST_F(VsyncReceiverTest, GetVSyncPeriodAndLastTimeStamp001, Function | MediumTest| Level3)
{
    onVsyncCount = 0;
    auto& rsClient = RSInterfaces::GetInstance();
    auto rsReceiver = rsClient.CreateVSyncReceiver("VsyncReceiverTest");

    ASSERT_EQ(rsReceiver->Init(), VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };

    ASSERT_EQ(rsReceiver->RequestNextVSync(fcb), VSYNC_ERROR_OK);
    while (onVsyncCount == 0) {
        sleep(1);
        std::cout<< "OnVsync called count: " << onVsyncCount << std::endl;
    }

    int64_t period;
    int64_t timeStamp;
    ASSERT_EQ(rsReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp), VSYNC_ERROR_OK);
    ASSERT_EQ(rsReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp, true), VSYNC_ERROR_UNKOWN);
    ASSERT_NE(period, 0);
    ASSERT_NE(timeStamp, 0);
    rsReceiver->looper_->RemoveFileDescriptorListener(rsReceiver->fd_);
    rsReceiver->looper_ = nullptr;
    rsReceiver->fd_ = -1;
}

/*
 * @tc.name: SetVsyncCallBackForEveryFrame
 * @tc.desc: Test For SetVsyncCallBackForEveryFrame
 * @tc.type: FUNC
 * @tc.require: issueIC989U
 */
HWTEST_F(VsyncReceiverTest, SetVsyncCallBackForEveryFrame001, Function | MediumTest| Level3)
{
    onVsyncCount = 0;
    auto& rsClient = RSInterfaces::GetInstance();
    auto rsReceiver = rsClient.CreateVSyncReceiver("VsyncReceiverTest");

    ASSERT_EQ(rsReceiver->Init(), VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };

    ASSERT_EQ(rsReceiver->RequestNextVSync(fcb), VSYNC_ERROR_OK);
    while (onVsyncCount == 0) {
        sleep(1);
        std::cout<< "OnVsync called count: " << onVsyncCount << std::endl;
    }

    onVsyncCount = 0;
    ASSERT_EQ(rsReceiver->SetVsyncCallBackForEveryFrame(fcb, true), VSYNC_ERROR_OK);
    int64_t timeStart = SystemTime();
    int64_t period = 0;
    ASSERT_EQ(rsReceiver->GetVSyncPeriod(period), VSYNC_ERROR_OK);
    usleep(period / 10);
    ASSERT_EQ(rsReceiver->SetVsyncCallBackForEveryFrame(fcb, false), VSYNC_ERROR_OK);
    int64_t timeEnd = SystemTime();
    sleep(1);
    std::cout<< "OnVsync called count: " << onVsyncCount << " period: " << period << std::endl;
    bool res = abs(onVsyncCount - (timeEnd - timeStart) / period) <= 5 ? true : false;
    ASSERT_EQ(res, true);
    rsReceiver->looper_->RemoveFileDescriptorListener(rsReceiver->fd_);
    rsReceiver->looper_ = nullptr;
    rsReceiver->fd_ = -1;
}

/*
* Function: SetVSyncRate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VsyncReceiverTest, SetVSyncRate001, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 0), VSYNC_ERROR_INVALID_ARGUMENTS);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: SetVSyncRate002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VsyncReceiverTest, SetVSyncRate002, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 1), VSYNC_ERROR_OK);
    ASSERT_EQ(VsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, -1), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: SendDataFailedTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SendData Failed
 */
HWTEST_F(VsyncReceiverTest, SendDataFailedTest, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync2,
    };
    vsyncDistributor->AddConnection(conn);
    for (int i = 0; i < 10; i++) { // test 10 times
        ASSERT_EQ(vsyncReceiver->RequestNextVSync(fcb), VSYNC_ERROR_OK);
        usleep(10000); // 10000us
    }
    sleep(1); // 1s
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: SetUiDvsyncSwitchTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetUiDvsyncSwitch
 */
HWTEST_F(VsyncReceiverTest, SetUiDvsyncSwitchTest, Function | MediumTest| Level3)
{
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiver->SetUiDvsyncSwitch(true), VSYNC_ERROR_OK);
    ASSERT_EQ(vsyncReceiver->SetUiDvsyncSwitch(false), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: SetUiDvsyncConfigTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetUiDvsyncConfig
 */
HWTEST_F(VsyncReceiverTest, SetUiDvsyncConfigTest, Function | MediumTest| Level3)
{
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiver->SetUiDvsyncConfig(1), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: OnReadable001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test OnReadable
 */
HWTEST_F(VsyncReceiverTest, OnReadable001, Function | MediumTest| Level3)
{
    onVsyncCount = 0;
    auto& rsClient = RSInterfaces::GetInstance();
    auto rsReceiver = rsClient.CreateVSyncReceiver("VsyncReceiverTest");

    ASSERT_EQ(rsReceiver->Init(), VSYNC_ERROR_OK);
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };

    ASSERT_EQ(rsReceiver->RequestNextVSync(fcb), VSYNC_ERROR_OK);
    while (onVsyncCount == 0) {
        sleep(1);
        std::cout<< "OnVsync called count: " << onVsyncCount << std::endl;
    }

    onVsyncCount = 0;
    ASSERT_EQ(rsReceiver->SetVsyncCallBackForEveryFrame(fcb, true), VSYNC_ERROR_OK);
    int64_t period = 0;
    ASSERT_EQ(rsReceiver->GetVSyncPeriod(period), VSYNC_ERROR_OK);
    usleep(period / 10);
    ASSERT_EQ(rsReceiver->SetVsyncCallBackForEveryFrame(fcb, false), VSYNC_ERROR_OK);
    sleep(1);
    std::cout<< "OnVsync called count: " << onVsyncCount << " period: " << period << std::endl;
    ASSERT_EQ(abs(onVsyncCount - 100) <= 5, true);

    rsReceiver->listener_->OnReadable(-1);
    rsReceiver->listener_->OnReadable(999);
    rsReceiver->looper_->RemoveFileDescriptorListener(rsReceiver->fd_);
    rsReceiver->looper_ = nullptr;
    rsReceiver->fd_ = -1;
}
} // namespace
} // namespace Rosen
} // namespace OHOS