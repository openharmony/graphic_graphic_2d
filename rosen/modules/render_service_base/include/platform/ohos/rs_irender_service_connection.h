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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_H

#include <iremote_broker.h>
#include <string>
#include <surface.h>

#include "command/rs_command.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/iapplication_render_thread.h"
#include "ipc_callbacks/screen_change_callback.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
class RSSyncTask;

class RSIRenderServiceConnection : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");

    RSIRenderServiceConnection() = default;
    virtual ~RSIRenderServiceConnection() noexcept = default;

    enum {
        COMMIT_TRANSACTION,
        CREATE_NODE_AND_SURFACE,
        GET_DEFAULT_SCREEN_ID,
        CREATE_VIRTUAL_SCREEN,
        SET_VIRTUAL_SCREEN_SURFACE,
        REMOVE_VIRTUAL_SCREEN,
        SET_SCREEN_CHANGE_CALLBACK,
        SET_SCREEN_ACTIVE_MODE,
        SET_SCREEN_POWER_STATUS,
        SET_SCREEN_BACK_LIGHT,
        TAKE_SURFACE_CAPTURE,
        GET_SCREEN_ACTIVE_MODE,
        GET_SCREEN_SUPPORTED_MODES,
        GET_SCREEN_CAPABILITY,
        GET_SCREEN_POWER_STATUS,
        GET_SCREEN_BACK_LIGHT,
        GET_SCREEN_DATA,
        EXECUTE_SYNCHRONOUS_TASK,
        REGISTER_APPLICATION_RENDER_THREAD,
        SET_BUFFER_AVAILABLE_LISTENER,
        GET_SCREEN_SUPPORTED_GAMUTS,
        GET_SCREEN_GAMUT,
        SET_SCREEN_GAMUT,
        SET_SCREEN_GAMUT_MAP,
        GET_SCREEN_GAMUT_MAP,
    };

    virtual void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) = 0;

    virtual void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) = 0;

    virtual sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) = 0;

    virtual ScreenId GetDefaultScreenId() = 0;

    // mirrorId: decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
    virtual ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int32_t flags = 0) = 0;

    virtual int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) = 0;

    virtual void RemoveVirtualScreen(ScreenId id) = 0;

    virtual void SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) = 0;

    virtual void SetScreenActiveMode(ScreenId id, uint32_t modeId) = 0;

    virtual void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) = 0;

    virtual void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback) = 0;

    virtual void RegisterApplicationRenderThread(uint32_t pid, sptr<IApplicationRenderThread> app) = 0;

    virtual RSScreenModeInfo GetScreenActiveMode(ScreenId id) = 0;

    virtual std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) = 0;

    virtual RSScreenCapability GetScreenCapability(ScreenId id) = 0;

    virtual ScreenPowerStatus GetScreenPowerStatus(ScreenId id) = 0;

    virtual RSScreenData GetScreenData(ScreenId id) = 0;

    virtual int32_t GetScreenBacklight(ScreenId id) = 0;

    virtual void SetScreenBacklight(ScreenId id, uint32_t level) = 0;

    virtual void RegisterBufferAvailableListener(NodeId id, sptr<RSIBufferAvailableCallback> callback) = 0;

    virtual int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) = 0;

    virtual int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) = 0;

    virtual int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) = 0;

    virtual int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) = 0;

    virtual int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_H
