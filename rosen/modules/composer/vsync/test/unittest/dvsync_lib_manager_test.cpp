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
#include <gtest/gtest.h>

#include "dvsync_lib_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DVSyncLibManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<VSyncController> vsyncController = nullptr;
    static inline sptr<VSyncDistributor> vsyncDistributor = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator = nullptr;
    static inline sptr<VSyncConnection> vsyncConnection = nullptr;
    static inline DVSyncLibManager dvsyncLibManager;
};

void DVSyncLibManagerTest::SetUpTestCase()
{
    vsyncGenerator = CreateVSyncGenerator();
    vsyncController = new VSyncController(vsyncGenerator, 0);
    vsyncDistributor = new VSyncDistributor(vsyncController, "VSyncConnection");
}

void DVSyncLibManagerTest::TearDownTestCase()
{
    vsyncController = nullptr;
    vsyncGenerator = nullptr;
    vsyncDistributor = nullptr;
    vsyncConnection = nullptr;
}

namespace {
/*
* Function: Initialize001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test func when so is not loaded
 */
HWTEST_F(DVSyncLibManagerTest, Initialize001, Function | MediumTest| Level1)
{
    int64_t timeStamp = 0;
    uint64_t timestamp = 0;
    int64_t period = 0;
    bool start = true;
    bool isNative = false;
    DVSyncFeatureParam dvsyncParam;
    std::vector<std::string> packageList;
    int32_t touchStatus = 0;
    int32_t touchCnt = 0;
    int32_t pid;
    uint32_t num = 0;
    bool isAppRequested = true;
    int64_t time = 0;
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.IsAppDVSyncOn(), false);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.SetAppRequestedStatus(vsyncConnection, isAppRequested), false);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.CheckVsyncReceivedAndGetRelTs(timestamp), 0);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetOccurPeriod(), 0);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetOccurRefreshRate(), 0);

    DVSyncLibManagerTest::dvsyncLibManager.GetConnectionApp(vsyncConnection);
    DVSyncLibManagerTest::dvsyncLibManager.MarkRSRendering(vsyncConnection);
    DVSyncLibManagerTest::dvsyncLibManager.SetAppDVSyncSwitch(vsyncConnection, start, isNative);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetUiCommandDelayTime(), 0);
    DVSyncLibManagerTest::dvsyncLibManager.UpdatePendingReferenceTime(timeStamp);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetRealTimeOffsetOfDvsync(time), 0);
    DVSyncLibManagerTest::dvsyncLibManager.SetHardwareTaskNum(num);
    DVSyncLibManagerTest::dvsyncLibManager.SetPhysicalScreenNum(num);
    DVSyncLibManagerTest::dvsyncLibManager.SetTaskEndWithTime(timestamp);
    DVSyncLibManagerTest::dvsyncLibManager.InitWithParam(dvsyncParam);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.SetDistributor(true, vsyncDistributor), false);
    DVSyncLibManagerTest::dvsyncLibManager.SetConnection(true, vsyncConnection);
    DVSyncLibManagerTest::dvsyncLibManager.DisableVSync(vsyncDistributor);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.NeedSkipAndUpdateTs(vsyncConnection, timeStamp), false);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.NeedSkipUi(vsyncConnection), false);
    DVSyncLibManagerTest::dvsyncLibManager.RecordEnableVsync(vsyncDistributor);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.NeedPreexecuteAndUpdateTs(vsyncConnection, timeStamp, period),
        false);

    DVSyncLibManagerTest::dvsyncLibManager.NotifyPackageEvent(packageList);
    DVSyncLibManagerTest::dvsyncLibManager.HandleTouchEvent(touchStatus, touchCnt);
    DVSyncLibManagerTest::dvsyncLibManager.IsAppRequested();
    DVSyncLibManagerTest::dvsyncLibManager.GetVSyncConnectionApp(vsyncConnection);

    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.NeedUpdateVSyncTime(pid), false);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetLastUpdateTime(), 0);
}

/*
* Function: Initialize002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test func when so is not loaded
 */
HWTEST_F(DVSyncLibManagerTest, Initialize002, Function | MediumTest| Level1)
{
    std::string sceneId = "1";
    uint32_t currRefreshRate = 0;
    uint32_t lastRefreshRate = 0;
    uint32_t dvsyncRate = 30;
    int64_t VsyncCount = 0;
    int64_t offset = 0;
    int32_t fd = 1000;
    int32_t touchType = 3;
    std::string name = "test";
    uint64_t dvsyncTime = 0;
    uint64_t vsyncTime =0;
    AppExecFwk::InnerEvent::Callback callback;
    DVSyncLibManagerTest::dvsyncLibManager.DVSyncUpdate(dvsyncTime, vsyncTime);
    DVSyncLibManagerTest::dvsyncLibManager.ForceRsDVsync(sceneId);
    ASSERT_EQ("1", sceneId);
    DVSyncLibManagerTest::dvsyncLibManager.SetCurrentRefreshRate(currRefreshRate, lastRefreshRate);
    DVSyncLibManagerTest::dvsyncLibManager.DVSyncRateChanged(currRefreshRate, dvsyncRate);
    ASSERT_EQ(30, dvsyncRate);
    DVSyncLibManagerTest::dvsyncLibManager.SetToCurrentPeriod();
    DVSyncLibManagerTest::dvsyncLibManager.GetVsyncCount(VsyncCount);
    ASSERT_EQ(1, VsyncCount);

    DVSyncLibManagerTest::dvsyncLibManager.InitDvsyncController(vsyncGenerator, offset, vsyncController);
    DVSyncLibManagerTest::dvsyncLibManager.SetVSyncTimeUpdated();
    DVSyncLibManagerTest::dvsyncLibManager.ToDelay(callback, name, fd);
    DVSyncLibManagerTest::dvsyncLibManager.SetTouchEvent(touchType);
    DVSyncLibManagerTest::dvsyncLibManager.UpdateDelayInfo();
    ASSERT_EQ(dvsyncLibManager.updateDelayInfoFunc_, nullptr);
}

/*
* Function: RecordVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RecordVSync
 */
HWTEST_F(DVSyncLibManagerTest, RecordVSync001, Function | MediumTest| Level1)
{
    int64_t now = 0;
    int64_t period = 8333333;
    uint32_t refreshRate = 120;
    bool isDVSyncController = true;
    DVSyncLibManagerTest::dvsyncLibManager.RecordVSync(vsyncDistributor, now, period, refreshRate,
        isDVSyncController);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.recordVSyncFunc_, nullptr);
}

/*
* Function: SetUiDVSyncConfig001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetUiDVSyncConfig
 */
HWTEST_F(DVSyncLibManagerTest, SetUiDVSyncConfig001, Function | MediumTest| Level1)
{
    int32_t bufferCount = 1;
    bool compositeSceneEnable = true;
    bool nativeDelayEnable = true;
    std::vector<std::string> rsDvsyncAnimationList;
    DVSyncLibManagerTest::dvsyncLibManager.SetUiDVSyncConfig(bufferCount, compositeSceneEnable,
        nativeDelayEnable, rsDvsyncAnimationList);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.setUiDVSyncConfigFunc_, nullptr);
}

/*
* Function: RecordRNV001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RecordRNV
 */
HWTEST_F(DVSyncLibManagerTest, RecordRNV001, Function | MediumTest| Level1)
{
    std::string fromWhom = "UI";
    int64_t lastVSyncTS = 0;
    int64_t requestVsyncTime = 0;
    DVSyncLibManagerTest::dvsyncLibManager.RecordRNV(vsyncConnection, fromWhom, VSYNC_MODE_LTPO,
        lastVSyncTS, requestVsyncTime);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.recordRNVFunc_, nullptr);
}

/*
* Function: SetBufferInfo001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetBufferInfo
 */
HWTEST_F(DVSyncLibManagerTest, SetBufferInfo001, Function | MediumTest| Level1)
{
    uint64_t id = 0;
    std::string name = "test";
    int32_t queueSize = 0;
    int32_t bufferCount = 0;
    int64_t lastConsumeTime = 0;
    DVSyncLibManagerTest::dvsyncLibManager.SetBufferInfo(id, name, queueSize, bufferCount, lastConsumeTime);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.setBufferInfoFunc_, nullptr);
}

/*
* Function: UpdateReferenceTimeAndPeriod001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateReferenceTimeAndPeriod
 */
HWTEST_F(DVSyncLibManagerTest, UpdateReferenceTimeAndPeriod001, Function | MediumTest| Level1)
{
    bool isLtpoNeedChange = false;
    int64_t occurDvsyncReferenceTime = -1;
    int64_t dvsyncPeriodRecord = -1;
    DVSyncLibManagerTest::dvsyncLibManager.UpdateReferenceTimeAndPeriod(isLtpoNeedChange, occurDvsyncReferenceTime,
        dvsyncPeriodRecord);
    ASSERT_EQ(false, isLtpoNeedChange);
    ASSERT_EQ(-1, occurDvsyncReferenceTime);
    ASSERT_EQ(-1, dvsyncPeriodRecord);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.updateReferenceTimeAndPeriodFunc_, nullptr);
}

/*
* Function: Initialize003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test func when so is not right.
 */
HWTEST_F(DVSyncLibManagerTest, Initialize003, Function | MediumTest| Level1)
{
    bool success = DVSyncLibManagerTest::dvsyncLibManager.Initialize("nolibdvsync.so");
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
    ASSERT_EQ(success, false);
    DVSyncLibManagerTest::dvsyncLibManager.Shutdown();
}

/*
* Function: Initialize004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test func when so is right and load twice.
 */
HWTEST_F(DVSyncLibManagerTest, Initialize004, Function | MediumTest| Level1)
{
    bool success = DVSyncLibManagerTest::dvsyncLibManager.Initialize("libdvsync.z.so");
    if (!success) {
        return;
    }
    success = DVSyncLibManagerTest::dvsyncLibManager.Initialize("libdvsync.z.so");
    ASSERT_EQ(success, true);
    DVSyncLibManagerTest::dvsyncLibManager.ClearAllFunctions();
    DVSyncLibManagerTest::dvsyncLibManager.Shutdown();
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
}

/*
* Function: Initialize004
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test func when so is right and load twice.
 */
HWTEST_F(DVSyncLibManagerTest, Initialize005, Function | MediumTest| Level1)
{
    int32_t pid;
    uint64_t dvsyncTime = 0;
    uint64_t vsyncTime =0;
    bool success = dvsyncLibManager.Initialize("libdvsync.z.so");
    if (!success) {
        return;
    }
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
    sptr<VSyncConnection> vsyncCon = nullptr;
    DVSyncLibManagerTest::dvsyncLibManager.GetConnectionApp(vsyncCon);
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.getConnectionAppFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.IsAppRequested();
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.isAppRequestedFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.GetVSyncConnectionApp(vsyncCon);
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.getVSyncConnectionAppFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.NeedUpdateVSyncTime(pid);
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.needUpdateVSyncTimeFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.GetLastUpdateTime();
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.getLastUpdateTimeFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.DVSyncUpdate(dvsyncTime, vsyncTime);
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.dvsyncUpdateFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.SetToCurrentPeriod();
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.setToCurrentPeriodFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.SetVSyncTimeUpdated();
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.setVSyncTimeUpdatedFunc_, nullptr);
    DVSyncLibManagerTest::dvsyncLibManager.Shutdown();
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
}

/*
* Function: LoadFunction001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test LoadFunction when load func not exist.
 */
HWTEST_F(DVSyncLibManagerTest, LoadFunction001, Function | MediumTest| Level1)
{
    bool success = dvsyncLibManager.Initialize("libdvsync.z.so");
    if (!success) {
        return;
    }
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
    bool loadSuccess = true;
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongIsAppDVSyncOn", dvsyncLibManager.isAppDVSyncOnFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetAppRequestedStatus",
        dvsyncLibManager.setAppRequestedStatusFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongCheckVsyncReceivedAndGetRelTs",
        dvsyncLibManager.checkVsyncReceivedAndGetRelTsFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetOccurPeriod", dvsyncLibManager.getOccurPeriodFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetOccurRefreshRate",
        dvsyncLibManager.getOccurRefreshRateFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongRecordVSync", dvsyncLibManager.recordVSyncFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetConnectionApp", dvsyncLibManager.getConnectionAppFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongMarkRSRendering", dvsyncLibManager.markRSRenderingFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetAppDVSyncSwitch", dvsyncLibManager.setAppDVSyncSwitchFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetUiDVSyncConfig",
        dvsyncLibManager.setUiDVSyncConfigFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetUiCommandDelayTime",
        dvsyncLibManager.getUiCommandDelayTimeFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongUpdatePendingReferenceTime",
        dvsyncLibManager.updatePendingReferenceTimeFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetRealTimeOffsetOfDvsync",
        dvsyncLibManager.getRealTimeOffsetOfDvsyncFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetHardwareTaskNum", dvsyncLibManager.setHardwareTaskNumFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetPhysicalScreenNum",
        dvsyncLibManager.setPhysicalScreenNumFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetTaskEndWithTime", dvsyncLibManager.setTaskEndWithTimeFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongInitWithParam", dvsyncLibManager.initWithParamFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetDistributor", dvsyncLibManager.setDistributorFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetConnection", dvsyncLibManager.setConnectionFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongDisableVSync", dvsyncLibManager.disableVSyncFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongNeedSkipAndUpdateTs",
        dvsyncLibManager.needSkipAndUpdateTsFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongNeedSkipUi", dvsyncLibManager.needSkipUiFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongRecordEnableVsync", dvsyncLibManager.recordEnableVsyncFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongRecordRNV", dvsyncLibManager.recordRNVFunc_);
    ASSERT_NE(loadSuccess, true);
    DVSyncLibManagerTest::dvsyncLibManager.Shutdown();
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
}

/*
* Function: LoadFunction002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test LoadFunction when load func not exist.
 */
HWTEST_F(DVSyncLibManagerTest, LoadFunction002, Function | MediumTest| Level1)
{
    bool success = dvsyncLibManager.Initialize("libdvsync.z.so");
    if (!success) {
        return;
    }
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
    bool loadSuccess = true;
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongNeedPreexecuteAndUpdateTs",
        dvsyncLibManager.needPreexecuteAndUpdateTsFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongNotifyPackageEvent", dvsyncLibManager.notifyPackageEventFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongHandleTouchEvent", dvsyncLibManager.handleTouchEventFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetBufferInfo", dvsyncLibManager.setBufferInfoFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongIsAppRequested", dvsyncLibManager.isAppRequestedFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetVSyncConnectionApp",
        dvsyncLibManager.getVSyncConnectionAppFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongNeedUpdateVSyncTime",
        dvsyncLibManager.needUpdateVSyncTimeFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetLastUpdateTime", dvsyncLibManager.getLastUpdateTimeFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongDVSyncUpdate", dvsyncLibManager.dvsyncUpdateFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongForceRsDVsync", dvsyncLibManager.forceRsDVsyncFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongUpdateReferenceTimeAndPeriod",
        dvsyncLibManager.updateReferenceTimeAndPeriodFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetCurrentRefreshRate",
        dvsyncLibManager.setCurrentRefreshRateFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongDVSyncRateChanged", dvsyncLibManager.dvsyncRateChangedFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetToCurrentPeriod", dvsyncLibManager.setToCurrentPeriodFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongGetVsyncCount", dvsyncLibManager.getVsyncCountFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongInitDvsyncController",
        dvsyncLibManager.initDvsyncControllerFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetVSyncTimeUpdated",
        dvsyncLibManager.setVSyncTimeUpdatedFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongToDelay", dvsyncLibManager.toDelayFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongSetTouchEvent", dvsyncLibManager.setTouchEventFunc_);
    loadSuccess &= dvsyncLibManager.LoadFunction("WrongUpdateDelayInfo", dvsyncLibManager.updateDelayInfoFunc_);
    ASSERT_NE(loadSuccess, true);
    DVSyncLibManagerTest::dvsyncLibManager.Shutdown();
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
}

/*
* Function: LoadFunction003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. test LoadFunction when load func not exist.
 */
HWTEST_F(DVSyncLibManagerTest, LoadFunction003, Function | MediumTest| Level1)
{
    uint64_t timestamp = 0;
    DVSyncFeatureParam dvsyncParam;
    std::vector<std::string> packageList;
    int32_t touchStatus = 0;
    int32_t touchCnt = 0;
    std::string sceneId = "1";
    uint64_t id = 0;
    std::string name = "test";
    int32_t queueSize = 0;
    int32_t bufferCount = 0;
    int64_t lastConsumeTime = 0;
    int64_t VsyncCount = 0;
    int64_t offset = 0;
    uint32_t num = 0;
    int64_t time = 0;
    bool success = dvsyncLibManager.Initialize("libdvsync.z.so");
    if (!success) {
        return;
    }
    ASSERT_NE(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
    bool loadSuccess = dvsyncLibManager.LoadAllFunctions();
    ASSERT_NE(loadSuccess, false);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.CheckVsyncReceivedAndGetRelTs(timestamp), 0);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetOccurPeriod(), 0);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetOccurRefreshRate(), 0);
    DVSyncLibManagerTest::dvsyncLibManager.MarkRSRendering(true);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetUiCommandDelayTime(), 0);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.GetRealTimeOffsetOfDvsync(time), 0);
    DVSyncLibManagerTest::dvsyncLibManager.SetHardwareTaskNum(num);
    DVSyncLibManagerTest::dvsyncLibManager.SetPhysicalScreenNum(num);
    DVSyncLibManagerTest::dvsyncLibManager.SetTaskEndWithTime(timestamp);
    DVSyncLibManagerTest::dvsyncLibManager.InitWithParam(dvsyncParam);
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.SetDistributor(true, vsyncDistributor), true);
    DVSyncLibManagerTest::dvsyncLibManager.NotifyPackageEvent(packageList);
    DVSyncLibManagerTest::dvsyncLibManager.HandleTouchEvent(touchStatus, touchCnt);
    bool result = DVSyncLibManagerTest::dvsyncLibManager.SetBufferInfo(id, name, queueSize,
        bufferCount, lastConsumeTime);
    ASSERT_EQ(result, false);
    DVSyncLibManagerTest::dvsyncLibManager.ForceRsDVsync(sceneId);
    DVSyncLibManagerTest::dvsyncLibManager.GetVsyncCount(VsyncCount);
    DVSyncLibManagerTest::dvsyncLibManager.InitDvsyncController(vsyncGenerator, offset, vsyncController);
    DVSyncLibManagerTest::dvsyncLibManager.Shutdown();
    ASSERT_EQ(DVSyncLibManagerTest::dvsyncLibManager.libHandle_, nullptr);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
