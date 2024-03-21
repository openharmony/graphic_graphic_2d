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


#ifndef VSYNC_VSYNC_GENERATOR_H
#define VSYNC_VSYNC_GENERATOR_H

#include <cstdint>
#include <refbase.h>
#include "graphic_common.h"

#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include "vsync_type.h"
#include "vsync_system_ability_listener.h"

namespace OHOS {
namespace Rosen {
class VSyncGenerator : public RefBase {
public:
    class Callback : public RefBase {
    public:
        virtual void OnVSyncEvent(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode) = 0;
        virtual void OnPhaseOffsetChanged(int64_t phaseOffset) = 0;
        /* std::pair<id, refresh rate> */
        virtual void OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates) = 0;
    };
    struct ListenerRefreshRateData {
        sptr<OHOS::Rosen::VSyncGenerator::Callback> cb = nullptr;
        // id, refreshRate
        std::vector<std::pair<uint64_t, uint32_t>> refreshRates;
    };
    struct ListenerPhaseOffsetData {
        sptr<OHOS::Rosen::VSyncGenerator::Callback> cb = nullptr;
        int32_t phaseByPulseNum = 0;
    };
    VSyncGenerator() = default;
    virtual ~VSyncGenerator() noexcept = default;
    virtual VsyncError UpdateMode(int64_t period, int64_t phase, int64_t referenceTime) = 0;
    virtual VsyncError AddListener(int64_t phase, const sptr<Callback>& cb) = 0;
    virtual VsyncError RemoveListener(const sptr<Callback>& cb) = 0;
    virtual VsyncError ChangePhaseOffset(const sptr<Callback>& cb, int64_t offset) = 0;
    virtual bool IsEnable() = 0;
    virtual VsyncError ChangeGeneratorRefreshRateModel(const ListenerRefreshRateData &listenerRefreshRates,
                                                       const ListenerPhaseOffsetData &listenerPhaseOffset,
                                                       uint32_t generatorRefreshRate) = 0;
    virtual int64_t GetVSyncPulse() = 0;
    virtual VsyncError SetVSyncMode(VSyncMode vsyncMode) = 0;
    virtual VSyncMode GetVSyncMode() = 0;
    virtual VsyncError SetVSyncPhaseByPulseNum(int32_t phaseByPulseNum) = 0;
    virtual void Dump(std::string &result) = 0;
    virtual bool GetFrameRateChaingStatus() = 0;
    virtual VsyncError SetReferenceTimeOffset(int32_t phaseByPulseNum) = 0;
    virtual VsyncError CheckAndUpdateRefereceTime(int64_t hardwareVsyncInterval, int64_t referenceTime) = 0;
    virtual void SetPendingMode(int64_t period, int64_t timestamp) = 0;
    virtual VsyncError StartRefresh() = 0;
};

sptr<VSyncGenerator> CreateVSyncGenerator();
void DestroyVSyncGenerator();

namespace impl {
class VSyncGenerator : public OHOS::Rosen::VSyncGenerator {
public:
    static sptr<OHOS::Rosen::VSyncGenerator> GetInstance() noexcept;
    static void DeleteInstance() noexcept;

    // nocopyable
    VSyncGenerator(const VSyncGenerator &) = delete;
    VSyncGenerator &operator=(const VSyncGenerator &) = delete;
    VsyncError UpdateMode(int64_t period, int64_t phase, int64_t referenceTime) override;
    VsyncError AddListener(int64_t phase, const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb) override;
    VsyncError RemoveListener(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb) override;
    VsyncError ChangePhaseOffset(const sptr<OHOS::Rosen::VSyncGenerator::Callback>& cb, int64_t offset) override;
    bool IsEnable() override;
    VsyncError ChangeGeneratorRefreshRateModel(const ListenerRefreshRateData &listenerRefreshRates,
                                               const ListenerPhaseOffsetData &listenerPhaseOffset,
                                               uint32_t generatorRefreshRate) override;
    int64_t GetVSyncPulse() override;
    VsyncError SetVSyncMode(VSyncMode vsyncMode) override;
    VSyncMode GetVSyncMode() override;
    VsyncError SetVSyncPhaseByPulseNum(int32_t phaseByPulseNum) override;
    void Dump(std::string &result) override;
    bool GetFrameRateChaingStatus() override;
    VsyncError SetReferenceTimeOffset(int32_t phaseByPulseNum) override;
    VsyncError CheckAndUpdateRefereceTime(int64_t hardwareVsyncInterval, int64_t referenceTime) override;
    void SetPendingMode(int64_t period, int64_t timestamp) override;
    VsyncError StartRefresh() override;

private:
    friend class OHOS::Rosen::VSyncGenerator;

    struct Listener {
        int64_t phase_;
        sptr<OHOS::Rosen::VSyncGenerator::Callback> callback_;
        int64_t lastTime_;
        int64_t lastTimeRecord_;
    };

    VSyncGenerator();
    ~VSyncGenerator() override;

    int64_t ComputeNextVSyncTimeStamp(int64_t now, int64_t referenceTime);
    std::vector<Listener> GetListenerTimeouted(int64_t now, int64_t occurTimestamp, int64_t referenceTime);
    int64_t ComputeListenerNextVSyncTimeStamp(const Listener &listen, int64_t now, int64_t referenceTime);
    void ThreadLoop();
    void UpdateWakeupDelay(int64_t occurTimestamp, int64_t nextTimeStamp);
    bool ChangeListenerOffsetInternal();
    bool ChangeListenerRefreshRatesInternal();
    uint32_t JudgeRefreshRateLocked(int64_t period);
    bool CheckTimingCorrect(int64_t now, int64_t referenceTime, int64_t nextVSyncTime);
    bool UpdateChangeDataLocked(int64_t now, int64_t referenceTime, int64_t nextVSyncTime);
    void UpdateVSyncModeLocked();
    std::vector<Listener> GetListenerTimeoutedLTPO(int64_t now, int64_t referenceTime);
    void ListenerVsyncEventCB(int64_t occurTimestamp, int64_t nextTimeStamp,
        int64_t occurReferenceTime, bool isWakeup);
    VsyncError UpdatePeriodLocked(int64_t period);
    VsyncError UpdateReferenceTimeLocked(int64_t referenceTime);
#ifdef COMPOSER_SCHED_ENABLE
    void SubScribeSystemAbility();
#endif

    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
    int64_t period_;
    int64_t phase_;
    int64_t referenceTime_;
    int64_t wakeupDelay_;

    std::vector<Listener> listeners_;

    std::mutex mutex_;
    std::condition_variable con_;
    std::mutex waitForTimeoutMtx_;
    std::condition_variable waitForTimeoutCon_;
    std::thread thread_;
    bool vsyncThreadRunning_;
    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::VSyncGenerator> instance_;
    int64_t pulse_; // by ns
    uint32_t currRefreshRate_; // by Hz
    int32_t referenceTimeOffsetPulseNum_;
    int32_t defaultReferenceTimeOffsetPulseNum_;
    ListenerRefreshRateData changingRefreshRates_ = {};
    ListenerPhaseOffsetData changingPhaseOffset_ = {};
    uint32_t changingGeneratorRefreshRate_ = 0;
    bool needChangeRefreshRates_ = false;
    bool needChangePhaseOffset_ = false;
    bool needChangeGeneratorRefreshRate_ = false;
    VSyncMode vsyncMode_ = VSYNC_MODE_LTPS; //default LTPS
    VSyncMode pendingVsyncMode_ = VSYNC_MODE_INVALID;
    std::vector<Listener> listenersRecord_;
    bool refreshRateIsChanged_ = false;
    bool frameRateChanging_ = false;
    int64_t pendingPeriod_ = 0;
    int64_t pendingReferenceTime_ = 0;
    bool startRefresh_ = false;
    int64_t phaseRecord_ = 0;
    int64_t periodRecord_ = 0;
};
} // impl
} // namespace Rosen
} // namespace OHOS

#endif
