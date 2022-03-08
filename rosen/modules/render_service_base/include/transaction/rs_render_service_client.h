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
#include <map>
#include <memory>
#include <mutex>
#include <refbase.h>
#include <surface.h>

#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/iapplication_render_thread.h"
#include "ipc_callbacks/screen_change_callback.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "platform/drawing/rs_surface.h"
#include "rs_irender_client.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/screen_types.h"
#include "vsync_receiver.h"

namespace OHOS {
namespace Rosen {
// normal callback functor for client users.
using ScreenChangeCallback = std::function<void(ScreenId, ScreenEvent)>;
using BufferAvailableCallback = std::function<void(bool)>;
class SurfaceCaptureCallback {
public:
    SurfaceCaptureCallback() {}
    virtual ~SurfaceCaptureCallback() {}
    virtual void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) = 0;
};

class RSRenderServiceClient : public RSIRenderClient {
public:
    RSRenderServiceClient() = default;
    virtual ~RSRenderServiceClient() = default;

    RSRenderServiceClient(const RSRenderServiceClient&) = delete;
    void operator=(const RSRenderServiceClient&) = delete;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    std::shared_ptr<RSSurface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config);

    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr);

    bool TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY);

    ScreenId GetDefaultScreenId();

    ScreenId CreateVirtualScreen(const std::string& name, uint32_t width, uint32_t height, sptr<Surface> surface,
        ScreenId mirrorId, int32_t flags);

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);
    
    void RemoveVirtualScreen(ScreenId id);

    int32_t SetScreenChangeCallback(const ScreenChangeCallback& callback);

    void SetScreenActiveMode(ScreenId id, uint32_t modeId);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    RSScreenModeInfo GetScreenActiveMode(ScreenId id);

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id);

    RSScreenCapability GetScreenCapability(ScreenId id);

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id);

    RSScreenData GetScreenData(ScreenId id);

    int32_t GetScreenBacklight(ScreenId id);

    void SetScreenBacklight(ScreenId id, uint32_t level);

    bool RegisterBufferAvailableListener(NodeId id, const BufferAvailableCallback &callback);

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode);

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode);

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode);

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode);
    
    bool RequestRotation(ScreenId id, ScreenRotation rotation);

    ScreenRotation GetRotation(ScreenId id);

private:
    void TriggerSurfaceCaptureCallback(NodeId id, Media::PixelMap* pixelmap);
    std::mutex mutex_;
    std::map<NodeId, sptr<RSIBufferAvailableCallback>> bufferAvailableCbMap_;
    sptr<RSIScreenChangeCallback> screenChangeCb_;
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCbDirector_;
    std::map<NodeId, std::shared_ptr<SurfaceCaptureCallback>> surfaceCaptureCbMap_;

    friend class SurfaceCaptureCallbackDirector;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_H
