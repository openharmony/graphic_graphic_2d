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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H

#include <memory>

#include "transaction/rs_render_service_client.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSInterfaces {
public:
    static RSInterfaces &GetInstance();
    RSInterfaces(const RSInterfaces &) = delete;
    void operator=(const RSInterfaces &) = delete;

    ScreenId GetDefaultScreenId();

    // mirrorId: decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int flags = 0);

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);

    void RemoveVirtualScreen(ScreenId id);

    int32_t SetScreenChangeCallback(const ScreenChangeCallback &callback);

    bool TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.0f, float scaleY = 1.0f);

    bool TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.0f, float scaleY = 1.0f);

    void SetScreenActiveMode(ScreenId id, uint32_t modeId);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    RSScreenModeInfo GetScreenActiveMode(ScreenId id);

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id);

    RSScreenCapability GetScreenCapability(ScreenId id);

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id);

    RSScreenData GetScreenData(ScreenId id);

    int32_t GetScreenBacklight(ScreenId id);

    void SetScreenBacklight(ScreenId id, uint32_t level);

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode);

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode);

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode);

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode);
    
    bool RequestRotation(ScreenId id, ScreenRotation rotation);

    ScreenRotation GetRotation(ScreenId id);

    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability);

    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr);

private:
    RSInterfaces();
    ~RSInterfaces() noexcept;

    std::unique_ptr<RSRenderServiceClient> renderServiceClient_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H
