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

#include "hgm_screen.h"
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

    bool IsEnabled() const
    {
        return isEnabled_;
    }

    bool IsInit() const
    {
        return isInit_;
    }

    int32_t GetScreenListSize() const
    {
        return screenList_.size();
    }

    ScreenId GetActiveScreenId() const
    {
        return activeScreenId_;
    }

    const std::shared_ptr<PolicyConfigData>& GetPolicyConfigData() const
    {
        return mPolicyConfigData_;
    }

    void SetPendingScreenRefreshRate(uint32_t rate)
    {
        pendingScreenRefreshRate_ = rate;
    }

    uint32_t GetPendingScreenRefreshRate() const
    {
        return pendingScreenRefreshRate_;
    }

    void SetTimestamp(uint64_t timestamp)
    {
        timestamp_ = timestamp;
    }

    uint64_t GetCurrentTimestamp() const
    {
        return timestamp_;
    }

    bool GetLtpoEnabled() const
    {
        return ltpoEnabled_ && (customFrameRateMode_ == HGM_REFRESHRATE_MODE_AUTO) &&
            (maxTE_ == VSYNC_MAX_REFRESHRATE);
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

    bool GetDirectCompositionFlag() const
    {
        return doDirectComposition_;
    }

    void SetDirectCompositionFlag(bool doDirectComposition)
    {
        doDirectComposition_ = doDirectComposition;
    }

    // set refresh rates
    int32_t SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);
    static int32_t SetRateAndResolution(ScreenId id, int32_t sceneId, int32_t rate, int32_t width, int32_t height);
    int32_t SetRefreshRateMode(int32_t refreshRateMode);

    void NotifyScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    // screen interface
    int32_t AddScreen(ScreenId id, int32_t defaultMode, ScreenSize& screenSize);
    int32_t RemoveScreen(ScreenId id);
    int32_t AddScreenInfo(ScreenId id, int32_t width, int32_t height, uint32_t rate, int32_t mode);
    int32_t RefreshBundleName(const std::string& name);
    uint32_t GetScreenCurrentRefreshRate(ScreenId id) const;
    int32_t GetCurrentRefreshRateMode() const;
    int32_t GetCurrentRefreshRateModeName() const;
    sptr<HgmScreen> GetScreen(ScreenId id) const;
    sptr<HgmScreen> GetActiveScreen() const;
    std::vector<uint32_t> GetScreenSupportedRefreshRates(ScreenId id);
    std::vector<int32_t> GetScreenComponentRefreshRates(ScreenId id);
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> GetModesToApply();
    void SetActiveScreenId(ScreenId id);
    std::shared_ptr<HgmFrameRateManager> GetFrameRateMgr() { return hgmFrameRateMgr_; };

    // for LTPO
    void SetLtpoConfig();
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
private:
    HgmCore();
    ~HgmCore() = default;
    HgmCore(const HgmCore&) = delete;
    HgmCore(const HgmCore&&) = delete;
    HgmCore& operator=(const HgmCore&) = delete;
    HgmCore& operator=(const HgmCore&&) = delete;

    bool Init();
    void CheckCustomFrameRateModeValid();
    int32_t InitXmlConfig();
    int32_t SetCustomRateMode(int32_t mode);

    bool isEnabled_ = true;
    bool isInit_ = false;
    static constexpr char configFileProduct[] = "/sys_prod/etc/graphic/hgm_policy_config.xml";
    std::unique_ptr<XMLParser> mParser_;
    std::shared_ptr<PolicyConfigData> mPolicyConfigData_ = nullptr;

    int32_t customFrameRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    std::vector<ScreenId> screenIds_;
    std::vector<sptr<HgmScreen>> screenList_;
    mutable std::mutex listMutex_;

    mutable std::mutex modeListMutex_;
    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeListToApply_ = nullptr;

    std::string currentBundleName_;
    ScreenId activeScreenId_ = INVALID_SCREEN_ID;
    std::unordered_set<SceneType> screenSceneSet_;
    std::shared_ptr<HgmFrameRateManager> hgmFrameRateMgr_ = nullptr;

    // for LTPO
    uint32_t pendingScreenRefreshRate_ = 0;
    uint64_t timestamp_ = 0;
    bool ltpoEnabled_ = false;
    uint32_t maxTE_ = 0;
    uint32_t alignRate_ = 0;
    int32_t pipelineOffsetPulseNum_ = 8;
    RefreshRateModeChangeCallback refreshRateModeChangeCallback_ = nullptr;
    RefreshRateUpdateCallback refreshRateUpdateCallback_ = nullptr;
    bool doDirectComposition_ = false;
};
} // namespace OHOS::Rosen
#endif // HGM_CORE_H
