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

#include <cstdint>
#include <functional>
#include <cinttypes>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <event_handler.h>

#include "hgm_frame_rate_manager.h"
#include "hgm_hfbc_config.h"
#include "hgm_screen.h"
#include "hgm_task_handle_thread.h"
#include "vsync_type.h"
#include "vsync_generator.h"
#include "xml_parser.h"

namespace OHOS::Rosen {
class HgmFrameRateManager;
using RefreshRateModeChangeCallback = std::function<void(int32_t)>;
using RefreshRateUpdateCallback = std::function<void(int32_t)>;
class HgmCore final {
public:
    static HgmCore& Instance();

    std::vector<ScreenId> GetScreenIds() const
    {
        return screenIds_;
    }

    int32_t GetScreenListSize() const
    {
        return screenList_.size();
    }

    ScreenId GetActiveScreenId() const
    {
        if (hgmFrameRateMgr_ != nullptr) {
            return hgmFrameRateMgr_->GetCurScreenId();
        }
        return activeScreenId_.load();
    }

    const std::shared_ptr<PolicyConfigData>& GetPolicyConfigData() const
    {
        return mPolicyConfigData_;
    }

    // called by RSMainThread
    void SetPendingScreenRefreshRate(uint32_t rate)
    {
        pendingScreenRefreshRate_.store(rate);
    }

    // called by RSMainThread/RSUniRenderThread
    uint32_t GetPendingScreenRefreshRate() const
    {
        return pendingScreenRefreshRate_.load();
    }

    // called by HgmThread
    // the rate takes effect at the latest hardware timing
    void SetScreenRefreshRateImme(uint32_t rate)
    {
        screenRefreshRateImme_.store(rate);
    }

    // called by HardwareThread
    uint32_t GetScreenRefreshRateImme()
    {
        // 0 means disenable
        return screenRefreshRateImme_.exchange(0);
    }

    // called by RSMainThread
    void SetPendingConstraintRelativeTime(uint64_t relativeTime)
    {
        pendingConstraintRelativeTime_.store(relativeTime);
    }

    // called by RSMainThread/RSUniRenderThread
    uint64_t GetPendingConstraintRelativeTime() const
    {
        return pendingConstraintRelativeTime_.load();
    }

    // called by RSMainThread
    void SetTimestamp(uint64_t timestamp)
    {
        timestamp_.store(timestamp);
    }

    // called by RSMainThread/RSUniRenderThread
    uint64_t GetCurrentTimestamp() const
    {
        return timestamp_.load();
    }

    // called by RSMainThread
    void SetActualTimestamp(int64_t timestamp)
    {
        actualTimestamp_.store(timestamp);
    }

    // called by RSMainThread/RSUniRenderThread
    int64_t GetActualTimestamp() const
    {
        return actualTimestamp_.load();
    }

    // called by RSMainThread
    void SetVsyncId(uint64_t vsyncId)
    {
        vsyncId_.store(vsyncId);
    }

    // called by RSMainThread/RSUniRenderThread
    uint64_t GetVsyncId() const
    {
        return vsyncId_.load();
    }

    // called by RSMainThread
    void SetForceRefreshFlag(bool isForceRefresh)
    {
        isForceRefresh_.store(isForceRefresh);
    }

    // called by RSMainThread/RSUniRenderThread
    bool GetForceRefreshFlag() const
    {
        return isForceRefresh_.load();
    }

    // called by RSMainThread
    void SetFastComposeTimeStampDiff(uint64_t fastComposeTimeStampDiff)
    {
        fastComposeTimeStampDiff_.store(fastComposeTimeStampDiff);
    }

    // called by RSMainThread/RSUniRenderThread
    uint64_t GetFastComposeTimeStampDiff() const
    {
        return fastComposeTimeStampDiff_.load();
    }

    // called by RSMainThread
    bool SetHgmTaskFlag(bool value)
    {
        return postHgmTaskFlag_.exchange(value);
    }

    bool GetLtpoEnabled() const
    {
        return ltpoEnabled_ && (maxTE_ == CreateVSyncGenerator()->GetVSyncMaxRefreshRate());
    }

    bool GetAdaptiveSyncEnabled() const
    {
        return GetLtpoEnabled() && adaptiveSync_ == ADAPTIVE_SYNC_ENABLED;
    }

    // called by RSHardwareTHread
    bool IsVBlankIdleCorrectEnabled() const
    {
        return vBlankIdleCorrectSwitch_.load();
    }

    bool IsLowRateToHighQuickEnabled() const
    {
        return lowRateToHighQuickSwitch_.load();
    }

    bool IsLTPOSwitchOn() const
    {
        return ltpoEnabled_;
    }

    void SetLtpoEnabled(bool ltpoEnabled)
    {
        ltpoEnabled_ = ltpoEnabled;
    }

    uint32_t GetAlignRate() const
    {
        return alignRate_;
    }

    void SetIdealPipelineOffset(int32_t pipelineOffsetPulseNum)
    {
        idealPipelineOffset_ = pipelineOffsetPulseNum * IDEAL_PULSE;
    }

    int64_t GetIdealPipelineOffset() const
    {
        return idealPipelineOffset_;
    }

    int64_t GetPipelineOffset() const
    {
        auto pulse = CreateVSyncGenerator()->GetVSyncPulse();
        return pipelineOffsetPulseNum_ * pulse;
    }

    uint32_t GetSupportedMaxTE() const
    {
        return maxTE_;
    }

    void SetSupportedMaxTE(uint32_t maxTE)
    {
        maxTE_ = maxTE;
    }

    int32_t GetPluseNum() const
    {
        return pluseNum_;
    }

    // called by RSMainThread/RSUniRenderThread
    bool GetDirectCompositionFlag() const
    {
        return doDirectComposition_.load();
    }

    // called by RSMainThread
    void SetDirectCompositionFlag(bool doDirectComposition)
    {
        doDirectComposition_.store(doDirectComposition);
    }

    // set refresh rates
    int32_t SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);
    static int32_t SetRateAndResolution(ScreenId id, int32_t sceneId, int32_t rate, int32_t width, int32_t height);
    int32_t SetRefreshRateMode(int32_t refreshRateMode);

    void NotifyScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    void NotifyScreenRectFrameRateChange(ScreenId id, const GraphicIRect& activeRect);

    // screen interface
    int32_t AddScreen(ScreenId id, int32_t defaultMode, ScreenSize& screenSize);
    int32_t RemoveScreen(ScreenId id);
    int32_t AddScreenInfo(ScreenId id, int32_t width, int32_t height, uint32_t rate, int32_t mode);
    uint32_t GetScreenCurrentRefreshRate(ScreenId id) const;
    int32_t GetCurrentRefreshRateMode() const;
    int32_t GetCurrentRefreshRateModeName() const;
    sptr<HgmScreen> GetScreen(ScreenId id) const;
    sptr<HgmScreen> GetActiveScreen() const;
    std::vector<uint32_t> GetScreenSupportedRefreshRates(ScreenId id);
    std::vector<int32_t> GetScreenComponentRefreshRates(ScreenId id);
    // called by RSHardwareTHread
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> GetModesToApply();
    void SetActiveScreenId(ScreenId id);
    std::shared_ptr<HgmFrameRateManager> GetFrameRateMgr() { return hgmFrameRateMgr_; };

    // for LTPO
    void SetLtpoConfig();
    void SetScreenConstraintConfig();
    void SetPerformanceConfig();
    int64_t GetIdealPeriod(uint32_t rate);
    void RegisterRefreshRateModeChangeCallback(const RefreshRateModeChangeCallback& callback);
    void RegisterRefreshRateUpdateCallback(const RefreshRateUpdateCallback& callback);
    RefreshRateModeChangeCallback GetRefreshRateModeChangeCallback() const
    {
        return refreshRateModeChangeCallback_;
    }

    RefreshRateUpdateCallback GetRefreshRateUpdate() const
    {
        return refreshRateUpdateCallback_;
    }

    void SetEnableDynamicMode(bool enableDynamicMode)
    {
        enableDynamicMode_ = enableDynamicMode;
    }

    bool GetEnableDynamicMode() const
    {
        return enableDynamicMode_;
    }

    bool IsDelayMode() const
    {
        return isDelayMode_;
    }

    void SetMultiSelfOwnedScreenEnable(bool multiSelfOwnedScreenEnable)
    {
        multiSelfOwnedScreenEnable_.store(multiSelfOwnedScreenEnable);
    }

    bool GetMultiSelfOwnedScreenEnable() const
    {
        return multiSelfOwnedScreenEnable_.load();
    }

    // only called/used by RSHardwareThread
    bool IsSwitchDssEnable(ScreenId screenId) const
    {
        if (auto iter = screenSwitchDssEnableMap_.find(screenId); iter != screenSwitchDssEnableMap_.end()) {
            return iter->second;
        }
        return false;
    }

    // only called/used by RSHardwareThread
    void SetScreenSwitchDssEnable(ScreenId screenId, bool switchDssEnable)
    {
        screenSwitchDssEnableMap_[screenId] = switchDssEnable;
    }

    // called by RSMainThread
    HgmHfbcConfig& GetHfbcConfig()
    {
        return hfbcConfig_;
    }

    bool CheckNeedUpdateAppOffsetRefreshRate(uint32_t refreshRate)
    {
        return refreshRate == OLED_60_HZ;
    }
private:
    HgmCore();
    ~HgmCore() = default;
    HgmCore(const HgmCore&) = delete;
    HgmCore(const HgmCore&&) = delete;
    HgmCore& operator=(const HgmCore&) = delete;
    HgmCore& operator=(const HgmCore&&) = delete;

    void Init();
    void CheckCustomFrameRateModeValid();
    int32_t InitXmlConfig();
    int32_t SetCustomRateMode(int32_t mode);
    void SetASConfig(const PolicyConfigData::ScreenSetting& curScreenSetting);

    bool IsEnabled() const
    {
        return mPolicyConfigData_ != nullptr && !mPolicyConfigData_->refreshRateForSettings_.empty();
    }

    static constexpr char configFileProduct[] = "/sys_prod/etc/graphic/hgm_policy_config.xml";
    std::unique_ptr<XMLParser> mParser_ = nullptr;
    std::shared_ptr<PolicyConfigData> mPolicyConfigData_ = nullptr;

    int32_t customFrameRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    std::vector<ScreenId> screenIds_;
    std::vector<sptr<HgmScreen>> screenList_;
    mutable std::mutex listMutex_;

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
    uint32_t maxTE_ = 0;
    uint32_t alignRate_ = 0;
    int64_t idealPipelineOffset_ = 0;
    int32_t pluseNum_ = -1;
    int adaptiveSync_ = 0;
    int32_t pipelineOffsetPulseNum_ = 8;
    std::atomic<bool> vBlankIdleCorrectSwitch_{ false };
    std::atomic<bool> lowRateToHighQuickSwitch_{ false };
    RefreshRateModeChangeCallback refreshRateModeChangeCallback_ = nullptr;
    RefreshRateUpdateCallback refreshRateUpdateCallback_ = nullptr;
    std::atomic<bool> doDirectComposition_{ false };
    bool enableDynamicMode_ = true;
    std::atomic<bool> multiSelfOwnedScreenEnable_{ false };
    std::atomic<bool> postHgmTaskFlag_{ true };
    std::unordered_map<ScreenId, bool> screenSwitchDssEnableMap_; // only called/used by RSHardwareThread
    HgmHfbcConfig hfbcConfig_;

    friend class HWCParam;
};
} // namespace OHOS::Rosen
#endif // HGM_CORE_H
