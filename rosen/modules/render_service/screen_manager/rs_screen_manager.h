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

#ifndef RS_SCREEN_MANAGER
#define RS_SCREEN_MANAGER

#include <condition_variable>
#include <cstdint>
#include <event_handler.h>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include <ipc_callbacks/screen_change_callback.h>
#include <ipc_callbacks/screen_switching_notify_callback.h>
#include <refbase.h>
#include <surface.h>
#include <surface_type.h>

#include <screen_manager/rs_screen_props.h>
#include <screen_manager/rs_screen_mode_info.h>
#include <screen_manager/rs_screen_capability.h>
#include <screen_manager/rs_screen_data.h>
#include <screen_manager/rs_screen_hdr_capability.h>
#include <screen_manager/screen_types.h>
#include <screen_manager/rs_virtual_screen_resolution.h>
#include <screen_manager/rs_screen_info.h>
#include <screen_manager/rs_screen_property.h>
#include "rs_screen_preprocessor.h"
#include "product_feature/fold/rs_fold_screen_manager.h"

namespace OHOS {
namespace Rosen {
class RSScreen;
// This class can be only created by RSRenderService to manager screen.
class RSScreenManager : public RefBase {
public:
    RSScreenManager() = default;
    ~RSScreenManager() = default;
    RSScreenManager(const RSScreenManager&) = delete;
    RSScreenManager& operator=(const RSScreenManager&) = delete;
    void RegisterCoreListener(const sptr<RSIScreenManagerListener>& listener);
    void RegisterAgentListener(const sptr<RSIScreenManagerAgentListener>& listener);
    void UnRegisterAgentListener(const sptr<RSIScreenManagerAgentListener>& listener);
    bool Init(const std::shared_ptr<AppExecFwk::EventHandler>& mainHandler) noexcept;

    ScreenId GetDefaultScreenId() const;
    ScreenId GetActiveScreenId();
    std::vector<ScreenId> GetAllScreenIds() const;

    int32_t GetScreenType(ScreenId id, RSScreenType& type) const;

    int32_t SetScreenSwitchingNotifyCallback(const sptr<RSIScreenSwitchingNotifyCallback>& callback);
    void DisplayDump(std::string& dumpString);
    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval);

    /* only used for mock tests */
    void MockHdiScreenConnected(std::shared_ptr<RSScreen> rsScreen);

    uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId);
    void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const;
    void GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const;
    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const;
    uint32_t GetScreenActiveRefreshRate(ScreenId id) const;

    RSScreenCapability GetScreenCapability(ScreenId id) const;
    RSScreenData GetScreenData(ScreenId id) const;
    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) const;

    ScreenConnectionType GetScreenConnectionType(ScreenId id) const;

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation);

    int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height);
    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const;
    void DisablePowerOffRenderControl(ScreenId id);
    // used to skip render frame or render only one frame when screen power is off.
    void MarkPowerOffNeedProcessOneFrame();

    void SetScreenBacklight(ScreenId id, uint32_t level);
    int32_t GetScreenBacklight(ScreenId id) const;
    int32_t SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type);

    int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat);
    int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const;

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

    int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace);
    int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const;
    int32_t GetScreenSupportedColorSpaces(
        ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const;

    void SetScreenSwitchStatus(ScreenId id, bool status);

    uint32_t SetScreenActiveRect(ScreenId id, const Rect& activeRect);
    int32_t SetScreenLinearMatrix(ScreenId id, const std::vector<float>& matrix);

    // virtual screen
    ScreenId CreateVirtualScreen(const std::string& name, uint32_t width, uint32_t height, sptr<Surface> surface,
        ScreenId associatedScreenId = 0, int32_t flags = 0, std::vector<uint64_t> whiteList = {});
    void RemoveVirtualScreen(ScreenId id);
    uint32_t GetCurrentVirtualScreenNum();

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);
    sptr<Surface> GetProducerSurface(ScreenId id) const;

    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height);
    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height);
    void GetVirtualScreenResolution(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const;

    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation);
    bool GetCanvasRotation(ScreenId id) const;

    int32_t SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation);
    bool GetVirtualScreenAutoRotation(ScreenId id) const;

    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode ScaleMode);
    ScreenScaleMode GetScaleMode(ScreenId id) const;

    bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus);

    // type blacklist
    int32_t SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList);
    
    // blacklist
    int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    int32_t AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    int32_t RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);

    // global blacklist
    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable);

    int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<uint64_t>& securityExemptionList);

    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask);

    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false);

    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate);

    void SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY);
    void SetScreenFrameGravity(ScreenId id, int32_t gravity);

    void ExecuteCallback(const sptr<RSIScreenChangeCallback>& callback) const;

    bool UpdateVsyncEnabledScreenId(ScreenId screenId);
    uint64_t JudgeVSyncEnabledScreenWhilePowerStatusChanged(ScreenId screenId, ScreenPowerStatus status, uint64_t enabledScreenId);
    void UpdateFoldScreenConnectStatusLocked(ScreenId screenId, bool connected);
    void SetScreenVsyncEnableById(ScreenId vsyncEnabledScreenId, ScreenId screenId, bool enabled);
    uint64_t GetScreenVsyncEnableById(ScreenId vsyncEnabledScreenId);
    bool GetIsFoldScreenFlag();

private:
    void OnHwcDeadEvent(std::map<ScreenId, std::shared_ptr<RSScreen>>& retScreens);

    // physical screen
    bool CheckFoldScreenIdBuiltIn(ScreenId id);
    void ProcessScreenConnected(ScreenId id);
    void ProcessPendingConnections();
    void ProcessScreenDisConnected(ScreenId id);
    void HandleDefaultScreenDisConnected();

    sptr<RSScreenProperty> QueryScreenProperty(ScreenId id) const; // Only for internal use by ScreenManager
    std::shared_ptr<RSScreen> GetScreen(ScreenId id) const;
    void NotifySwitchingCallback(bool status) const;

    // virtual screen
    ScreenId GenerateVirtualScreenId();

    mutable std::mutex screenMapMutex_;
    std::map<ScreenId, std::shared_ptr<RSScreen>> screens_;
    using ScreenNode = decltype(screens_)::value_type;
    bool AnyScreenFits(std::function<bool(const ScreenNode&)> func) const;

    void OnScreenBacklightChanged(ScreenId id, uint32_t level);

    // global blacklist
    int32_t SetGlobalBlackList(const std::unordered_set<NodeId>& blackList);
    int32_t AddGlobalBlackList(const std::vector<NodeId>& blackList);
    int32_t RemoveGlobalBlackList(const std::vector<uint64_t>& blackList);

    std::atomic<ScreenId> defaultScreenId_ = INVALID_SCREEN_ID;

    std::mutex virtualScreenIdMutex_;
    std::queue<ScreenId> freeVirtualScreenIds_;
    std::atomic<uint32_t> virtualScreenCount_ = 0;
    std::atomic<uint32_t> currentVirtualScreenNum_ = 0;

    mutable std::shared_mutex screenChangeCallbackMutex_;
    mutable std::shared_mutex screenSwitchingNotifyCallbackMutex_;
    sptr<RSIScreenSwitchingNotifyCallback> screenSwitchingNotifyCallback_;

    std::atomic<bool> mipiCheckInFirstHotPlugEvent_ = false;

    mutable std::mutex hotPlugAndConnectMutex_;
    std::vector<ScreenId> pendingConnectedIds_;

    mutable std::shared_mutex powerStatusMutex_;
    std::unordered_map<ScreenId, uint32_t> screenPowerStatus_;

    mutable std::shared_mutex backLightAndCorrectionMutex_;
    std::unordered_map<ScreenId, uint32_t> screenBacklight_;
    std::unordered_map<ScreenId, ScreenRotation> screenCorrection_;

    std::mutex globalBlackListMutex_;
    std::unordered_set<NodeId> globalBlackList_ = {};

    std::atomic<uint64_t> frameId_ = 0;

    std::atomic<bool> powerOffNeedProcessOneFrame_ = false;

    mutable std::mutex renderControlMutex_;
    std::unordered_set<ScreenId> disableRenderControlScreens_ = {};

    std::atomic<bool> isScreenSwitching_ = false;

    bool isFoldScreenFlag_ = false;
    std::unique_ptr<RSFoldScreenManager> foldScreenManager_;
    struct FoldScreenStatus {
        bool isConnected;
        bool isPowerOn;
    };
    std::unordered_map<uint64_t, FoldScreenStatus> foldScreenIds_; // screenId, FoldScreenStatus

    const std::unique_ptr<RSScreenCallbackManager> callbackMgr_ = std::make_unique<RSScreenCallbackManager>();
    std::unique_ptr<RSScreenPreprocessor> preprocessor_;

    friend class RSScreenPreprocessor;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_MANAGER
