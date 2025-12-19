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

#include "dvsync_lib_manager.h"
#include <dlfcn.h>
#include "vsync_log.h"

namespace OHOS {
namespace Rosen {
DVSyncLibManager& DVSyncLibManager::Instance()
{
    static DVSyncLibManager instance;
    static std::once_flag createFlag;
    std::call_once(createFlag, []() {
        instance.Initialize("libdvsync.z.so");
    });
    return instance;
}

DVSyncLibManager::~DVSyncLibManager()
{
    Shutdown();
}

bool DVSyncLibManager::Initialize(const std::string& libPath)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) {
        return true;
    }
    libHandle_ = dlopen(libPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!libHandle_) {
        VLOGE("Failed to load library %{public}s.", libPath.c_str());
        return false;
    }
    if (!LoadAllFunctions()) {
        dlclose(libHandle_);
        libHandle_ = nullptr;
        return false;
    }
    initialized_ = true;
    return true;
}

void DVSyncLibManager::Shutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (libHandle_) {
        dlclose(libHandle_);
        libHandle_ = nullptr;
    }
}

bool DVSyncLibManager::LoadAllFunctions() {
    // load dvsync
    LoadFunction("IsAppDVSyncOn", isAppDVSyncOnFunc_);
    LoadFunction("SetAppRequestedStatus", setAppRequestedStatusFunc_);
    LoadFunction("CheckVsyncReceivedAndGetRelTs", checkVsyncReceivedAndGetRelTsFunc_);
    LoadFunction("GetOccurPeriod", getOccurPeriodFunc_);
    LoadFunction("GetOccurRefreshRate", getOccurRefreshRateFunc_);
    LoadFunction("RecordVSync", recordVSyncFunc_);
    LoadFunction("GetConnectionApp", getConnectionAppFunc_);
    LoadFunction("MarkRSRendering", markRSRenderingFunc_);
    LoadFunction("SetAppDVSyncSwitch", setAppDVSyncSwitchFunc_);
    LoadFunction("SetUiDVSyncConfig", setUiDVSyncConfigFunc_);
    LoadFunction("GetUiCommandDelayTime", getUiCommandDelayTimeFunc_);
    LoadFunction("UpdatePendingReferenceTime", updatePendingReferenceTimeFunc_);
    LoadFunction("GetRealTimeOffsetOfDvsync", getRealTimeOffsetOfDvsyncFunc_);
    LoadFunction("SetHardwareTaskNum", setHardwareTaskNumFunc_);
    LoadFunction("SetTaskEndWithTime", setTaskEndWithTimeFunc_);
    LoadFunction("InitWithParam", initWithParamFunc_);
    LoadFunction("SetDistributor", setDistributorFunc_);
    LoadFunction("SetConnection", setConnectionFunc_);
    LoadFunction("DisableVSync", disableVSyncFunc_);
    LoadFunction("NeedSkipAndUpdateTs", needSkipAndUpdateTsFunc_);
    LoadFunction("NeedSkipUi", needSkipUiFunc_);
    LoadFunction("RecordEnableVsync", recordEnableVsyncFunc_);
    LoadFunction("RecordRNV", recordRNVFunc_);
    LoadFunction("NeedPreexecuteAndUpdateTs", needPreexecuteAndUpdateTsFunc_);
    LoadFunction("NotifyPackageEvent", notifyPackageEventFunc_);
    LoadFunction("HandleTouchEvent", handleTouchEventFunc_);
    LoadFunction("SetBufferInfo", setBufferInfoFunc_);
    LoadFunction("IsAppRequested", isAppRequestedFunc_);
    LoadFunction("GetVSyncConnectionApp", getVSyncConnectionAppFunc_);
    LoadFunction("NeedUpdateVSyncTime", needUpdateVSyncTimeFunc_);
    LoadFunction("GetLastUpdateTime", getLastUpdateTimeFunc_);
    LoadFunction("DVSyncUpdate", dvsyncUpdateFunc_);
    LoadFunction("ForceRsDVsync", forceRsDVsyncFunc_);
    LoadFunction("UpdateReferenceTimeAndPeriod", updateReferenceTimeAndPeriodFunc_);
    LoadFunction("SetCurrentRefreshRate", setCurrentRefreshRateFunc_);
    LoadFunction("DVSyncRateChanged", dvsyncRateChangedFunc_);
    LoadFunction("SetToCurrentPeriod", setToCurrentPeriodFunc_);
    LoadFunction("GetVsyncCount", getVsyncCountFunc_);
    LoadFunction("InitDvsyncController", initDvsyncControllerFunc_);
    LoadFunction("SetVSyncTimeUpdated", setVSyncTimeUpdatedFunc_);
    //load dvsync delay
    LoadFunction("ToDelay", toDelayFunc_);
    LoadFunction("SetTouchEvent", setTouchEventFunc_);
    LoadFunction("UpdateDelayInfo", updateDelayInfoFunc_);
    return true;
}

template<typename FuncPtr> 
bool DVSyncLibManager::LoadFunction(const std::string& funcName, FuncPtr& funcPtr) {
    void* symbol = dlsym(libHandle_, funcName.c_str());
    if (!symbol) {
        VLOGE("Failed to load function %{public}s", funcName.c_str());
        funcPtr = nullptr;
        return false;
    }
    funcPtr = reinterpret_cast<FuncPtr>(symbol);
    return true;
}

void DVSyncLibManager::ClearAllFunctions()
{
    
}

bool DVSyncLibManager::IsAppDVSyncOn()
{
    if (isAppDVSyncOnFunc_ == nullptr) {
        return false;
    }
    return isAppDVSyncOnFunc_();
}

bool DVSyncLibManager::SetAppRequestedStatus(const sptr<VSyncConnection>& connection, bool isAppRequested)
{
    if (setAppRequestedStatusFunc_ == nullptr) {
        return false;
    }
    return setAppRequestedStatusFunc_(connection, isAppRequested);
}

uint64_t DVSyncLibManager::CheckVsyncReceivedAndGetRelTs(uint64_t timestamp)
{
    if (checkVsyncReceivedAndGetRelTsFunc_ == nullptr) {
        return timestamp;
    }
    return checkVsyncReceivedAndGetRelTsFunc_(timestamp);
}

int64_t DVSyncLibManager::GetOccurPeriod()
{
    if (getOccurPeriodFunc_ == nullptr) {
        return 0;
    }
    return getOccurPeriodFunc_();
}

uint32_t DVSyncLibManager::GetOccurRefreshRate()
{
    if (getOccurRefreshRateFunc_ == nullptr) {
        return 0;
    }
    return getOccurRefreshRateFunc_();
}

void DVSyncLibManager::RecordVSync(const sptr<VSyncDistributor>& distributor, int64_t now, int64_t period,
    uint32_t refreshRate, bool isDVSyncController)
{
    if (recordVSyncFunc_ == nullptr) {
        return;
    }
    recordVSyncFunc_(distributor, now, period, refreshRate, isDVSyncController);
}

void DVSyncLibManager::GetConnectionApp(sptr<VSyncConnection>& connection)
{
    if (getConnectionAppFunc_ == nullptr) {
        return;
    }
    getConnectionAppFunc_(connection);
}

void DVSyncLibManager::MarkRSRendering(bool isRender)
{
    if (markRSRenderingFunc_ == nullptr) {
        return;
    }
    markRSRenderingFunc_(isRender);
}

void DVSyncLibManager::SetAppDVSyncSwitch(const sptr<VSyncConnection>& connection, bool start, bool isNative)
{
    if (setAppDVSyncSwitchFunc_ == nullptr) {
        return;
    }
    setAppDVSyncSwitchFunc_(connection, start, isNative);
}

void DVSyncLibManager::SetUiDVSyncConfig(int32_t bufferCount, bool compositeSceneEnable, bool nativeDelayEnable,
    const std::vector<std::string>& rsDvsyncAnimationList)
{
    if (setUiDVSyncConfigFunc_ == nullptr) {
        return;
    }
    setUiDVSyncConfigFunc_(bufferCount, compositeSceneEnable, nativeDelayEnable, rsDvsyncAnimationList);
}

int64_t DVSyncLibManager::GetUiCommandDelayTime()
{
    if (getUiCommandDelayTimeFunc_ == nullptr) {
        return 0;
    }
    return getUiCommandDelayTimeFunc_();
}

void DVSyncLibManager::UpdatePendingReferenceTime(int64_t& timeStamp)
{
    if (updatePendingReferenceTimeFunc_ == nullptr) {
        return;
    }
    updatePendingReferenceTimeFunc_(timeStamp);
}

uint64_t DVSyncLibManager::GetRealTimeOffsetOfDvsync(int64_t time)
{
    if (getRealTimeOffsetOfDvsyncFunc_ == nullptr) {
        return 0;
    }
    return getRealTimeOffsetOfDvsyncFunc_(time);
}

void DVSyncLibManager::SetHardwareTaskNum(uint32_t num)
{
    if (setHardwareTaskNumFunc_ == nullptr) {
        return;
    }
    setHardwareTaskNumFunc_(num);
}

void DVSyncLibManager::SetTaskEndWithTime(uint64_t time)
{
    if (setTaskEndWithTimeFunc_ == nullptr) {
        return;
    }
    setTaskEndWithTimeFunc_(time);
}

void DVSyncLibManager::InitWithParam(DVSyncFeatureParam dvsyncParam)
{
    if (initWithParamFunc_ == nullptr) {
        return;
    }
    initWithParamFunc_(dvsyncParam);
}

bool DVSyncLibManager::SetDistributor(bool isRs, const sptr<VSyncDistributor>& distributor)
{
    if (setDistributorFunc_ == nullptr) {
        return false;
    }
    return setDistributorFunc_(isRs, distributor);
}

void DVSyncLibManager::SetConnection(bool isRs, const sptr<VSyncConnection>& connection)
{
    if (setConnectionFunc_ == nullptr) {
        return;
    }
    setConnectionFunc_(isRs, connection);
}

void DVSyncLibManager::DisableVSync(const sptr<VSyncDistributor>& distributor)
{
    if (disableVSyncFunc_ == nullptr) {
        return;
    }
    disableVSyncFunc_(distributor);
}

bool DVSyncLibManager::NeedSkipAndUpdateTs(const sptr<VSyncConnection>& connection, int64_t& timeStamp)
{
    if (needSkipAndUpdateTsFunc_ == nullptr) {
        return false;
    }
    return needSkipAndUpdateTsFunc_(connection, timeStamp);
}

bool DVSyncLibManager::NeedSkipUi(sptr<VSyncConnection> connection)
{
    if (needSkipUiFunc_ == nullptr) {
        return false;
    }
    return needSkipUiFunc_(connection);
}

void DVSyncLibManager::RecordEnableVsync(const sptr<VSyncDistributor>& distributor)
{
    if (recordEnableVsyncFunc_ == nullptr) {
        return;
    }
    recordEnableVsyncFunc_(distributor);
}

void DVSyncLibManager::RecordRNV(const sptr<VSyncConnection>& connection, const std::string& fromWhom,
    VSyncMode vsyncMode, int64_t lastVSyncTS, int64_t requestVsyncTime)
{
    if (recordRNVFunc_ == nullptr) {
        return;
    }
    recordRNVFunc_(connection, fromWhom, vsyncMode, lastVSyncTS, requestVsyncTime);
}

bool DVSyncLibManager::NeedPreexecuteAndUpdateTs(const sptr<VSyncConnection>& connection, int64_t& timeStamp,
    int64_t& period)
{
    if (needPreexecuteAndUpdateTsFunc_ == nullptr) {
        return false;
    }
    return needPreexecuteAndUpdateTsFunc_(connection, timeStamp, period);
}

void DVSyncLibManager::NotifyPackageEvent(const std::vector<std::string>& packageList)
{
    if (notifyPackageEventFunc_ == nullptr) {
        return;
    }
    notifyPackageEventFunc_(packageList);
}

void DVSyncLibManager::HandleTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    if (handleTouchEventFunc_ == nullptr) {
        return;
    }
    handleTouchEventFunc_(touchStatus, touchCnt);
}

bool DVSyncLibManager::SetBufferInfo(uint64_t id, const std::string& name, int32_t queueSize, int32_t bufferCount,
    int64_t lastConsumeTime)
{
    if (setBufferInfoFunc_ == nullptr) {
        return false;
    }
    return setBufferInfoFunc_(id, name, queueSize, bufferCount, lastConsumeTime);
}

bool DVSyncLibManager::IsAppRequested()
{
    if (isAppRequestedFunc_ == nullptr) {
        return false;
    }
    return isAppRequestedFunc_();
}

void DVSyncLibManager::GetVSyncConnectionApp(sptr<VSyncConnection>& connection)
{
    if (getVSyncConnectionAppFunc_ == nullptr) {
        return;
    }
    getVSyncConnectionAppFunc_(connection);
}

bool DVSyncLibManager::NeedUpdateVSyncTime(int32_t& pid)
{
    if (needUpdateVSyncTimeFunc_ == nullptr) {
        return false;
    }
    return needUpdateVSyncTimeFunc_(pid);
}

int64_t DVSyncLibManager::GetLastUpdateTime()
{
    if (getLastUpdateTimeFunc_ == nullptr) {
        return 0;
    }
    return getLastUpdateTimeFunc_();
}

void DVSyncLibManager::DVSyncUpdate(uint64_t dvsyncTime, uint64_t vsyncTime)
{
    if (dvsyncUpdateFunc_ == nullptr) {
        return;
    }
    dvsyncUpdateFunc_(dvsyncTime, vsyncTime);
}

void DVSyncLibManager::ForceRsDVsync(const std::string& sceneId)
{
    if (forceRsDVsyncFunc_ == nullptr) {
        return;
    }
    forceRsDVsyncFunc_(sceneId);
}

void DVSyncLibManager::UpdateReferenceTimeAndPeriod(bool& isLtpoNeedChange, int64_t& occurDvsyncReferenceTime,
    int64_t& dvsyncPeriodRecord)
{
    if (updateReferenceTimeAndPeriodFunc_ == nullptr) {
        return ;
    }
    updateReferenceTimeAndPeriodFunc_(isLtpoNeedChange, occurDvsyncReferenceTime, dvsyncPeriodRecord);
}

int64_t DVSyncLibManager::SetCurrentRefreshRate(uint32_t currRefreshRate, uint32_t lastRefreshRate)
{
    if (setCurrentRefreshRateFunc_ == nullptr) {
        return 0;
    }
    return setCurrentRefreshRateFunc_(currRefreshRate, lastRefreshRate);
}

bool DVSyncLibManager::DVSyncRateChanged(uint32_t currRefreshRate, uint32_t& dvsyncRate)
{
    if (dvsyncRateChangedFunc_ == nullptr) {
        return false;
    }
    return dvsyncRateChangedFunc_(currRefreshRate, dvsyncRate);
}

void DVSyncLibManager::SetToCurrentPeriod()
{
    if (setToCurrentPeriodFunc_ == nullptr) {
        return;
    }
    setToCurrentPeriodFunc_();
}

int64_t DVSyncLibManager::GetVsyncCount(int64_t& VsyncCount)
{
    if (getVsyncCountFunc_ == nullptr) {
        VsyncCount++;
        return VsyncCount;
    }
    return getVsyncCountFunc_(VsyncCount);
}

void DVSyncLibManager::InitDvsyncController(const sptr<VSyncGenerator>& gen, int64_t offset,
    sptr<VSyncController>& controller)
{
    if (initDvsyncControllerFunc_ == nullptr) {
        return;
    }
    initDvsyncControllerFunc_(gen, offset, controller);
}

void DVSyncLibManager::SetVSyncTimeUpdated()
{
    if (setVSyncTimeUpdatedFunc_ == nullptr) {
        return;
    }
    setVSyncTimeUpdatedFunc_();
}

//dvsync delay
void DVSyncLibManager::ToDelay(const AppExecFwk::InnerEvent::Callback& callback, const std::string& name,
    int32_t fd)
{
    if (toDelayFunc_ == nullptr) {
        return;
    }
    toDelayFunc_(callback, name, fd);
}

void DVSyncLibManager::SetTouchEvent(int32_t touchType)
{
    if (setTouchEventFunc_ == nullptr) {
        return;
    }
    setTouchEventFunc_(touchType);
}

void DVSyncLibManager::UpdateDelayInfo()
{
    if (updateDelayInfoFunc_ == nullptr) {
        return;
    }
    updateDelayInfoFunc_();
}
}
}
