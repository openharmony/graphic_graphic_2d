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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_SINGLE_PROCESS_MANAGER_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_SINGLE_PROCESS_MANAGER_H

#include "rs_render_process_manager.h"

namespace OHOS {
namespace Rosen {
class RSRenderService;
class RSSingleRenderProcessManager final : public RSRenderProcessManager {
public:
    explicit RSSingleRenderProcessManager(RSRenderService& renderService);
    ~RSSingleRenderProcessManager() noexcept override = default;

    sptr<IRemoteObject> OnScreenConnected(ScreenId id,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property) override;
    void OnScreenDisconnected(ScreenId id) override;
    void OnScreenPropertyChanged(
        ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property) override;
    void OnScreenRefresh(ScreenId id) override;
    void OnVirtualScreenConnected(ScreenId id,
        ScreenId associatedScreenId, const sptr<RSScreenProperty>& property) override;
    void OnVirtualScreenDisconnected(ScreenId id) override;

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId = 0) const override;
    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override;
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override;

private:
    sptr<RSIServiceToRenderConnection> serviceToRenderConnection_ = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection_ = nullptr;
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_ = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_RS_RENDER_SINGLE_PROCESS_MANAGER_H
