/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "vsync_manager.h"
#include "vsync_manager_agent.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class VSyncManagerAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<RSVsyncManager> vsyncManager_;
    static inline sptr<RSVsyncManagerAgent> vsyncManagerAgent_;
    static inline sptr<RSScreenManager> screenManager_;
    static constexpr const int32_t WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS = 2;
};

void VSyncManagerAgentTest::SetUpTestCase()
{
    screenManager_ = sptr<RSScreenManager>::MakeSptr();
    vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    if (!vsyncManager_->init(screenManager_)) {
        RS_LOGE("vsync manager case::%{public}s: vsync init failed", __func__);
    }
    vsyncManagerAgent_ = vsyncManager_->GetVsyncManagerAgent();
}

void VSyncManagerAgentTest::TearDownTestCase()
{
    sleep(WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS);
    screenManager_ = nullptr;
    vsyncManager_ = nullptr;
    vsyncManagerAgent_ = nullptr;
}

static int64_t SystemTime()
{
    timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec) * 1000000000LL + t.tv_nsec; // 1000000000ns == 1s
}

namespace {
/*
* Function: IsVsyncRSDistributorExist
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test IsVsyncRSDistributorExist
 */
HWTEST_F(VSyncManagerAgentTest, IsVsyncRSDistributorExist, Function | MediumTest| Level0)
{
    auto res = vsyncManagerAgent_->IsVsyncRSDistributorExist();
    ASSERT_TRUE(res);
}

/*
* Function: IsVsyncAppDistributorExist
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test IsVsyncAppDistributorExist
 */
HWTEST_F(VSyncManagerAgentTest, IsVsyncAppDistributorExist, Function | MediumTest| Level0)
{
    auto res = vsyncManagerAgent_->IsVsyncAppDistributorExist();
    ASSERT_TRUE(res);
}

/*
* Function: ForceRsDVsync
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ForceRsDVsync
 */
HWTEST_F(VSyncManagerAgentTest, ForceRsDVsync, Function | MediumTest| Level0)
{
    std::string sceneId = "TestSceneId";
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->ForceRsDVsync(sceneId);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->ForceRsDVsync(sceneId);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: DVSyncUpdate
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test DVSyncUpdate
 */
HWTEST_F(VSyncManagerAgentTest, DVSyncUpdate, Function | MediumTest| Level0)
{
    uint64_t dvsyncTime = 0;
    uint64_t vsyncTime = 0;
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->DVSyncUpdate(dvsyncTime, vsyncTime);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->DVSyncUpdate(dvsyncTime, vsyncTime);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: SetTaskEndWithTime
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test SetTaskEndWithTime
 */
HWTEST_F(VSyncManagerAgentTest, SetTaskEndWithTime, Function | MediumTest| Level0)
{
    uint64_t time = 0;
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->SetTaskEndWithTime(time);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->SetTaskEndWithTime(time);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: NotifyPackageEvent
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test NotifyPackageEvent
 */
HWTEST_F(VSyncManagerAgentTest, NotifyPackageEvent, Function | MediumTest| Level0)
{
    std::vector<std::string> packageList;
    packageList.push_back("test");
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->NotifyPackageEvent(packageList);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->NotifyPackageEvent(packageList);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: SetBufferInfo
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test SetBufferInfo
 */
HWTEST_F(VSyncManagerAgentTest, SetBufferInfo, Function | MediumTest| Level0)
{
    BufferInfo bufferInfo;
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->SetBufferInfo(bufferInfo);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->SetBufferInfo(bufferInfo);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: GetRealTimeOffsetOfDvsync
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetRealTimeOffsetOfDvsync
 */
HWTEST_F(VSyncManagerAgentTest, GetRealTimeOffsetOfDvsync, Function | MediumTest| Level0)
{
    int64_t time = SystemTime();
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    uint64_t res = vsyncManagerAgent_->GetRealTimeOffsetOfDvsync(time);
    ASSERT_EQ(res, 0);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->GetRealTimeOffsetOfDvsync(time);
    ASSERT_EQ(res, 0);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
}

/*
* Function: SetHardwareTaskNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test SetHardwareTaskNum
 */
HWTEST_F(VSyncManagerAgentTest, SetHardwareTaskNum, Function | MediumTest| Level0)
{
    uint32_t num = 1;
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->SetHardwareTaskNum(num);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->SetHardwareTaskNum(num);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: CheckVsyncReceivedAndGetRelTs
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test CheckVsyncReceivedAndGetRelTs
 */
HWTEST_F(VSyncManagerAgentTest, CheckVsyncReceivedAndGetRelTs, Function | MediumTest| Level0)
{
    uint64_t timestamp = SystemTime();
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    uint64_t res = vsyncManagerAgent_->CheckVsyncReceivedAndGetRelTs(timestamp);
    ASSERT_EQ(res, timestamp);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->CheckVsyncReceivedAndGetRelTs(timestamp);
    ASSERT_EQ(res, timestamp);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
}

/*
* Function: PrintConnectionsStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test PrintConnectionsStatus
 */
HWTEST_F(VSyncManagerAgentTest, PrintConnectionsStatus, Function | MediumTest| Level0)
{
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->PrintConnectionsStatus();
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->PrintConnectionsStatus();
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: PrintGeneratorStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test PrintGeneratorStatus
 */
HWTEST_F(VSyncManagerAgentTest, PrintGeneratorStatus, Function | MediumTest| Level0)
{
    auto generator = vsyncManagerAgent_->vsyncGenerator_;
    vsyncManagerAgent_->PrintGeneratorStatus();
    vsyncManagerAgent_->vsyncGenerator_ = nullptr;
    vsyncManagerAgent_->PrintGeneratorStatus();
    vsyncManagerAgent_->vsyncGenerator_ = generator;
    ASSERT_NE(vsyncManagerAgent_->vsyncGenerator_, nullptr);
}

/*
* Function: SetFrameIsRender
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test SetFrameIsRender
 */
HWTEST_F(VSyncManagerAgentTest, SetFrameIsRender, Function | MediumTest| Level0)
{
    bool isRender = true;
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->SetFrameIsRender(isRender);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->SetFrameIsRender(isRender);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: GetUiCommandDelayTime
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetUiCommandDelayTime
 */
HWTEST_F(VSyncManagerAgentTest, GetUiCommandDelayTime, Function | MediumTest| Level0)
{
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    int64_t res = vsyncManagerAgent_->GetUiCommandDelayTime();
    ASSERT_EQ(res, 0);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->GetUiCommandDelayTime();
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_EQ(res, 0);
}

/*
* Function: SetVSyncTimeUpdated
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test SetVSyncTimeUpdated
 */
HWTEST_F(VSyncManagerAgentTest, SetVSyncTimeUpdated, Function | MediumTest| Level0)
{
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->SetVSyncTimeUpdated();
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->SetVSyncTimeUpdated();
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}

/*
* Function: NeedUpdateVSyncTime
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test NeedUpdateVSyncTime
 */
HWTEST_F(VSyncManagerAgentTest, NeedUpdateVSyncTime, Function | MediumTest| Level0)
{
    int32_t pid = 1234;
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    bool res = vsyncManagerAgent_->NeedUpdateVSyncTime(pid);
    ASSERT_FALSE(res);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->NeedUpdateVSyncTime(pid);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_FALSE(res);
}

/*
* Function: GetLastUpdateTime
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetLastUpdateTime
 */
HWTEST_F(VSyncManagerAgentTest, GetLastUpdateTime, Function | MediumTest| Level0)
{
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    int64_t res = vsyncManagerAgent_->GetLastUpdateTime();
    ASSERT_EQ(res, 0);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->GetLastUpdateTime();
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_EQ(res, 0);
}

/*
* Function: IsUiDvsyncOn
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test IsUiDvsyncOn
 */
HWTEST_F(VSyncManagerAgentTest, IsUiDvsyncOn, Function | MediumTest| Level0)
{
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    bool res = vsyncManagerAgent_->IsUiDvsyncOn();
    ASSERT_FALSE(res);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->IsUiDvsyncOn();
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_FALSE(res);
}

/*
* Function: GetWaitForDvsyncFrame
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetWaitForDvsyncFrame
 */
HWTEST_F(VSyncManagerAgentTest, GetWaitForDvsyncFrame, Function | MediumTest| Level0)
{
    bool init = vsyncManagerAgent_->GetWaitForDvsyncFrame();
    bool wait = true;
    vsyncManagerAgent_->SetWaitForDvsyncFrame(wait);
    bool res = vsyncManagerAgent_->GetWaitForDvsyncFrame();
    ASSERT_TRUE(res);
    wait = false;
    vsyncManagerAgent_->SetWaitForDvsyncFrame(wait);
    res = vsyncManagerAgent_->GetWaitForDvsyncFrame();
    ASSERT_FALSE(res);
    vsyncManagerAgent_->SetWaitForDvsyncFrame(init);
}

/*
* Function: SetQosVSyncAppRateByPidPublic
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test SetQosVSyncAppRateByPidPublic
 */
HWTEST_F(VSyncManagerAgentTest, SetQosVSyncAppRateByPidPublic, Function | MediumTest| Level0)
{
    uint32_t pid = 1234;
    uint32_t rate = 60;
    bool isSystemAnimateScene = true;
    auto appDistributor = vsyncManagerAgent_->appVSyncDistributor_;
    auto res = vsyncManagerAgent_->SetQosVSyncAppRateByPidPublic(pid, rate, isSystemAnimateScene);
    ASSERT_EQ(res, VSYNC_ERROR_OK);
    vsyncManagerAgent_->appVSyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->SetQosVSyncAppRateByPidPublic(pid, rate, isSystemAnimateScene);
    vsyncManagerAgent_->appVSyncDistributor_ = appDistributor;
    ASSERT_EQ(res, VSYNC_ERROR_API_FAILED);
}

/*
* Function: AddAppVsyncConnection
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test AddAppVsyncConnection
 */
HWTEST_F(VSyncManagerAgentTest, AddAppVsyncConnection, Function | MediumTest| Level0)
{
    std::string name = "test";
    sptr<VSyncIConnectionToken> vsyncToken = new IRemoteStub<VSyncIConnectionToken>();
    uint64_t id = 10;
    uint64_t windowNodeId = 1;
    auto appDistributor = vsyncManagerAgent_->appVSyncDistributor_;
    sptr<VSyncConnection> conn = vsyncManagerAgent_->CreateAppVsyncConnection(name, vsyncToken, id, windowNodeId);
    ASSERT_NE(conn, nullptr);
    auto res = vsyncManagerAgent_->AddAppVsyncConnection(conn, windowNodeId);
    ASSERT_EQ(res, VSYNC_ERROR_OK);
    vsyncManagerAgent_->appVSyncDistributor_ = nullptr;
    res = vsyncManagerAgent_->AddAppVsyncConnection(conn, windowNodeId);
    vsyncManagerAgent_->appVSyncDistributor_ = appDistributor;
    ASSERT_EQ(res, VSYNC_ERROR_API_FAILED);
}

/*
* Function: VsyncRSDistributorHandleTouchEvent
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test VsyncRSDistributorHandleTouchEvent
 */
HWTEST_F(VSyncManagerAgentTest, VsyncRSDistributorHandleTouchEvent, Function | MediumTest| Level0)
{
    int32_t touchStatus = TouchStatus::TOUCH_DOWN;
    int32_t touchCnt = 1;
    auto rsDistributor = vsyncManagerAgent_->rsVsyncDistributor_;
    vsyncManagerAgent_->VsyncRSDistributorHandleTouchEvent(touchStatus, touchCnt);
    vsyncManagerAgent_->rsVsyncDistributor_ = nullptr;
    vsyncManagerAgent_->VsyncRSDistributorHandleTouchEvent(touchStatus, touchCnt);
    vsyncManagerAgent_->rsVsyncDistributor_ = rsDistributor;
    ASSERT_NE(vsyncManagerAgent_->rsVsyncDistributor_, nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
