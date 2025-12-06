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

#ifndef VSYNC_VSYNC_SAMPLER_H
#define VSYNC_VSYNC_SAMPLER_H

#include <cstdint>
#include <memory>
#include <mutex>

#include <event_handler.h>
#include <refbase.h>
#include "graphic_common.h"
#include <screen_manager/screen_types.h>

namespace OHOS {
namespace Rosen {
class VSyncSampler : public RefBase {
public:
    using SetScreenVsyncEnabledCallback = std::function<void(uint64_t, bool)>;
    using UpdateVsyncEnabledScreenIdCallback = std::function<bool(uint64_t)>;
    using JudgeVSyncEnabledScreenWhilePowerStatusChangedCallback = std::function<uint64_t(uint64_t, ScreenPowerStatus, uint64_t)>;
    using UpdateFoldScreenConnectStatusLockedCallback = std::function<void(uint64_t, bool)>;
    using SetScreenVsyncEnableByIdCallback = std::function<void(uint64_t, uint64_t, bool)>;
    using GetScreenVsyncEnableByIdCallback = std::function<uint64_t(uint64_t)>;
    VSyncSampler() = default;
    virtual ~VSyncSampler() noexcept = default;
    virtual bool AddSample(int64_t timestamp) = 0;
    virtual int64_t GetPeriod() const = 0;
    virtual int64_t GetPhase() const = 0;
    virtual int64_t GetRefrenceTime() const = 0;
    virtual bool AddPresentFenceTime(uint32_t screenId, int64_t timestamp) = 0;
    virtual void SetHardwareVSyncStatus(bool enabled) = 0;
    virtual void RegSetScreenVsyncEnabledCallback(SetScreenVsyncEnabledCallback cb) = 0;
    virtual void SetScreenVsyncEnabledInRSMainThread(uint64_t screenId, bool enabled) = 0;
    virtual int64_t GetHardwarePeriod() const = 0;
    virtual void SetPendingPeriod(int64_t period) = 0;
    virtual void Dump(std::string &result) = 0;
    virtual void ClearAllSamples() = 0;
    virtual void SetVsyncSamplerEnabled(bool enabled) = 0;
    virtual bool GetVsyncSamplerEnabled() = 0;
    virtual int32_t StartSample(bool forceReSample) = 0;
    virtual void SetVsyncEnabledScreenId(uint64_t vsyncEnabledScreenId) = 0;
    virtual uint64_t GetVsyncEnabledScreenId() = 0;
    virtual void SetAdaptive(bool isAdaptive) = 0;
    virtual void RecordDisplayVSyncStatus(bool enabled) = 0;
    virtual void RollbackHardwareVSyncStatus() = 0;
    virtual void RegSetScreenVsyncEnabledCallbackForRenderService(uint64_t vsyncEnabledScreenId,
        std::shared_ptr<AppExecFwk::EventHandler> handler) = 0;
    virtual void ProcessVSyncScreenIdWhilePowerStatusChanged(uint64_t id, ScreenPowerStatus status,
        std::shared_ptr<AppExecFwk::EventHandler> handler, bool isFold) = 0;
    virtual uint64_t JudgeVSyncEnabledScreenWhileHotPlug(uint64_t screenId, bool connected) = 0;
    virtual void RegUpdateVsyncEnabledScreenIdCallback(UpdateVsyncEnabledScreenIdCallback cb) = 0;
    virtual void RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback(JudgeVSyncEnabledScreenWhilePowerStatusChangedCallback cb) = 0;
    virtual void RegUpdateFoldScreenConnectStatusLockedCallback(UpdateFoldScreenConnectStatusLockedCallback cb) = 0;
    virtual void RegSetScreenVsyncEnableByIdCallback(SetScreenVsyncEnableByIdCallback cb) = 0;
    virtual void RegGetScreenVsyncEnableByIdCallback(GetScreenVsyncEnableByIdCallback cb) = 0;
protected:
    SetScreenVsyncEnabledCallback setScreenVsyncEnabledCallback_ = nullptr;
};

sptr<VSyncSampler> CreateVSyncSampler();

namespace impl {
class VSyncSampler : public OHOS::Rosen::VSyncSampler {
public:
    static sptr<OHOS::Rosen::VSyncSampler> GetInstance() noexcept;

    // nocopyable
    VSyncSampler(const VSyncSampler &) = delete;
    VSyncSampler &operator=(const VSyncSampler &) = delete;
    virtual bool AddSample(int64_t timestamp) override;
    virtual int64_t GetPeriod() const override;
    virtual int64_t GetPhase() const override;
    virtual int64_t GetRefrenceTime() const override;
    virtual bool AddPresentFenceTime(uint32_t screenId, int64_t timestamp) override;
    virtual void SetHardwareVSyncStatus(bool enabled) override;
    virtual void RegSetScreenVsyncEnabledCallback(SetScreenVsyncEnabledCallback cb) override;
    virtual void SetScreenVsyncEnabledInRSMainThread(uint64_t screenId, bool enabled) override;
    virtual int64_t GetHardwarePeriod() const override;
    virtual void SetPendingPeriod(int64_t period) override;
    virtual void Dump(std::string &result) override;
    virtual void ClearAllSamples() override;
    virtual void SetVsyncSamplerEnabled(bool enabled) override;
    virtual bool GetVsyncSamplerEnabled() override;
    virtual int32_t StartSample(bool forceReSample) override;
    virtual void SetVsyncEnabledScreenId(uint64_t vsyncEnabledScreenId) override;
    virtual uint64_t GetVsyncEnabledScreenId() override;
    virtual void SetAdaptive(bool isAdaptive) override;
    virtual void RecordDisplayVSyncStatus(bool enabled) override;
    virtual void RollbackHardwareVSyncStatus() override;
    virtual void RegSetScreenVsyncEnabledCallbackForRenderService(uint64_t vsyncEnabledScreenId,
        std::shared_ptr<AppExecFwk::EventHandler> handler) override;
    virtual void ProcessVSyncScreenIdWhilePowerStatusChanged(uint64_t id, ScreenPowerStatus status,
        std::shared_ptr<AppExecFwk::EventHandler> handler, bool isFold) override;
    virtual uint64_t JudgeVSyncEnabledScreenWhileHotPlug(uint64_t screenId, bool connected) override;
    virtual void RegUpdateVsyncEnabledScreenIdCallback(UpdateVsyncEnabledScreenIdCallback cb) override;
    virtual void RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback(JudgeVSyncEnabledScreenWhilePowerStatusChangedCallback cb) override;
    virtual void RegUpdateFoldScreenConnectStatusLockedCallback(UpdateFoldScreenConnectStatusLockedCallback cb) override;
    virtual void RegSetScreenVsyncEnableByIdCallback(SetScreenVsyncEnableByIdCallback cb) override;
    virtual void RegGetScreenVsyncEnableByIdCallback(GetScreenVsyncEnableByIdCallback cb) override;

private:
    friend class OHOS::Rosen::VSyncSampler;
    enum : uint32_t { MAX_SAMPLES = 32 };
    enum : uint32_t { MIN_SAMPLES_FOR_UPDATE = 6 };
    enum : uint32_t { MAX_SAMPLES_WITHOUT_PRESENT = 4 };
    enum : uint32_t { NUM_PRESENT = 8 };

    VSyncSampler();
    ~VSyncSampler() override;

    void UpdateModeLocked();
    void UpdateErrorLocked();
    void ResetErrorLocked();
    void UpdateReferenceTimeLocked();
    void CheckIfFirstRefreshAfterIdleLocked();
    void ComputePhaseLocked();
    void SetScreenVsyncEnabledInRSMainThreadLocked(uint64_t screenId, bool enabled);

    int64_t period_;
    int64_t phase_;
    int64_t referenceTime_;
    double error_;
    int64_t samples_[MAX_SAMPLES] = {0};
    int64_t presentFenceTime_[NUM_PRESENT] = {-1};
    uint32_t firstSampleIndex_;
    uint32_t numSamples_;
    bool modeUpdated_;
    uint32_t numResyncSamplesSincePresent_ = 0;
    uint32_t presentFenceTimeOffset_ = 0;

    mutable std::mutex mutex_;

    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::VSyncSampler> instance_;
    bool hardwareVSyncStatus_ = true;
    bool displayVSyncStatus_ = false;
    int64_t pendingPeriod_ = 0;
    std::atomic<bool> enableVsyncSample_ = true;
    uint64_t vsyncEnabledScreenId_ = UINT64_MAX;
    bool isAdaptive_ = false;
    int64_t lastAdaptiveTime_ = 0;
    UpdateVsyncEnabledScreenIdCallback updateVsyncEnabledScreenIdCallback_;
    JudgeVSyncEnabledScreenWhilePowerStatusChangedCallback judgeVSyncEnabledScreenWhilePowerStatusChangedCallback_;
    UpdateFoldScreenConnectStatusLockedCallback updateFoldScreenConnectStatusLockedCallback_;
    SetScreenVsyncEnableByIdCallback setScreenVsyncEnableByIdCallback_;
    GetScreenVsyncEnableByIdCallback getScreenVsyncEnableByIdCallback_;
};
} // impl
} // namespace Rosen
} // namespace OHOS

#endif // VSYNC_VSYNC_SAMPLER_H
