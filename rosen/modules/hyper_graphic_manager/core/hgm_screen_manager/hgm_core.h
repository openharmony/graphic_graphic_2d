/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HGM_CORE_H
#define HGM_CORE_H

#include <cinttypes>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "hgm_frame_rate_manager.h"
#include "hgm_screen.h"
#include "hgm_task_handle_thread.h"
#include "vsync_generator.h"
#include "xml_parser.h"

namespace OHOS::Rosen {
class HgmFrameRateManager;
class RSScreenManager;

class HgmCore final {
public:
    using RefreshRateModeChangeCallback = std::function<void(int32_t)>;
    using RefreshRateUpdateCallback = std::function<void(int32_t)>;

    static void SysModeChangeProcess(const char* key, const char* value, void* context);
    static HgmCore& Instance();

    std::vector<ScreenId> GetScreenIds() const { return screenIds_; }
    int32_t GetScreenListSize() const { return screenList_.size(); }
    ScreenId GetActiveScreenId() const;

    const std::shared_ptr<PolicyConfigData>& GetPolicyConfigData() const { return mPolicyConfigData_; }
    std::shared_ptr<PolicyConfigVisitor> GetPolicyConfigVisitor() const { return mPolicyConfigVisitor_; }

    // called by RenderService
    uint32_t GetPendingScreenRefreshRate() const { return pendingScreenRefreshRate_.load(); }
    uint64_t GetPendingConstraintRelativeTime() const { return pendingConstraintRelativeTime_.load(); }
    uint64_t GetCurrentTimestamp() const { return timestamp_.load(); }
    void SetPendingScreenRefreshRate(uint32_t rate) { pendingScreenRefreshRate_.store(rate); }
    void SetPendingConstraintRelativeTime(uint64_t relativeTime) { pendingConstraintRelativeTime_.store(relativeTime); }
    void SetTimestamp(uint64_t timestamp) { timestamp_.store(timestamp); }
    bool SetHgmTaskFlag(bool value) { return postHgmTaskFlag_.exchange(value); }

    // called by Composer
    uint32_t GetScreenRefreshRateImme() { return screenRefreshRateImme_.exchange(0); } // 0 means disenable
    // called by HgmThread, the rate takes effect at the latest hardware timing
    void SetScreenRefreshRateImme(uint32_t rate) { screenRefreshRateImme_.store(rate); }

    bool GetLtpoEnabled() const { return isLtpoMode_.load(); }
    bool GetAdaptiveSyncEnabled() const { return GetLtpoEnabled() && adaptiveSync_ == ADAPTIVE_SYNC_ENABLED; }

    // called by RSHardwareTHread
    bool IsVBlankIdleCorrectEnabled() const { return vBlankIdleCorrectSwitch_.load(); }
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> GetModesToApply();

    bool IsLowRateToHighQuickEnabled() const { return lowRateToHighQuickSwitch_.load(); }
    bool IsLTPOSwitchOn() const { return ltpoEnabled_; }
    uint32_t GetAlignRate() const { return alignRate_; }

    int64_t GetIdealPipelineOffset() const { return idealPipelineOffset_; }
    int64_t GetIdealPipelineOffset144() const { return idealPipelineOffset144_; }
    void SetIdealPipelineOffset(int32_t pipelineOffsetPulseNum);
    void SetIdealPipelineOffset144(int32_t pipelineOffsetPulseNum)
    {
        idealPipelineOffset144_ = pipelineOffsetPulseNum * IDEAL_PULSE144;
    }

    int64_t GetPipelineOffset() const { return pipelineOffsetPulseNum_ * CreateVSyncGenerator()->GetVSyncPulse(); }
    int32_t GetPipelineOffsetPulseNum() const { return pipelineOffsetPulseNum_; }

    uint32_t GetSupportedMaxTE() const { return maxTE_; }
    uint32_t GetSupportedMaxTE144() const { return maxTE144_; }
    void SetSupportedMaxTE(uint32_t maxTE) { maxTE_ = maxTE; }
    void SetSupportedMaxTE144(uint32_t maxTE144) { maxTE144_ = maxTE144; }

    int32_t GetPluseNum() const { return pluseNum_; }

    // refresh rates
    int32_t GetCurrentRefreshRateMode() const;
    int32_t GetCurrentRefreshRateModeName() const;
    int32_t SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate, bool shouldSendCallback = true);
    int32_t SetRefreshRateMode(int32_t refreshRateMode);

    void NotifyScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    void NotifyScreenRectFrameRateChange(ScreenId id, const Rect& activeRect);

    // screen interface
    int32_t AddScreen(ScreenId id, int32_t defaultMode, ScreenSize& screenSize, bool& isSelfOwnedScreen,
        const std::vector<RSScreenModeInfo>& supportedModes = {});
    int32_t RemoveScreen(ScreenId id);
    uint32_t GetScreenCurrentRefreshRate(ScreenId id) const;
    sptr<HgmScreen> GetScreen(ScreenId id) const;
    sptr<HgmScreen> GetActiveScreen() const;
    std::vector<uint32_t> GetScreenSupportedRefreshRates(ScreenId id);
    std::vector<int32_t> GetScreenComponentRefreshRates(ScreenId id);
    void SetActiveScreenId(ScreenId id) { activeScreenId_.store(id); }

    std::shared_ptr<HgmFrameRateManager> GetFrameRateMgr() { return hgmFrameRateMgr_; }

    // for LTPO
    int64_t GetIdealPeriod(uint32_t rate);
    void SetLtpoConfig();
    void SetScreenConstraintConfig(const PolicyConfigData::ScreenSetting& curScreenSetting);
    void SetPerformanceConfig(const PolicyConfigData::ScreenSetting& curScreenSetting);

    // Test ONLY
    RefreshRateModeChangeCallback GetRefreshRateModeChangeCallback() const { return refreshRateModeChangeCallback_; }
    void RegisterRefreshRateModeChangeCallback(const RefreshRateModeChangeCallback& callback);
    void RegisterRefreshRateUpdateCallback(const RefreshRateUpdateCallback& callback);

    bool GetEnableDynamicMode() const { return enableDynamicMode_; }
    void SetEnableDynamicMode(bool enableDynamicMode) { enableDynamicMode_ = enableDynamicMode; }

    bool IsDelayMode() const { return isDelayMode_; }

    int64_t GetRsPhaseOffset(const int64_t orgValue) const;
    int64_t GetAppPhaseOffset(const int64_t orgValue) const;

    bool GetMultiSelfOwnedScreenEnable() const { return multiSelfOwnedScreenEnable_.load(); }
    void SetMultiSelfOwnedScreenEnable(bool multiSelfOwnedScreenEnable);

    RSScreenManager* GetScreenManager() { return screenManager_; }
    void SetScreenManager(RSScreenManager* screenManager) { screenManager_ = screenManager; }

private:
    HgmCore();
    ~HgmCore() noexcept = default;
    HgmCore(const HgmCore&) = delete;
    HgmCore(const HgmCore&&) = delete;
    HgmCore& operator=(const HgmCore&) = delete;
    HgmCore& operator=(const HgmCore&&) = delete;
    friend class HgmUserDefineImpl;

    void Init();
    int AddParamWatcher() const;
    void CheckCustomFrameRateModeValid();
    int32_t InitXmlConfig();
    int32_t SetCustomRateMode(int32_t mode);
    void SetMaxTEConfig(const PolicyConfigData::ScreenSetting& curScreenSetting);
    void SetASConfig(const PolicyConfigData::ScreenSetting& curScreenSetting);

    bool IsEnabled() const;

    std::unique_ptr<XMLParser> mParser_ = nullptr;
    std::shared_ptr<PolicyConfigData> mPolicyConfigData_ = nullptr;
    std::shared_ptr<PolicyConfigVisitor> mPolicyConfigVisitor_ = nullptr;

    mutable std::mutex listMutex_;
    int32_t customFrameRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    std::vector<ScreenId> screenIds_;
    std::vector<sptr<HgmScreen>> screenList_;

    mutable std::mutex modeListMutex_;
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeListToApply_ = nullptr;

    std::atomic<ScreenId> activeScreenId_{ INVALID_SCREEN_ID };
    std::shared_ptr<HgmFrameRateManager> hgmFrameRateMgr_ = nullptr;

    // for LTPO
    std::atomic<uint32_t> pendingScreenRefreshRate_{ 0 };
    std::atomic<uint32_t> screenRefreshRateImme_{ 0 };
    std::atomic<uint64_t> pendingConstraintRelativeTime_{ 0 };
    std::atomic<uint64_t> timestamp_{ 0 };
    std::atomic<int64_t> actualTimestamp_{ 0 };
    std::atomic<uint64_t> vsyncId_{ 0 };
    std::atomic<bool> isForceRefresh_{ false };
    std::atomic<uint64_t> fastComposeTimeStampDiff_{ 0 };
    bool isDelayMode_ = true;
    bool ltpoEnabled_ = false;
    std::atomic<bool> isLtpoMode_{ false };
    uint32_t maxTE_ = 0;
    uint32_t maxTE144_ = 0;
    uint32_t alignRate_ = 0;
    int64_t idealPipelineOffset_ = 0;
    int64_t idealPipelineOffset144_ = 0;
    int32_t pluseNum_ = -1;
    int adaptiveSync_ = 0;
    int32_t pipelineOffsetPulseNum_ = 8;
    std::atomic<bool> vBlankIdleCorrectSwitch_{ false };
    std::atomic<bool> lowRateToHighQuickSwitch_{ false };
    RefreshRateModeChangeCallback refreshRateModeChangeCallback_ = nullptr;
    RefreshRateUpdateCallback refreshRateUpdateCallback_ = nullptr;
    bool enableDynamicMode_ = true;
    std::atomic<bool> multiSelfOwnedScreenEnable_{ false };
    std::atomic<bool> postHgmTaskFlag_{ true };

    // RS/APP Vsync Offset
    std::atomic<int64_t> rsPhaseOffset_{ 0 };
    std::atomic<int64_t> appPhaseOffset_{ 0 };
    std::atomic<bool> isVsyncOffsetCustomized_{ false };

    RSScreenManager* screenManager_;
};
} // namespace OHOS::Rosen
#endif // HGM_CORE_H