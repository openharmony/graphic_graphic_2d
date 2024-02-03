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
class vsyncReceiverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline void OnVSync(int64_t now, void* data);

    static inline sptr<VSyncController> vsyncController = nullptr;
    static inline sptr<VSyncDistributor> vsyncDistributor = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator = nullptr;
    static inline sptr<VSyncConnection> conn = nullptr;
    static inline sptr<VSyncReceiver> vsyncReceiver = nullptr;
    static inline int32_t onVsyncCount = 0;
};

void vsyncReceiverTest::OnVSync(int64_t now, void* data)
{
    onVsyncCount ++;
}

void vsyncReceiverTest::SetUpTestCase()
{
    vsyncGenerator = CreateVSyncGenerator();
    vsyncController = new VSyncController(vsyncGenerator, 0);
    vsyncDistributor = new VSyncDistributor(vsyncController, "vsyncReceiverTest");
    conn = new VSyncConnection(vsyncDistributor, "vsyncReceiverTest");
    vsyncReceiver = new VSyncReceiver(conn);
}

void vsyncReceiverTest::TearDownTestCase()
{
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
                   3. call GetVSyncPeriodAndLastTimeStamp Before Init
 */
HWTEST_F(vsyncReceiverTest, BeforeInit001, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->RequestNextVSync(fcb), VSYNC_ERROR_API_FAILED);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 0), VSYNC_ERROR_API_FAILED);

    int64_t period;
    int64_t timeStamp;
    auto& rsClient = RSInterfaces::GetInstance();
    auto rsReceiver = rsClient.CreateVSyncReceiver("vsyncReceiverTest");
    ASSERT_EQ(rsReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp), VSYNC_ERROR_API_FAILED);
}

/*
* Function: Init001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Init
 */
HWTEST_F(vsyncReceiverTest, Init001, Function | MediumTest| Level3)
{
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->Init(), VSYNC_ERROR_OK);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->Init(), VSYNC_ERROR_OK);
}

/*
* Function: Init002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Init
 */
HWTEST_F(vsyncReceiverTest, Init002, Function | MediumTest| Level3)
{
    sptr<IVSyncConnection> connection_ = nullptr;
    ASSERT_NE(vsyncReceiverTest::vsyncReceiver->Init(), VSYNC_ERROR_NULLPTR);
}

/*
* Function: Init003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Init
 */
HWTEST_F(vsyncReceiverTest, Init003, Function | MediumTest| Level3)
{
    int fd_;
    sptr<IVSyncConnection> connection_ = nullptr;
    VsyncError ret = connection_->GetReceiveFd(fd_);
    ASSERT_NE(ret, VSYNC_ERROR_OK);
}

/*
* Function: RequestNextVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(vsyncReceiverTest, RequestNextVSync001, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->RequestNextVSync(fcb), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: RequestNextVSync002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(vsyncReceiverTest, RequestNextVSync002, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->RequestNextVSync(fcb, "unknown", 0), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: GetVSyncPeriodAndLastTimeStamp001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateVSyncReceiver
*                  2. call GetVSyncPeriodAndLastTimeStamp and check result
 */
HWTEST_F(vsyncReceiverTest, GetVSyncPeriodAndLastTimeStamp001, Function | MediumTest| Level3)
{
    onVsyncCount = 0;
    auto& rsClient = RSInterfaces::GetInstance();
    auto rsReceiver = rsClient.CreateVSyncReceiver("vsyncReceiverTest");

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
    ASSERT_EQ(rsReceiver->GetVSyncPeriodAndLastTimeStamp(period, timeStamp, true), VSYNC_ERROR_API_FAILED);
    ASSERT_NE(period, 0);
    ASSERT_NE(timeStamp, 0);
}

/*
* Function: SetVSyncRate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(vsyncReceiverTest, SetVSyncRate001, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 0), VSYNC_ERROR_INVALID_ARGUMENTS);
    vsyncDistributor->RemoveConnection(conn);
}

/*
* Function: SetVSyncRate002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(vsyncReceiverTest, SetVSyncRate002, Function | MediumTest| Level3)
{
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = OnVSync,
    };
    vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(vsyncReceiverTest::vsyncReceiver->SetVSyncRate(fcb, 1), VSYNC_ERROR_OK);
    vsyncDistributor->RemoveConnection(conn);
}
} // namespace
} // namespace Rosen
} // namespace OHOS