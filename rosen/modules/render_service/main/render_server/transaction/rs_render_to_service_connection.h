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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_RS_RENDER_TO_SERVICE_CONNECTION_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_RS_RENDER_TO_SERVICE_CONNECTION_H

#include "transaction/zidl/rs_render_to_service_connection_stub.h"
#include "render_server/rs_render_service_agent.h"
#include "render_server/rs_render_process_manager_agent.h"
#include "screen_manager/public/rs_screen_manager_agent.h"

namespace OHOS {
namespace Rosen {
class RSRenderToServiceConnection : public RSRenderToServiceConnectionStub {
public:
    RSRenderToServiceConnection(sptr<RSRenderServiceAgent> renderServiceAgent,
        sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent, sptr<RSScreenManagerAgent> screenManagerAgent);
    virtual ~RSRenderToServiceConnection() noexcept = default;

    bool NotifyRenderProcessInitFinished() override;
    void ReplyDumpResultToService(std::string& dumpString) override;
    sptr<HgmServiceToProcessInfo> NotifyRenderServiceProcessHgmFrameRate(uint64_t timestamp, uint64_t vsyncId,
        std::unordered_set<ScreenId> screenIds, const sptr<HgmProcessToServiceInfo>& processToServiceInfo) override;
    void NotifyScreenSwitchFinished(ScreenId screenId) override;
        
private:
    sptr<RSRenderServiceAgent> renderServiceAgent_ = nullptr;
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent_ = nullptr;
    sptr<RSScreenManagerAgent> screenManagerAgent_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_RS_RENDER_TO_SERVICE_CONNECTION_H