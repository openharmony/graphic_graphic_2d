/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_RS_RENDER_PROCESS_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_RS_RENDER_PROCESS_H

#include <event_handler.h>

#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_irender_to_service_connection.h"
#include "rs_render_pipeline.h"
#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

namespace OHOS {
namespace Rosen {
class RSMainThread;

class RSRenderProcess : public RefBase {
public:
    RSRenderProcess() = default;
    ~RSRenderProcess() noexcept override = default;

    RSRenderProcess(const RSRenderProcess&) = delete;
    RSRenderProcess& operator=(const RSRenderProcess&) = delete;

    bool Init();
    void Run();

private:
    sptr<RSIRenderService> GetRenderServer();
    sptr<RSIRenderToServiceConnection> ConnectToRenderService();

    void ApplyIpcPersistenceData(const sptr<RSRenderPipelineAgent>& renderPipelineAgent,
        const IpcPersistenceMap& replayData);
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
#endif

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;

    sptr<RSIRenderToServiceConnection> renderToServiceConnection_ = nullptr;
    std::shared_ptr<RSRenderPipeline> renderPipeline_ = nullptr;

#ifdef RES_SCHED_ENABLE
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif

    friend class RSRenderProcessAgent;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_RS_RENDER_PROCESS_H
