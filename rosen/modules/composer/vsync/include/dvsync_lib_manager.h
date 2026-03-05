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

#ifndef DVSYNC_LIB_MANAGER_H
#define DVSYNC_LIB_MANAGER_H

#include <string>
#include <mutex>
#include "event_handler.h"
#include "vsync_generator.h"
#include "vsync_distributor.h"
namespace OHOS {
namespace Rosen {
using IsAppDVSyncOnFunc = bool (*)();
using SetAppRequestedStatusFunc = bool (*)(const sptr<VSyncConnection>& connection, bool isAppRequested);
using CheckVsyncReceivedAndGetRelTsFunc = uint64_t(*)(uint64_t timestamp);
using GetOccurPeriodFunc = int64_t (*)();
using GetOccurRefreshRateFunc = uint32_t (*)();
using RecordVSyncFunc = void (*)(const sptr<VSyncDistributor>& distributor, int64_t now, int64_t period,
    uint32_t refreshRate, bool isDVSyncController);
using GetConnectionAppFunc = void (*)(sptr<VSyncConnection>& connection);
using MarkRSRenderingFunc = void (*)(bool isRender);
using SetAppDVSyncSwitchFunc = void (*)(const sptr<VSyncConnection>& connection, bool start, bool isNative);
using SetUiDVSyncConfigFunc = void (*)(int32_t bufferCount, bool compositeSceneEnable, bool nativeDelayEnable,
    const std::vector<std::string>& rsDvsyncAnimationList);
using GetUiCommandDelayTimeFunc = int64_t (*)();
using UpdatePendingReferenceTimeFunc = void (*)(int64_t& timeStamp);
using GetRealTimeOffsetOfDvsyncFunc = uint64_t (*)(int64_t time);
using SetHardwareTaskNumFunc = void (*)(uint32_t num);
using SetPhysicalScreenNumFunc = void (*)(uint32_t num);
using SetTaskEndWithTimeFunc = void (*)(uint64_t time);
using InitWithParamFunc = void (*)(DVSyncFeatureParam dvsyncParam);
using SetDistributorFunc = bool (*)(bool isRs, const sptr<VSyncDistributor>& distributor);
using SetConnectionFunc = void (*)(bool isRs, const sptr<VSyncConnection>& connection);
using DisableVSyncFunc = void (*)(const sptr<VSyncDistributor>& distributor);
using NeedSkipAndUpdateTsFunc = bool (*)(const sptr<VSyncConnection>& connection, int64_t& timeStamp);
using NeedSkipUiFunc = bool (*)(sptr<VSyncConnection> connection);
using RecordEnableVsyncFunc = void (*)(const sptr<VSyncDistributor>& distributor);
using RecordRNVFunc = void (*)(const sptr<VSyncConnection>& connection, const std::string& fromWhom,
    VSyncMode vsyncMode, int64_t lastVSyncTS, int64_t requestVsyncTime);
using NeedPreexecuteAndUpdateTsFunc = bool (*)(const sptr<VSyncConnection>& connection, int64_t& timeStamp,
    int64_t& period);
using NotifyPackageEventFunc = void (*)(const std::vector<std::string>& packageList);
using HandleTouchEventFunc = void (*)(int32_t touchStatus, int32_t touchCnt);
using SetBufferInfoFunc = bool (*)(uint64_t id, const std::string& name, int32_t queueSize, int32_t bufferCount,
    int64_t lastConsumeTime);
using IsAppRequestedFunc = bool (*)();
using GetVSyncConnectionAppFunc = void (*)(sptr<VSyncConnection>& connection);
using NeedUpdateVSyncTimeFunc = bool (*)(int32_t& pid);
using GetLastUpdateTimeFunc = int64_t (*)();
using DVSyncUpdateFunc = void (*)(uint64_t dvsyncTime, uint64_t vsyncTime);
using ForceRsDVsyncFunc = void (*)(const std::string& sceneId);
using UpdateReferenceTimeAndPeriodFunc = void (*)(bool& isLtpoNeedChange, int64_t& occurDvsyncReferenceTime,
    int64_t& dvsyncPeriodRecord);
using SetCurrentRefreshRateFunc = int64_t (*)(uint32_t currRefreshRate, uint32_t lastRefreshRate);
using DVSyncRateChangedFunc = bool (*)(uint32_t currRefreshRate, uint32_t& dvsyncRate);
using SetToCurrentPeriodFunc = void (*)();
using GetVsyncCountFunc = int64_t (*)(int64_t& VsyncCount);
using SetCallbackFunc = void (*)(OHOS::Rosen::VSyncController::Callback *cb);
using SetEnableFunc = void (*)(bool enable, bool& isGeneratorEnable);
using InitDvsyncControllerFunc = void (*)(const sptr<VSyncGenerator>& gen, int64_t offset,
                                          sptr<VSyncController>& controller);
using SetVSyncTimeUpdatedFunc = void (*)();

//dvsync delay
using ToDelayFunc = void (*)(const AppExecFwk::InnerEvent::Callback& callback, const std::string& name, int32_t fd);
using SetTouchEventFunc = void (*)(int32_t touchType);
using UpdateDelayInfoFunc = void (*)();

class DVSyncLibManager {
public:
    DVSyncLibManager(const DVSyncLibManager&) = delete;
    DVSyncLibManager& operator=(const DVSyncLibManager&) = delete;
    static DVSyncLibManager& Instance();
    static DVSyncLibManager& DvsyncDelayInstance();
    bool Initialize(const std::string& libPath = "libdvsync.z.so", bool isDvsyncDelay = false);
    void Shutdown();
    bool IsInitialized() const { return initialized_; }
    bool AllFunctionsLoaded() const;

    bool IsAppDVSyncOn();
    bool SetAppRequestedStatus(const sptr<VSyncConnection>& connection, bool isAppRequested);
    uint64_t CheckVsyncReceivedAndGetRelTs(uint64_t timestamp);
    int64_t GetOccurPeriod();
    uint32_t GetOccurRefreshRate();
    void RecordVSync(const sptr<VSyncDistributor>& distributor, int64_t now, int64_t period,
        uint32_t refreshRate, bool isDVSyncController);
    void GetConnectionApp(sptr<VSyncConnection>& connection);
    void MarkRSRendering(bool isRender);
    void SetAppDVSyncSwitch(const sptr<VSyncConnection>& connection, bool start, bool isNative);
    void SetUiDVSyncConfig(int32_t bufferCount, bool compositeSceneEnable, bool nativeDelayEnable,
        const std::vector<std::string>& rsDvsyncAnimationList);
    int64_t GetUiCommandDelayTime();
    void UpdatePendingReferenceTime(int64_t& timeStamp);
    uint64_t GetRealTimeOffsetOfDvsync(int64_t time);
    void SetHardwareTaskNum(uint32_t num);
    void SetPhysicalScreenNum(uint32_t num);
    void SetTaskEndWithTime(uint64_t time);
    void InitWithParam(DVSyncFeatureParam dvsyncParam);
    bool SetDistributor(bool isRs, const sptr<VSyncDistributor>& distributor);
    void SetConnection(bool isRs, const sptr<VSyncConnection>& connection);
    void DisableVSync(const sptr<VSyncDistributor>& distributor);
    bool NeedSkipAndUpdateTs(const sptr<VSyncConnection>& connection, int64_t& timeStamp);
    bool NeedSkipUi(sptr<VSyncConnection> connection);
    void RecordEnableVsync(const sptr<VSyncDistributor>& distributor);
    void RecordRNV(const sptr<VSyncConnection>& connection, const std::string& fromWhom,
        VSyncMode vsyncMode, int64_t lastVSyncTS, int64_t requestVsyncTime);
    bool NeedPreexecuteAndUpdateTs(const sptr<VSyncConnection>& connection, int64_t& timeStamp, int64_t& period);
    void NotifyPackageEvent(const std::vector<std::string>& packageList);
    void HandleTouchEvent(int32_t touchStatus, int32_t touchCnt);
    bool SetBufferInfo(uint64_t id, const std::string& name, int32_t queueSize, int32_t bufferCount,
        int64_t lastConsumeTime);
    bool IsAppRequested();
    void GetVSyncConnectionApp(sptr<VSyncConnection>& connection);
    bool NeedUpdateVSyncTime(int32_t& pid);
    int64_t GetLastUpdateTime();
    void DVSyncUpdate(uint64_t dvsyncTime, uint64_t vsyncTime);
    void ForceRsDVsync(const std::string& sceneId);
    void UpdateReferenceTimeAndPeriod(bool& isLtpoNeedChange, int64_t& occurDvsyncReferenceTime,
        int64_t& dvsyncPeriodRecord);
    int64_t SetCurrentRefreshRate(uint32_t currRefreshRate, uint32_t lastRefreshRate);
    bool DVSyncRateChanged(uint32_t currRefreshRate, uint32_t& dvsyncRate);
    void SetToCurrentPeriod();
    int64_t GetVsyncCount(int64_t& VsyncCount);
    void InitDvsyncController(const sptr<VSyncGenerator>& gen, int64_t offset, sptr<VSyncController>& controller);
    void SetVSyncTimeUpdated();

//dvsync delay
    void ToDelay(const AppExecFwk::InnerEvent::Callback& callback, const std::string& name, int32_t fd);
    void SetTouchEvent(int32_t touchType);
    void UpdateDelayInfo();

private:
    DVSyncLibManager() = default;
    ~DVSyncLibManager();

    void* libHandle_ = nullptr;
    bool initialized_ = false;
    mutable std::mutex mutex_;
    IsAppDVSyncOnFunc isAppDVSyncOnFunc_ = nullptr;
    SetAppRequestedStatusFunc setAppRequestedStatusFunc_ = nullptr;
    CheckVsyncReceivedAndGetRelTsFunc checkVsyncReceivedAndGetRelTsFunc_ = nullptr;
    GetOccurPeriodFunc getOccurPeriodFunc_ = nullptr;
    GetOccurRefreshRateFunc getOccurRefreshRateFunc_ = nullptr;
    RecordVSyncFunc recordVSyncFunc_ = nullptr;
    GetConnectionAppFunc getConnectionAppFunc_ = nullptr;
    MarkRSRenderingFunc markRSRenderingFunc_ = nullptr;
    SetAppDVSyncSwitchFunc setAppDVSyncSwitchFunc_ = nullptr;
    SetUiDVSyncConfigFunc setUiDVSyncConfigFunc_ = nullptr;
    GetUiCommandDelayTimeFunc getUiCommandDelayTimeFunc_ = nullptr;
    UpdatePendingReferenceTimeFunc updatePendingReferenceTimeFunc_ = nullptr;
    GetRealTimeOffsetOfDvsyncFunc getRealTimeOffsetOfDvsyncFunc_ = nullptr;
    SetHardwareTaskNumFunc setHardwareTaskNumFunc_ = nullptr;
    SetPhysicalScreenNumFunc setPhysicalScreenNumFunc_ = nullptr;
    SetTaskEndWithTimeFunc setTaskEndWithTimeFunc_ = nullptr;
    InitWithParamFunc initWithParamFunc_ = nullptr;
    SetDistributorFunc setDistributorFunc_ = nullptr;
    SetConnectionFunc setConnectionFunc_ = nullptr;
    DisableVSyncFunc disableVSyncFunc_ = nullptr;
    NeedSkipAndUpdateTsFunc needSkipAndUpdateTsFunc_ = nullptr;
    NeedSkipUiFunc needSkipUiFunc_ = nullptr;
    RecordEnableVsyncFunc recordEnableVsyncFunc_ = nullptr;
    RecordRNVFunc recordRNVFunc_ = nullptr;
    NeedPreexecuteAndUpdateTsFunc needPreexecuteAndUpdateTsFunc_ = nullptr;
    NotifyPackageEventFunc notifyPackageEventFunc_ = nullptr;
    HandleTouchEventFunc handleTouchEventFunc_ = nullptr;
    SetBufferInfoFunc setBufferInfoFunc_ = nullptr;
    IsAppRequestedFunc isAppRequestedFunc_ = nullptr;
    GetVSyncConnectionAppFunc getVSyncConnectionAppFunc_ = nullptr;
    NeedUpdateVSyncTimeFunc needUpdateVSyncTimeFunc_ = nullptr;
    GetLastUpdateTimeFunc getLastUpdateTimeFunc_ = nullptr;
    DVSyncUpdateFunc dvsyncUpdateFunc_ = nullptr;
    ForceRsDVsyncFunc forceRsDVsyncFunc_ = nullptr;
    UpdateReferenceTimeAndPeriodFunc updateReferenceTimeAndPeriodFunc_ = nullptr;
    SetCurrentRefreshRateFunc setCurrentRefreshRateFunc_ = nullptr;
    DVSyncRateChangedFunc dvsyncRateChangedFunc_ = nullptr;
    SetToCurrentPeriodFunc setToCurrentPeriodFunc_ = nullptr;
    GetVsyncCountFunc getVsyncCountFunc_ = nullptr;
    InitDvsyncControllerFunc initDvsyncControllerFunc_ = nullptr;
    SetVSyncTimeUpdatedFunc setVSyncTimeUpdatedFunc_ = nullptr;

    //dvsync delay
    ToDelayFunc toDelayFunc_ = nullptr;
    SetTouchEventFunc setTouchEventFunc_ = nullptr;
    UpdateDelayInfoFunc updateDelayInfoFunc_ = nullptr;

    //inner func
    bool LoadAllFunctions();
    bool LoadDvsyncDelayFunctions();
    void ClearAllFunctions();
    void ClearDvsyncDelayFunctions();
    
    template<typename FuncPtr>
    bool LoadFunction(const std::string& funcName, FuncPtr& funcPtr)
    {
        void* symbol = dlsym(libHandle_, funcName.c_str());
        if (!symbol) {
            funcPtr = nullptr;
            return false;
        }
        funcPtr = reinterpret_cast<FuncPtr>(symbol);
        return true;
    }
};
}
}

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_LOG_H