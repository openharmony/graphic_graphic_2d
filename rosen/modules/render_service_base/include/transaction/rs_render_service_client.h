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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_H

#include <functional>
#include <memory>

#include <refbase.h>
#include <surface.h>

#include "ipc_callbacks/screen_change_callback.h"
#include "platform/drawing/rs_surface.h"
#include "rs_irender_client.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceClient : public RSIRenderClient {
public:
    RSRenderServiceClient() = default;
    virtual ~RSRenderServiceClient() = default;

    RSRenderServiceClient(const RSRenderServiceClient&) = delete;
    void operator=(const RSRenderServiceClient&) = delete;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;

    std::shared_ptr<RSSurface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config);

    ScreenId GetDefaultScreenId();

    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId,
        int32_t flags);

    void RemoveVirtualScreen(ScreenId id);

    void SetScreenChangeCallback(const ScreenChangeCallback &callback);

    void SetScreenActiveMode(ScreenId id, uint32_t modeId);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    RSScreenModeInfo GetScreenActiveMode(ScreenId id);

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id);

    RSScreenCapability GetScreenCapability(ScreenId id);

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id);

    RSScreenData GetScreenData(ScreenId id);

private:
    sptr<RSIScreenChangeCallback> screenChangeCb_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_H
