/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "command/rs_node_showing_command.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_clear_callback.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_callbacks/screen_change_callback.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "memory/rs_memory_graphic.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"
#include "screen_manager/rs_screen_hdr_capability.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_virtual_screen_resolution.h"
#include "transaction/rs_transaction_data.h"
#include "transaction/rs_render_service_client.h"
#include "ivsync_connection.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "vsync_iconnection_token.h"

namespace OHOS {
namespace Rosen {

class RSIRenderServiceConnection : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");

    RSIRenderServiceConnection() = default;
    virtual ~RSIRenderServiceConnection() noexcept = default;

    virtual void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) = 0;
    virtual void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) = 0;

    virtual bool GetUniRenderEnabled() = 0;

    virtual bool CreateNode(const RSSurfaceRenderNodeConfig& config) = 0;
    virtual sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) = 0;

    virtual sptr<IVSyncConnection> CreateVSyncConnection(const std::string& name,
                                                         const sptr<VSyncIConnectionToken>& token = nullptr) = 0;

    virtual int32_t SetFocusAppInfo(
        int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName,
        uint64_t focusNodeId) = 0;

    virtual ScreenId GetDefaultScreenId() = 0;

    virtual std::vector<ScreenId> GetAllScreenIds() = 0;

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

    virtual int32_t SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) = 0;

    virtual void SetScreenActiveMode(ScreenId id, uint32_t modeId) = 0;

    virtual void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate) = 0;

    virtual void SetRefreshRateMode(int32_t refreshRateMode) = 0;

    virtual uint32_t GetScreenCurrentRefreshRate(ScreenId id) = 0;

    virtual std::vector<uint32_t> GetScreenSupportedRefreshRates(ScreenId id) = 0;

    virtual int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) = 0;

    virtual void TakeSurfaceCapture(
        NodeId id, sptr<RSISurfaceCaptureCallback> callback, float scaleX, float scaleY) = 0;

    virtual void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) = 0;

    virtual RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id) = 0;

    virtual RSScreenModeInfo GetScreenActiveMode(ScreenId id) = 0;

    virtual std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) = 0;

    virtual RSScreenCapability GetScreenCapability(ScreenId id) = 0;

    virtual ScreenPowerStatus GetScreenPowerStatus(ScreenId id) = 0;

    virtual RSScreenData GetScreenData(ScreenId id) = 0;

    virtual MemoryGraphic GetMemoryGraphic(int pid) = 0;

    virtual std::vector<MemoryGraphic> GetMemoryGraphics() = 0;

    virtual int32_t GetScreenBacklight(ScreenId id) = 0;

    virtual void SetScreenBacklight(ScreenId id, uint32_t level) = 0;

    virtual void RegisterBufferAvailableListener(
        NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread) = 0;
    
    virtual void RegisterBufferClearListener(
        NodeId id, sptr<RSIBufferClearCallback> callback) = 0;

    virtual int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) = 0;

    virtual int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) = 0;

    virtual int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) = 0;

    virtual int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) = 0;

    virtual int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) = 0;

    virtual int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) = 0;

    virtual int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) = 0;

    virtual int32_t GetScreenType(ScreenId id, RSScreenType& screenType) = 0;

#ifndef USE_ROSEN_DRAWING
    virtual bool GetBitmap(NodeId id, SkBitmap& bitmap) = 0;
#else
    virtual bool GetBitmap(NodeId id, Drawing::Bitmap& bitmap) = 0;
#endif

    virtual int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) = 0;

    virtual int32_t RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback) = 0;

    virtual void SetAppWindowNum(uint32_t num) = 0;

    virtual void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) = 0;

    virtual void ReportJankStats() = 0;

    virtual void ReportEventResponse(DataBaseRs info) = 0;

    virtual void ReportEventComplete(DataBaseRs info) = 0;

    virtual void ReportEventJankFrame(DataBaseRs info) = 0;

    virtual void ReportEventFirstFrame(DataBaseRs info) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_H
