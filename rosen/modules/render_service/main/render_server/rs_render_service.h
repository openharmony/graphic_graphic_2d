/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_SERVICE_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_SERVICE_H

#include <event_handler.h>
#include <map>
#include <unordered_set>

#include "rs_render_pipeline.h"
#include "rs_render_single_process_manager.h"
#include "vsync_controller.h"
#include "vsync_distributor.h"
#include "vsync_manager_agent.h"
#include "vsync_iconnection_token.h"
#include "vsync_receiver.h"
#include "dfx/rs_service_dumper.h"

#include "screen_manager/rs_screen_manager.h"
#include "transaction/zidl/rs_render_service_stub.h"
#include "feature/hyper_graphic_manager/hgm_context.h"

namespace OHOS {
namespace Rosen {
class RSMainThread;
class RSRenderService : public RSRenderServiceStub {
public:
    RSRenderService() = default;
    ~RSRenderService() noexcept = default;

    RSRenderService(const RSRenderService&) = delete;
    RSRenderService& operator=(const RSRenderService&) = delete;

    bool Init();
    void Run();

private:
    class ScreenManagerListener : public RSIScreenManagerListener {
    public:
        explicit ScreenManagerListener(RSRenderService& renderService) : renderService_(renderService) {}
        ~ScreenManagerListener() noexcept override = default;

        sptr<IRemoteObject> OnScreenConnected(ScreenId id,
            const ScreenEventData& data, const sptr<RSScreenProperty>& property) override;
        void OnScreenDisconnected(ScreenId id) override;
        void OnScreenPropertyChanged(ScreenId id, const sptr<RSScreenProperty>& property) override;
        void OnScreenRefresh(ScreenId id) override;
        void OnVBlankIdle(ScreenId id, uint64_t ns) override;
        void OnVirtualScreenConnected(ScreenId id,
            ScreenId associatedScreenId, const sptr<RSScreenProperty>& property) override;
        void OnVirtualScreenDisconnected(ScreenId id) override;
        void OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData) override;
        void OnActiveScreenIdChanged(ScreenId activeScreenId) override;
        void OnScreenBacklightChanged(ScreenId id, uint32_t level) override;

    private:
        RSRenderService& renderService_;
    };

    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void DumpSurfaceNode(std::string& dumpString, NodeId id) const;

    std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> CreateConnection(const sptr<RSIConnectionToken>& token) override;
    bool RemoveConnection(const sptr<RSIConnectionToken>& token) override;

    void InitDVSyncParams(DVSyncFeatureParam &dvsyncParam);
    void InitCCMConfig();
    // RS Filter CCM init
    void FilterCCMInit();

    void CoreComponentsInit();
    void FeatureComponentInit();
    void VsyncComponentInit();
    void RenderProcessManagerInit();
    bool SAMgrRegister();

    void HandleTouchEvent(int32_t touchStatus, int32_t touchCnt);
    void GetRefreshInfoToSP(std::string& dumpString, NodeId& nodeId);
    void FpsDump(std::string& dumpString, std::string& arg);
    const std::shared_ptr<HgmContext>& GetHgmContext() const { return hgmContext_; }

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    sptr<RSScreenManager> screenManager_ = nullptr;
    sptr<RSRenderProcessManager> renderProcessManager_ = nullptr;
    sptr<VSyncGenerator> vsyncGenerator_ = nullptr;
    sptr<VSyncSampler> vsyncSampler_ = nullptr;
    sptr<VSyncController> rsVSyncController_ = nullptr;
    sptr<VSyncController> appVSyncController_ = nullptr;
    sptr<VSyncDistributor> rsVSyncDistributor_ = nullptr;
    sptr<VSyncDistributor> appVSyncDistributor_ = nullptr;
    sptr<RSVsyncManagerAgent> rsVsyncManagerAgent_ = nullptr;
    std::shared_ptr<RSRenderComposerManager> rsRenderComposerManager_ = nullptr;
    std::shared_ptr<HgmContext> hgmContext_ = nullptr;

    // TODO: DO NOT USE. Will be removed asap
    RSMainThread* mainThread_ = nullptr;
    std::shared_ptr<RSRenderPipeline> renderPipeline_ = nullptr;

    friend class RSRenderServiceAgent;
    friend class RSRenderProcessManager;
    friend class RSSingleRenderProcessManager;
    friend class RSConnectToRenderProcess;
    friend class RSClientToRenderConnection;
    friend class RSClientToServiceConnection;
    mutable std::mutex mutex_;
    std::map<sptr<IRemoteObject>, std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>>>
        connections_;

    std::shared_ptr<RSServiceDumper> rsDumper_;

#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_SERVICE_H
