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
#include "vsync_distributor.h"
#include "vsync_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class VSyncDistributorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<VSyncController> vsyncController = nullptr;
    static inline sptr<VSyncDistributor> vsyncDistributor = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator = nullptr;
    static inline sptr<VSyncConnection> vsyncConnection = nullptr;
};

void VSyncDistributorTest::SetUpTestCase()
{
    vsyncGenerator = CreateVSyncGenerator();
    vsyncController = new VSyncController(vsyncGenerator, 0);
    vsyncDistributor = new VSyncDistributor(vsyncController, "VSyncConnection");
    vsyncConnection = new VSyncConnection(vsyncDistributor, "VSyncConnection");
}

void VSyncDistributorTest::TearDownTestCase()
{
    vsyncGenerator = nullptr;
    DestroyVSyncGenerator();
    vsyncController = nullptr;
    vsyncDistributor = nullptr;
    vsyncConnection = nullptr;
}

namespace {
/*
* Function: AddConnection001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddConnection
 */
HWTEST_F(VSyncDistributorTest, AddConnection001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->AddConnection(nullptr), VSYNC_ERROR_NULLPTR);
}

/*
* Function: AddConnection002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddConnection
 */
HWTEST_F(VSyncDistributorTest, AddConnection002, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->AddConnection(conn), VSYNC_ERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: RemoveConnection001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveConnection
 */
HWTEST_F(VSyncDistributorTest, RemoveConnection001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(nullptr), VSYNC_ERROR_NULLPTR);
}

/*
* Function: RemoveConnection002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveConnection
 */
HWTEST_F(VSyncDistributorTest, RemoveConnection002, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RemoveConnection003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveConnection
 */
HWTEST_F(VSyncDistributorTest, RemoveConnection003, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    VSyncDistributorTest::vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: RequestNextVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncDistributorTest, RequestNextVSync001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RequestNextVSync(nullptr), VSYNC_ERROR_NULLPTR);
}

/*
* Function: RequestNextVSync002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncDistributorTest, RequestNextVSync002, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RequestNextVSync(conn), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RequestNextVSync003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncDistributorTest, RequestNextVSync003, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    VSyncDistributorTest::vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RequestNextVSync(conn), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: RequestNextVSync004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncDistributorTest, RequestNextVSync004, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    VSyncDistributorTest::vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RequestNextVSync(conn, "unknown", 0), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: RequestNextVSync005
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new VSyncConnection(nullptr, "VSyncDistributorTest")
*                  2. call RequestNextVSync
*                  3. return VSYNC_ERROR_NULLPTR
 */
HWTEST_F(VSyncDistributorTest, RequestNextVSync005, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(nullptr, "VSyncDistributorTest");
    ASSERT_EQ(conn->RequestNextVSync("unknown", 0), VSYNC_ERROR_NULLPTR);
}

/*
* Function: RequestNextVSync006
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncDistributorTest, RequestNextVSync006, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    VSyncDistributorTest::vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RequestNextVSync(conn, "UrgentSelfdrawing", 0), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: SetVSyncRate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VSyncDistributorTest, SetVSyncRate001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetVSyncRate(0, nullptr), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetVSyncRate002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VSyncDistributorTest, SetVSyncRate002, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetVSyncRate(1, conn), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetVSyncRate003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VSyncDistributorTest, SetVSyncRate003, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    VSyncDistributorTest::vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetVSyncRate(1, conn), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: SetVSyncRate004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VSyncDistributorTest, SetVSyncRate004, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    VSyncDistributorTest::vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetVSyncRate(1, conn), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetVSyncRate(1, conn), VSYNC_ERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: SetVSyncRate005
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. set conn.distributor_ = nullptr
*                  2. call SetVSyncRate
*                  3. return VSYNC_ERROR_NULLPTR
 */
HWTEST_F(VSyncDistributorTest, SetVSyncRate005, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(nullptr, "VSyncDistributorTest");
    ASSERT_EQ(conn->SetVSyncRate(1), VSYNC_ERROR_NULLPTR);
}

/*
* Function: SetHighPriorityVSyncRate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetHighPriorityVSyncRate with abnormal parameters and check ret
 */
HWTEST_F(VSyncDistributorTest, SetHighPriorityVSyncRate001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetHighPriorityVSyncRate(0, nullptr),
              VSYNC_ERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetHighPriorityVSyncRate(1, nullptr),
              VSYNC_ERROR_INVALID_ARGUMENTS);
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetHighPriorityVSyncRate(1, conn), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetHighPriorityVSyncRate002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetHighPriorityVSyncRate with normal parameters and check ret
 */
HWTEST_F(VSyncDistributorTest, SetHighPriorityVSyncRate002, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    VSyncDistributorTest::vsyncDistributor->AddConnection(conn);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetHighPriorityVSyncRate(1, conn), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetHighPriorityVSyncRate(1, conn), VSYNC_ERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetHighPriorityVSyncRate(2, conn), VSYNC_ERROR_OK);
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
* Function: SetFrameIsRender001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetFrameIsRender with render is true
 */
HWTEST_F(VSyncDistributorTest, SetFrameIsRender001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->IsDVsyncOn(), false);
    VSyncDistributorTest::vsyncDistributor->SetFrameIsRender(true);
}

/*
* Function: SetFrameIsRender002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetFrameIsRender with render is false
 */
HWTEST_F(VSyncDistributorTest, SetFrameIsRender002, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->IsDVsyncOn(), false);
    VSyncDistributorTest::vsyncDistributor->SetFrameIsRender(false);
}

/*
* Function: GetRealTimeOffsetOfDvsync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetRealTimeOffsetOfDvsync
 */
HWTEST_F(VSyncDistributorTest, GetRealTimeOffsetOfDvsync001, Function | MediumTest| Level3)
{
    int64_t time = 1000;
    uint64_t offset = VSyncDistributorTest::vsyncDistributor->GetRealTimeOffsetOfDvsync(time);
    ASSERT_EQ(offset, 0);
}

/*
* Function: MarkRSAnimate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call MarkRSAnimate
 */
HWTEST_F(VSyncDistributorTest, MarkRSAnimate001, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    auto res = VSyncDistributorTest::vsyncDistributor->SetUiDvsyncSwitch(true, conn);
    ASSERT_EQ(res, VSYNC_ERROR_OK);
    VSyncDistributorTest::vsyncDistributor->MarkRSAnimate();
}

/*
* Function: UnmarkRSAnimate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UnmarkRSAnimate
 */
HWTEST_F(VSyncDistributorTest, UnmarkRSAnimate001, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    auto res = VSyncDistributorTest::vsyncDistributor->SetUiDvsyncSwitch(false, conn);
    ASSERT_EQ(res, VSYNC_ERROR_OK);
    VSyncDistributorTest::vsyncDistributor->UnmarkRSAnimate();
}

/*
* Function: HasPendingUIRNV001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call HasPendingUIRNV
 */
HWTEST_F(VSyncDistributorTest, HasPendingUIRNV001, Function | MediumTest| Level3)
{
    auto res = VSyncDistributorTest::vsyncDistributor->HasPendingUIRNV();
    EXPECT_FALSE(res);
}

/*
* Function: UpdatePendingReferenceTime001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdatePendingReferenceTime
 */
HWTEST_F(VSyncDistributorTest, UpdatePendingReferenceTime001, Function | MediumTest| Level3)
{
    int64_t timeStamp = 0;
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    auto res = VSyncDistributorTest::vsyncDistributor->SetUiDvsyncSwitch(true, conn);
    ASSERT_EQ(res, VSYNC_ERROR_OK);
    VSyncDistributorTest::vsyncDistributor->UpdatePendingReferenceTime(timeStamp);
}

/*
* Function: SetHardwareTaskNum001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetHardwareTaskNum
 */
HWTEST_F(VSyncDistributorTest, SetHardwareTaskNum001, Function | MediumTest| Level3)
{
    uint32_t num = 0;
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    auto res = VSyncDistributorTest::vsyncDistributor->SetUiDvsyncSwitch(true, conn);
    ASSERT_EQ(res, VSYNC_ERROR_OK);
    VSyncDistributorTest::vsyncDistributor->SetHardwareTaskNum(num);
}

/*
* Function: GetUiCommandDelayTime001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetUiCommandDelayTime
 */
HWTEST_F(VSyncDistributorTest, GetUiCommandDelayTime001, Function | MediumTest| Level3)
{
    vsyncDistributor->isRs_ = false;
    VSyncDistributorTest::vsyncDistributor->GetUiCommandDelayTime();
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: SetUiDvsyncConfig001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUiDvsyncConfig
 */
HWTEST_F(VSyncDistributorTest, SetUiDvsyncConfig001, Function | MediumTest| Level3)
{
    uint32_t bufferCount = 2;
    ASSERT_EQ(VSyncDistributorTest::vsyncDistributor->SetUiDvsyncConfig(bufferCount), VSYNC_ERROR_OK);
}

/*
* Function: SetUiDvsyncSwitchTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUiDvsyncSwitch
 */
HWTEST_F(VSyncDistributorTest, SetUiDvsyncSwitchTest, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    ASSERT_EQ(conn->SetUiDvsyncSwitch(true), VSYNC_ERROR_OK);
    ASSERT_EQ(conn->SetUiDvsyncSwitch(false), VSYNC_ERROR_OK);
}

/*
* Function: SetUiDvsyncSwitchTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new VSyncConnection(nullptr, "VSyncDistributorTest")
*                 2. call SetUiDvsyncSwitch
*                3. return VSYNC_ERROR_NULLPTR
 */
HWTEST_F(VSyncDistributorTest, SetUiDvsyncSwitchTest001, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(nullptr, "VSyncDistributorTest");
    ASSERT_EQ(conn->SetUiDvsyncSwitch(true), VSYNC_ERROR_NULLPTR);
    ASSERT_EQ(conn->SetUiDvsyncSwitch(false), VSYNC_ERROR_NULLPTR);
}

/*
* Function: SetUiDvsyncConfigTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUiDvsyncConfig
 */
HWTEST_F(VSyncDistributorTest, SetUiDvsyncConfigTest, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "VSyncDistributorTest");
    ASSERT_EQ(conn->SetUiDvsyncConfig(1), VSYNC_ERROR_OK);
}

/*
* Function: SetUiDvsyncConfigTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new VSyncConnection(nullptr, "VSyncDistributorTest")
*                 2. call SetUiDvsyncConfig
*                3. return VSYNC_ERROR_NULLPTR
 */
HWTEST_F(VSyncDistributorTest, SetUiDvsyncConfigTest001, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(nullptr, "VSyncDistributorTest");
    ASSERT_EQ(conn->SetUiDvsyncConfig(1), VSYNC_ERROR_NULLPTR);
}

/*
* Function: AddConnectionOverFlowTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call AddConnection over 256 times
 */
HWTEST_F(VSyncDistributorTest, AddConnectionOverFlowTest, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 257; i++) { // add 257 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_1234");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_API_FAILED);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: OnVSyncTriggerTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test OnVSyncTrigger
 */
HWTEST_F(VSyncDistributorTest, OnVSyncTriggerTest001, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    vsyncDistributor->OnVSyncTrigger(1000000000, 8333333, 120, VSYNC_MODE_LTPO, 360);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: OnVSyncTriggerTest002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test OnVSyncTrigger
 */
HWTEST_F(VSyncDistributorTest, OnVSyncTriggerTest002, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    vsyncDistributor->OnVSyncTrigger(1000000000, -8333333, 120, VSYNC_MODE_LTPO, 360);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: OnVSyncTriggerTest003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test OnVSyncTrigger
 */
HWTEST_F(VSyncDistributorTest, OnVSyncTriggerTest003, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    vsyncDistributor->OnVSyncTrigger(1000000000, 8333333, 120, VSYNC_MODE_LTPS, 360);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateByPidTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRateByPid
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateByPidTest001, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPid(0, 2, false), VSYNC_ERROR_OK);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateByPidTest002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRateByPid
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateByPidTest002, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPid(0, 1, false), VSYNC_ERROR_INVALID_ARGUMENTS);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateByPidTest003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRateByPid
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateByPidTest003, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPid(0, 2, true), VSYNC_ERROR_INVALID_ARGUMENTS);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateByPidTest004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRateByPid
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateByPidTest004, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPid(0, 1, true), VSYNC_ERROR_INVALID_ARGUMENTS);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateByPidTest005
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRateByPid
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateByPidTest005, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPid(1, 1, true), VSYNC_ERROR_OK);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateByPidTest006
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRateByPid
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateByPidTest006, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
        ASSERT_EQ(vsyncDistributor->SetHighPriorityVSyncRate(1, conn), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPid(1, 1, true), VSYNC_ERROR_OK);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRate
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateTest001, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
        ASSERT_EQ(vsyncDistributor->SetHighPriorityVSyncRate(1, conn), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRate(0xffffffffff, 1, true), VSYNC_ERROR_INVALID_ARGUMENTS);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateTest002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRate
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateTest002, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        ASSERT_EQ(vsyncDistributor->SetHighPriorityVSyncRate(1, conn), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRate(0x1ffffffff, 1, true), VSYNC_ERROR_OK);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateTest003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRate
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateTest003, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    ASSERT_EQ(vsyncDistributor->AddConnection(conn), VSYNC_ERROR_OK);
    conns.emplace_back(conn);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRate(0x1ffffffff, 1, true), VSYNC_ERROR_OK);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: SetQosVSyncRateByPidPublicTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetQosVSyncRateByPidPublic
 */
HWTEST_F(VSyncDistributorTest, SetQosVSyncRateByPidPublicTest001, Function | MediumTest| Level3)
{
    std::vector<sptr<VSyncConnection>> conns;
    for (int i = 0; i < 10; i++) { // add 10 connections
        sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "WM_" + std::to_string(i+1));
        ASSERT_EQ(vsyncDistributor->AddConnection(conn, 1), VSYNC_ERROR_OK);
        conns.emplace_back(conn);
    }
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPidPublic(0, 1, false), VSYNC_ERROR_OK);
    ASSERT_EQ(vsyncDistributor->SetQosVSyncRateByPidPublic(0, 2, false), VSYNC_ERROR_OK);
    for (int i = 0; i < conns.size(); i++) {
        ASSERT_EQ(vsyncDistributor->RemoveConnection(conns[i]), VSYNC_ERROR_OK);
    }
}

/*
* Function: TriggerNextConnPostEventTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test TriggerNext 2. test ConnPostEvent
 */
HWTEST_F(VSyncDistributorTest, TriggerNextConnPostEventTest001, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    conn->requestVsyncTimestamp_.clear();
    vsyncDistributor->TriggerNext(conn);
    vsyncDistributor->ConnPostEvent(conn, 10000000, 8333333, 1);
    ASSERT_EQ(conn->requestVsyncTimestamp_.size() != 0, true);
}

/*
* Function: ConnectionsPostEventTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test ConnectionsPostEvent
 */
HWTEST_F(VSyncDistributorTest, ConnectionsPostEventTest001, Function | MediumTest| Level3)
{
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    std::vector<sptr<VSyncConnection>> conns = {conn};
    int64_t now = 10000000;
    int64_t period = 16666666;
    int64_t generatorRefreshRate = 60;
    int64_t vsyncCount = 1;
    bool isDvsyncController = false;
    vsyncDistributor->ConnectionsPostEvent(conns, now, period, generatorRefreshRate, vsyncCount, isDvsyncController);
    ASSERT_EQ(isDvsyncController, false);
}

/*
* Function: DisableDVSyncControllerTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test DisableDVSyncController
 */
HWTEST_F(VSyncDistributorTest, DisableDVSyncControllerTest001, Function | MediumTest| Level3)
{
    vsyncDistributor->dvsyncControllerEnabled_ = false;
    vsyncDistributor->DisableDVSyncController();
    ASSERT_EQ(vsyncDistributor->dvsyncControllerEnabled_, false);
}

/*
* Function: OnDVSyncEventTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test OnDVSyncEvent
 */
HWTEST_F(VSyncDistributorTest, OnDVSyncEventTest001, Function | MediumTest| Level3)
{
    int64_t now = 10000000;
    int64_t period = 16666666;
    uint32_t refreshRate = 60;
    VSyncMode vsyncMode = VSYNC_MODE_LTPO;
    uint32_t vsyncMaxRefreshRate = 120;
    vsyncDistributor->OnDVSyncEvent(now, period, refreshRate, vsyncMode, vsyncMaxRefreshRate);
    ASSERT_EQ(period, 16666666);
}

/*
* Function: InitDVSyncTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test InitDVSync
 */
HWTEST_F(VSyncDistributorTest, InitDVSyncTest001, Function | MediumTest| Level3)
{
    vsyncDistributor->isRs_ = false;
    vsyncDistributor->InitDVSync();
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: DVSyncAddConnectionDVSyncDisableVSyncTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test DVSyncAddConnection 2. test DVSyncDisableVSync
 */
HWTEST_F(VSyncDistributorTest, DVSyncAddConnectionDVSyncDisableVSyncTest001, Function | MediumTest| Level3)
{
    vsyncDistributor->isRs_ = false;
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    vsyncDistributor->DVSyncAddConnection(conn);
    vsyncDistributor->DVSyncDisableVSync();
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: DVSyncRecordVSyncTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test DVSyncRecordVSync
 */
HWTEST_F(VSyncDistributorTest, DVSyncRecordVSyncTest001, Function | MediumTest| Level3)
{
    int64_t now = 10000000;
    int64_t period = 16666666;
    uint32_t refreshRate = 60;
    bool isDvsyncController = false;
    vsyncDistributor->DVSyncRecordVSync(now, period, refreshRate, isDvsyncController);
    ASSERT_EQ(isDvsyncController, false);
}

/*
* Function: DVSyncCheckSkipAndUpdateTsTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test DVSyncCheckSkipAndUpdateTs
 */
HWTEST_F(VSyncDistributorTest, DVSyncCheckSkipAndUpdateTsTest001, Function | MediumTest| Level3)
{
    vsyncDistributor->isRs_ = false;
    int64_t timeStamp = 10000000;
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    vsyncDistributor->DVSyncCheckSkipAndUpdateTs(conn, timeStamp);
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: DVSyncNeedSkipUiTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test DVSyncNeedSkipUi
 */
HWTEST_F(VSyncDistributorTest, DVSyncNeedSkipUiTest001, Function | MediumTest| Level3)
{
    vsyncDistributor->isRs_ = false;
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    vsyncDistributor->DVSyncNeedSkipUi(conn);
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: RecordEnableVsyncTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test RecordEnableVsync
 */
HWTEST_F(VSyncDistributorTest, RecordEnableVsyncTest001, Function | MediumTest| Level3)
{
    vsyncDistributor->isRs_ = false;
    vsyncDistributor->RecordEnableVsync();
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: DVSyncRecordRNVTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test DVSyncRecordRNV
 */
HWTEST_F(VSyncDistributorTest, DVSyncRecordRNVTest001, Function | MediumTest| Level3)
{
    std::string fromWhom = "test";
    vsyncDistributor->isRs_ = false;
    int64_t ts = 10000000;
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    vsyncDistributor->DVSyncRecordRNV(conn, fromWhom, ts);
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: DVSyncCheckPreexecuteAndUpdateTsTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test DVSyncCheckPreexecuteAndUpdateTs
 */
HWTEST_F(VSyncDistributorTest, DVSyncCheckPreexecuteAndUpdateTsTest001, Function | MediumTest| Level3)
{
    vsyncDistributor->isRs_ = false;
    int64_t timestamp = 10000000;
    int64_t period = 8333333;
    int64_t vsyncCount = 1;
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, "test");
    vsyncDistributor->DVSyncCheckPreexecuteAndUpdateTs(conn, timestamp, period, vsyncCount);
    ASSERT_EQ(vsyncDistributor->isRs_, false);
}

/*
* Function: NotifyPackageEventTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test NotifyPackageEvent
 */
HWTEST_F(VSyncDistributorTest, NotifyPackageEventTest001, Function | MediumTest| Level3)
{
    std::vector<std::string> packageList = {};
    vsyncDistributor->NotifyPackageEvent(packageList);
    ASSERT_EQ(packageList.size(), 0);
}

/*
* Function: NotifyTouchEventTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test NotifyTouchEvent
 */
HWTEST_F(VSyncDistributorTest, NotifyTouchEventTest001, Function | MediumTest| Level3)
{
    int32_t touchStatus = 0;
    int32_t touchCnt = 0;
    vsyncDistributor->NotifyTouchEvent(touchStatus, touchCnt);
    ASSERT_EQ(touchStatus, 0);
}

/*
* Function: AdaptiveDVSyncEnableTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test AdaptiveDVSyncEnable
 */
HWTEST_F(VSyncDistributorTest, AdaptiveDVSyncEnableTest001, Function | MediumTest| Level3)
{
    std::string nodeName = "test";
    bool needConsume = true;
    vsyncDistributor->AdaptiveDVSyncEnable(nodeName, 0, 0, needConsume);
    ASSERT_EQ(needConsume, true);
}

/*
* Function: SetVsyncRateDiscountLTPSTest001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test SetVsyncRateDiscountLTPS
 */
HWTEST_F(VSyncDistributorTest, SetVsyncRateDiscountLTPSTest001, Function | MediumTest| Level3)
{
    std::string name = "UnityChoreographer";
    sptr<VSyncConnection> conn = new VSyncConnection(vsyncDistributor, name, nullptr, 4294967296);

    vsyncDistributor->GetVsyncNameLinkerIds(1, name);
    ASSERT_EQ(vsyncDistributor->SetVsyncRateDiscountLTPS(1, name, 1), VSYNC_ERROR_INVALID_ARGUMENTS);

    ASSERT_EQ(vsyncDistributor->AddConnection(conn, 0), VSYNC_ERROR_OK);
    vsyncDistributor->GetVsyncNameLinkerIds(1, name);
    ASSERT_EQ(vsyncDistributor->SetVsyncRateDiscountLTPS(1, name, 1), VSYNC_ERROR_OK);
    ASSERT_EQ(vsyncDistributor->SetVsyncRateDiscountLTPS(1, name, 2), VSYNC_ERROR_OK);
    ASSERT_EQ(vsyncDistributor->RemoveConnection(conn), VSYNC_ERROR_OK);
}

/*
 * Function: AddRequestVsyncTimestamp001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. test AddRequestVsyncTimestamp001
 */
HWTEST_F(VSyncDistributorTest, AddRequestVsyncTimestamp001, Function | MediumTest | Level3)
{
    vsyncConnection->requestVsyncTimestamp_.clear();
    int64_t timestamp = 1;
    vsyncConnection->AddRequestVsyncTimestamp(timestamp);
    ASSERT_EQ(vsyncConnection->requestVsyncTimestamp_.size(), 1);
}

/*
 * Function: RemoveTriggeredVsync001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. test RemoveTriggeredVsync
 */
HWTEST_F(VSyncDistributorTest, RemoveTriggeredVsync001, Function | MediumTest | Level3)
{
    vsyncConnection->requestVsyncTimestamp_.clear();
    vsyncConnection->requestVsyncTimestamp_.insert(100);
    int64_t currentTime = 1000;
    vsyncConnection->RemoveTriggeredVsync(currentTime);
    ASSERT_EQ(vsyncConnection->requestVsyncTimestamp_.size(), 0);
}

/*
 * Function: NeedTriggeredVsync001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. test NeedTriggeredVsync
 */
HWTEST_F(VSyncDistributorTest, NeedTriggeredVsync001, Function | MediumTest | Level3)
{
    vsyncConnection->requestVsyncTimestamp_.clear();
    vsyncConnection->requestVsyncTimestamp_.insert(100);
    int64_t currentTime = 1000;
    ASSERT_TRUE(vsyncConnection->NeedTriggeredVsync(currentTime));
    currentTime = 10;
    ASSERT_TRUE(vsyncConnection->NeedTriggeredVsync(currentTime));
}
} // namespace
} // namespace Rosen
} // namespace OHOS