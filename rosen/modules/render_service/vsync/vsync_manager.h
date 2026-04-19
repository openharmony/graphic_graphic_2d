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

#ifndef RS_VSYNC_MANAGER_H
#define RS_VSYNC_MANAGER_H

#include "vsync_generator.h"
#include "vsync_distributor.h"
#include "vsync_sampler.h"
#include "vsync_controller.h"
#include "vsync_manager_agent.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_property.h"
#include "screen_manager/screen_types.h"
#include "hgm_core.h"
#include "graphic_feature_param_manager.h"

namespace OHOS {
namespace Rosen {
class RSVsyncManager : public RefBase {
public:
    RSVsyncManager();
    ~RSVsyncManager();
    void SetScreenManager(sptr<RSScreenManager> screenManager);
    sptr<VSyncDistributor> GetVSyncAppDistributor();
    sptr<VSyncDistributor> GetVSyncRSDistributor();
    sptr<VSyncGenerator> GetVSyncGenerator();
    sptr<VSyncController> GetVSyncRSController();
    sptr<VSyncController> GetVSyncAppController();
    sptr<VSyncSampler> GetVSyncSampler();
    bool init(sptr<RSScreenManager> screenManager);
    ScreenId OnScreenConnected(ScreenId screenId, std::shared_ptr<AppExecFwk::EventHandler> handler);
    void OnScreenDisconnected(ScreenId id, std::shared_ptr<AppExecFwk::EventHandler> handler);
    void OnScreenPropertyChanged(ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property,
        std::shared_ptr<AppExecFwk::EventHandler> handler, bool isFoldScreen);
    VsyncError AddRSVsyncConnection(const sptr<VSyncConnection>& connection);
    sptr<VSyncConnection> CreateRSVSyncConnection(std::string name,
        const sptr<IRemoteObject>& token = nullptr);
    sptr<RSVsyncManagerAgent> GetVsyncManagerAgent();

private:
    bool VsyncComponentInit();
    DVSyncFeatureParam InitDVSyncParams();
    void RegSetScreenVsyncEnabledCallbackForRenderService(ScreenId vsyncEnabledScreenId,
        std::shared_ptr<AppExecFwk::EventHandler> handler);
    uint64_t JudgeVSyncEnabledScreenWhileHotPlug(ScreenId screenId, bool connected);
    sptr<VSyncGenerator> vsyncGenerator_ = nullptr;
    sptr<VSyncSampler> vsyncSampler_ = nullptr;
    sptr<VSyncController> rsVSyncController_ = nullptr;
    sptr<VSyncController> appVSyncController_ = nullptr;
    sptr<VSyncDistributor> rsVSyncDistributor_ = nullptr;
    sptr<VSyncDistributor> appVSyncDistributor_ = nullptr;
    sptr<RSScreenManager> screenManager_ = nullptr;
    sptr<RSVsyncManagerAgent> rsVsyncManagerAgent_ = nullptr;
    mutable std::mutex mutex_;
    std::unordered_set<ScreenId> physicalScreens_;
};
} // namespace Rosen
} // namespace OHOS
#endif