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

#ifndef RS_SCREEN_MANAGER_AGENT_H
#define RS_SCREEN_MANAGER_AGENT_H

#include "ipc_callbacks/screen_change_callback.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
class RSScreenManagerAgent;
// This class is designed to notify external modules (such as DMS and SCB) of screen events.
class RSScreenManagerAgentListener : public RSIScreenManagerAgentListener {
public:
    RSScreenManagerAgentListener() = default;
    void OnScreenConnected(ScreenId id, ScreenChangeReason reason, sptr<IRemoteObject> remoteConn);
    void OnScreenDisconnected(ScreenId id, ScreenChangeReason reason);
    void SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback);

private:
    std::mutex mutex_;
    sptr<RSIScreenChangeCallback> screenChangeCallback_;
};

class RSScreenManagerAgent : public RefBase {
public:
    RSScreenManagerAgent(sptr<RSScreenManager> screenManager);
    ~RSScreenManagerAgent();

    int32_t SetScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback);

    ErrCode GetDefaultScreenId(uint64_t& screenId);
    ErrCode GetActiveScreenId(uint64_t& screenId);
    std::vector<ScreenId> GetAllScreenIds();
    void MarkPowerOffNeedProcessOneFrame();

    ScreenId CreateVirtualScreen(const std::string& name, uint32_t width, uint32_t height, sptr<Surface> surface,
        ScreenId associatedScreenId = 0, int32_t flags = 0, std::vector<NodeId> whiteList = {});
    void RemoveVirtualScreen(ScreenId id);
    void CleanVirtualScreens();
    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);
    bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus);

    uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId);
    void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const;

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const;
    RSScreenCapability GetScreenCapability(ScreenId id) const;
    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const;
    RSScreenData GetScreenData(ScreenId id) const;
    int32_t GetScreenBacklight(ScreenId id) const;
    void SetScreenBacklight(ScreenId id, uint32_t level);
    void DisablePowerOffRenderControl(ScreenId id);
    int32_t GetScreenType(ScreenId id, RSScreenType& type) const;
    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) const;

    int32_t SetScreenSwitchingNotifyCallback(sptr<RSIScreenSwitchingNotifyCallback> callback);

    int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    int32_t SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList);
    int32_t AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    int32_t RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    int32_t SetVirtualScreenSecurityExemptionList(ScreenId id, const std::vector<NodeId>& securityExemptionList);
    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask);
    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation);
    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable);

    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height);
    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height);
    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height);
    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id) const;
    int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);
    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const;
    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const;
    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode);
    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) const;

    int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx);
    int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const;
    int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const;
    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const;
    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const;

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation);
    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation);
    int32_t SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation);
    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode); 

    int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat);
    int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const;

    int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace);
    int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const;
    int32_t GetScreenSupportedColorSpaces(ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const;

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval);
    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate);
    uint32_t SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect);
    void SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY);
    void SetScreenSwitchStatus(ScreenId id, bool status);

private:
    sptr<RSScreenManager> screenManager_;
    sptr<RSScreenManagerAgentListener> agentListener_;
    std::unordered_set<ScreenId> virtualScreenIds_;
    std::mutex mutex_;

    friend class RSScreenManagerAgentListener;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_MANAGER_AGENT_H