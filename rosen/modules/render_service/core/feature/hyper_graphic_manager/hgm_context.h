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

#ifndef HGM_CONTEXT_H
#define HGM_CONTEXT_H

#include "event_handler.h"
#include "hgm_info_parcel.h"
#include "ipc_callbacks/rs_ihgm_config_change_callback.h"
#include "screen_manager/rs_screen_property.h"
#include "transaction/rp_hgm_config_data.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {
class HgmCore;
class HgmFrameRateManager;

class HgmContext {
public:
    HgmContext(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<HgmFrameRateManager>& frameRateMgr,
        std::function<void(bool, ScreenId)> callbackFunc,
        const sptr<VSyncDistributor>& appVSyncDistributor,
        const sptr<VSyncDistributor>& rsVSyncDistributor);
    ~HgmContext() noexcept = default;

    void InitHgmTaskHandleThread(
        const sptr<VSyncController>& rsVSyncController, const sptr<VSyncController>& appVSyncController,
        const sptr<VSyncGenerator>& vsyncGenerator);
    void ProcessHgmFrameRate(uint64_t timestamp, uint64_t vsyncId,
        const sptr<HgmProcessToServiceInfo>& processToServiceInfo, sptr<HgmServiceToProcessInfo> serviceToProcessInfo);

    void AddScreenToHgm(const sptr<RSScreenProperty>& property);
    void RemoveScreenFromHgm(ScreenId screenId);

    void CleanAllWhenServiceConnectionDie(pid_t remotePid);
    void CreateFrameRateLinker(const std::string& name, FrameRateLinkerId id, NodeId windowNodeId);
    void UnregisterFrameRateLinker(FrameRateLinkerId id);
    void SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range, int32_t animatorExpectedFrameRate);
    void NotifyXComponentExpectedFrameRate(pid_t remotePid, const std::string& id, int32_t expectedFrameRate);
    int32_t RegisterFrameRateLinkerExpectedFpsUpdateCallback(pid_t pid, int32_t dstPid,
        sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback);
    std::shared_ptr<RSRenderFrameRateLinker> GetRSFrameRateLinker() const { return rsFrameRateLinker_; }

    uint32_t GetScreenCurrentRefreshRate(ScreenId id);
    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);
    void SetRefreshRateMode(int32_t refreshRateMode);
    int32_t GetCurrentRefreshRateMode();
    std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id);

    void NotifyDynamicModeEvent(bool enableDynamicModeEvent);
    void NotifyRefreshRateEvent(pid_t pid, const EventInfo& eventInfo);
    ErrCode NotifyLightFactorStatus(pid_t pid, int32_t lightFactorStatus);
    ErrCode NotifyAppStrategyConfigChangeEvent(pid_t pid, const std::string& pkgName,
        const std::vector<std::pair<std::string, std::string>>& newConfig);
    void HandleTouchEvent(pid_t pid, int32_t touchStatus, int32_t touchCnt);
    void NotifyPackageEvent(pid_t pid, const std::vector<std::string>& packageList);
    void NotifyHgmConfigEvent(const std::string& eventName, bool state);
    void NotifyPageName(pid_t pid, const std::string& packageName, const std::string& pageName, bool isEnter);

    int32_t RegisterHgmRefreshRateUpdateCallback(pid_t pid, sptr<RSIHgmConfigChangeCallback> callback);
    int32_t RegisterHgmConfigChangeCallback(pid_t pid, sptr<RSIHgmConfigChangeCallback> callback);
    int32_t RegisterHgmRefreshRateModeChangeCallback(pid_t pid, sptr<RSIHgmConfigChangeCallback> callback);

    void SetWindowExpectedRefreshRate(pid_t pid, const std::unordered_map<uint64_t, EventInfo>& eventInfos);
    void SetWindowExpectedRefreshRate(pid_t pid, const std::unordered_map<std::string, EventInfo>& eventInfos);
    bool NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string& name, uint32_t rateDiscount);

    uint64_t GetCurrVsyncId() { return currVsyncId_; }
    uint64_t GetLastForceUpdateVsyncId() { return lastForceUpdateVsyncId_; }
    void SetLastForceUpdateVsyncId(uint64_t currVsyncId) { lastForceUpdateVsyncId_ = currVsyncId; }

private:
    void InitHgmUpdateCallback();

    void HandleHgmProcessInfo(const sptr<HgmProcessToServiceInfo>& info);
    void SetServiceToProcessInfo(sptr<HgmServiceToProcessInfo> serviceToProcessInfo);

    void InitHfbcConfig();

    const std::shared_ptr<AppExecFwk::EventHandler> renderServiceHandler_;
    const sptr<VSyncDistributor> appVSyncDistributor_;
    const sptr<VSyncDistributor> rsVSyncDistributor_;
    std::function<void(bool, ScreenId)> requestRSNextVsyncFunc_;

    HgmCore& hgmCore_;
    const std::shared_ptr<HgmFrameRateManager> frameRateManager_;

    FrameRateRange rsCurrRange_;
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker_ = nullptr;
    RSRenderFrameRateLinkerMap frameRateLinkerMap_;

    uint64_t currVsyncId_ = 0;
    uint64_t lastForceUpdateVsyncId_ = UINT64_MAX;

    HgmDataChangeTypes hgmDataChangeTypes_;

    bool ltpoEnabled_ = false;
    bool isDelayMode_ = false;
    int32_t pipelineOffsetPulseNum_ = 0;
    std::shared_ptr<RPHgmConfigData> rpHgmConfigData_ = nullptr;

    bool isAdaptive_ = false;
    std::string gameNodeName_ = "";
};
} // namespace OHOS
} // namespace Rosen
#endif // HGM_CONTEXT_H