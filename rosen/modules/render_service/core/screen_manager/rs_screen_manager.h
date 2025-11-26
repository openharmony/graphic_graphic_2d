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
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "hdi_backend.h"
#include <ipc_callbacks/screen_change_callback.h>
#include <ipc_callbacks/screen_switching_notify_callback.h>
#include <refbase.h>
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
#include <sensor_agent.h>
#include <sensor_agent_type.h>
#endif
#include <surface.h>
#include <surface_type.h>

#include <screen_manager/rs_screen_props.h>
#include <screen_manager/rs_screen_mode_info.h>
#include <screen_manager/rs_screen_capability.h>
#include <screen_manager/rs_screen_data.h>
#include <screen_manager/rs_screen_hdr_capability.h>
#include <screen_manager/screen_types.h>
#include <screen_manager/rs_virtual_screen_resolution.h>
#include <screen_manager/rs_screen_property.h>

namespace OHOS {
namespace Rosen {
class RSScreen;
class RSIScreenNodeListener;
struct ScreenHotPlugEvent {
    std::shared_ptr<HdiOutput> output;
    bool connected = false;
};

enum class FoldState : uint32_t {
    UNKNOW,
    FOLDED,
    EXPAND
};

class RSScreenManager : public RefBase {
public:
    static sptr<OHOS::Rosen::RSScreenManager> GetInstance() noexcept;

    bool Init() noexcept;
    void ProcessScreenHotPlugEvents(bool needPost = false);
    bool TrySimpleProcessHotPlugEvents();
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    void HandlePostureData(const SensorEvent* const event);
#endif

    ScreenId GetDefaultScreenId() const;
    ScreenId GetActiveScreenId();
    std::vector<ScreenId> GetAllScreenIds() const;

    ScreenInfo QueryScreenInfo(ScreenId id) const;
    ScreenInfo QueryDefaultScreenInfo() const;

    sptr<RSScreenProperty> QueryScreenProperty(ScreenId id) const;

    int32_t GetScreenType(ScreenId id, RSScreenType& type) const;
    ScreenInfo GetActualScreenMaxResolution() const;

    int32_t AddScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback);
    void RemoveScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback);
    int32_t SetScreenSwitchingNotifyCallback(const sptr<RSIScreenSwitchingNotifyCallback>& callback);
    void RegisterScreenNodeListener(std::shared_ptr<RSIScreenNodeListener> listener);

    void DisplayDump(std::string& dumpString);
    void SurfaceDump(std::string& dumpString);
    void DumpCurrentFrameLayers();
    void FpsDump(std::string& dumpString, std::string& arg);
    void ClearFpsDump(std::string& dumpString, std::string& arg);
    void HitchsDump(std::string& dumpString, std::string& arg);

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval);
    void PostForceRefreshTask();
    void RemoveForceRefreshTask();

    void ClearFrameBufferIfNeed();
    void ReleaseScreenDmaBuffer(ScreenId screenId);

    /* only used for mock tests */
    void MockHdiScreenConnected(std::shared_ptr<OHOS::Rosen::RSScreen> rsScreen);

    // physical screen
    std::shared_ptr<HdiOutput> GetOutput(ScreenId id) const;

    uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId);
    void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const;
    void GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const;
    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const;
    uint32_t GetScreenActiveRefreshRate(ScreenId id) const;

    RSScreenCapability GetScreenCapability(ScreenId id) const;
    RSScreenData GetScreenData(ScreenId id) const;
    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) const;

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation);
    ScreenRotation GetScreenCorrection(ScreenId id) const;

    int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height);
    int32_t GetRogScreenResolution(ScreenId id, uint32_t& width, uint32_t& height);
    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const;
    void WaitScreenPowerStatusTask();
    bool IsScreenPoweringOn() const;
    bool IsScreenPoweringOff(ScreenId id) const;
    bool IsScreenPowerOff(ScreenId id) const;
    bool IsAllScreensPowerOff() const;
    void DisablePowerOffRenderControl(ScreenId id);
    int GetDisableRenderControlScreensCount() const;
    // used to skip render frame or render only one frame when screen power is off.
    void MarkPowerOffNeedProcessOneFrame();
    void ResetPowerOffNeedProcessOneFrame();
    bool GetPowerOffNeedProcessOneFrame() const;

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

    void SetScreenSwitchStatus(bool flag);
    bool IsScreenSwitching() const;

    uint32_t SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect);
    void SetScreenHasProtectedLayer(ScreenId id, bool hasProtectedLayer);
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
    VirtualScreenStatus GetVirtualScreenStatus(ScreenId id) const;

    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable);
    int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    int32_t SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList);
    int32_t AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    int32_t RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList);
    std::unordered_set<uint64_t> GetVirtualScreenBlackList(ScreenId id) const;
    std::unordered_set<uint8_t> GetVirtualScreenTypeBlackList(ScreenId id) const;
    std::unordered_set<uint64_t> GetAllBlackList() const;
    std::unordered_set<uint64_t> GetAllWhiteList();
    std::unordered_set<uint64_t> GetBlackListVirtualScreenByNode(uint64_t nodeId);

    int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<uint64_t>& securityExemptionList);
    const std::vector<uint64_t> GetVirtualScreenSecurityExemptionList(ScreenId id) const;

    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask);
    std::shared_ptr<Media::PixelMap> GetScreenSecurityMask(ScreenId id) const;

    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false);
    Rect GetMirrorScreenVisibleRect(ScreenId id) const;
    bool IsVisibleRectSupportRotation(ScreenId id);

    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate);
    // Get all whiteList and their screenId
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> GetScreenWhiteList() const;

    void SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY);

    bool CheckVirtualScreenStatusChanged(ScreenId id);
    void RegisterHwcEvent(std::function<void()> func);

private:
    RSScreenManager() = default;
    ~RSScreenManager() override = default;
    RSScreenManager(const RSScreenManager&) = delete;
    RSScreenManager& operator=(const RSScreenManager&) = delete;

    static void OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected, void *data);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput>& output, bool connected);

    static void OnRefresh(ScreenId id, void *data);
    void OnRefreshEvent(ScreenId id);

    static void OnHwcDead(void *data);
    void OnHwcDeadEvent();
    void CleanAndReinit();

    static void OnScreenVBlankIdle(uint32_t devId, uint64_t ns, void *data);
    void OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns);

    void PrintScreenBlackList(
        std::string funcName, ScreenId id, const std::unordered_set<uint64_t> &set) const;

    // physical screen
    bool CheckFoldScreenIdBuiltIn(ScreenId id);
    void ProcessScreenConnected(std::shared_ptr<HdiOutput>& output);
    void ProcessPendingConnections();
    void ProcessScreenDisConnected(std::shared_ptr<HdiOutput>& output);
    void HandleDefaultScreenDisConnected();

    void UpdateScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    void ResetScreenPowerStatusTask();

    void RegSetScreenVsyncEnabledCallbackForMainThread(ScreenId vsyncEnabledScreenId);
    void RegSetScreenVsyncEnabledCallbackForHardwareThread(ScreenId vsyncEnabledScreenId);
    void UpdateVsyncEnabledScreenId(ScreenId screenId);
    void UpdateFoldScreenConnectStatusLocked(ScreenId screenId, bool connected);
    uint64_t JudgeVSyncEnabledScreenWhileHotPlug(ScreenId screenId, bool connected);
    uint64_t JudgeVSyncEnabledScreenWhilePowerStatusChanged(ScreenId screenId, ScreenPowerStatus status);
    void ProcessVSyncScreenIdWhilePowerStatusChanged(ScreenId id, ScreenPowerStatus status);

    void AddScreenToHgm(std::shared_ptr<HdiOutput>& output);
    void RemoveScreenFromHgm(std::shared_ptr<HdiOutput>& output);

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    void InitFoldSensor();
    void RegisterSensorCallback();
    void UnRegisterSensorCallback();
    static void OnBootComplete(const char* key, const char* value, void *context);
    void OnBootCompleteEvent();
    void HandleSensorData(float angle);
    FoldState TransferAngleToScreenState(float angle);
#endif

    std::shared_ptr<OHOS::Rosen::RSScreen> GetScreen(ScreenId id) const;
    void TriggerCallbacks(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason = ScreenChangeReason::DEFAULT) const;
    void NotifyScreenNodeChange(ScreenId id, bool connected) const;
    void NotifySwitchingCallback(bool status) const;

    // virtual screen
    ScreenId GenerateVirtualScreenId();
    void ForceRefreshOneFrame() const;

    void OnScreenPropertyChanged(const sptr<RSScreenProperty>& property);

    mutable std::mutex screenMapMutex_;
    std::map<ScreenId, std::shared_ptr<OHOS::Rosen::RSScreen>> screens_;
    using ScreenNode = decltype(screens_)::value_type;
    bool AnyScreenFits(std::function<bool(const ScreenNode&)> func) const;

    HdiBackend *composer_ = nullptr;
    std::atomic<ScreenId> defaultScreenId_ = INVALID_SCREEN_ID;

    std::mutex virtualScreenIdMutex_;
    std::queue<ScreenId> freeVirtualScreenIds_;
    std::atomic<uint32_t> virtualScreenCount_ = 0;
    std::atomic<uint32_t> currentVirtualScreenNum_ = 0;

    mutable std::shared_mutex screenChangeCallbackMutex_;
    std::vector<sptr<RSIScreenChangeCallback>> screenChangeCallbacks_;
    mutable std::shared_mutex screenSwitchingNotifyCallbackMutex_;
    sptr<RSIScreenSwitchingNotifyCallback> screenSwitchingNotifyCallback_;
    std::shared_ptr<RSIScreenNodeListener> screenNodeListener_;

    std::atomic<bool> mipiCheckInFirstHotPlugEvent_ = false;
    std::atomic<bool> isHwcDead_ = false;
    std::condition_variable hwcDeadCV_;
    std::condition_variable composerBlockedCV_;

    mutable std::mutex hotPlugAndConnectMutex_;
    std::map<ScreenId, ScreenHotPlugEvent> pendingHotPlugEvents_;
    std::vector<ScreenId> pendingConnectedIds_;

    mutable std::shared_mutex powerStatusMutex_;
    std::unordered_map<ScreenId, uint32_t> screenPowerStatus_;
    std::unordered_set<ScreenId> isScreenPoweringOff_;

    std::mutex syncTaskMutex_;
    std::condition_variable statusTaskCV_;
    bool statusTaskEndFlag_ = false;

    mutable std::shared_mutex backLightAndCorrectionMutex_;
    std::unordered_map<ScreenId, uint32_t> screenBacklight_;
    std::unordered_map<ScreenId, ScreenRotation> screenCorrection_;

    mutable std::mutex blackListMutex_;
    std::unordered_set<uint64_t> castScreenBlackList_ = {};
    // a blacklist node may exist in multiple virtual screens
    std::unordered_map<uint64_t, std::unordered_set<ScreenId>> blackListInVirtualScreen_ = {};

    uint64_t frameId_ = 0; // only used by SetScreenConstraint, called in hardware thread per frame

    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::RSScreenManager> instance_;

    std::atomic<bool> powerOffNeedProcessOneFrame_ = false;

    std::function<void()> registerHwcEventFunc_ = nullptr;

    mutable std::mutex renderControlMutex_;
    std::unordered_set<ScreenId> disableRenderControlScreens_ = {};

    bool isScreenPoweringOn_ = false;
    std::atomic<bool> isScreenSwitching_ = false;

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    SensorUser user_;
    bool isFoldScreenFlag_ = false;
    ScreenId innerScreenId_ = 0;
    ScreenId externalScreenId_ = INVALID_SCREEN_ID;
    ScreenId activeScreenId_ = 0;
    bool isPostureSensorDataHandled_ = false;
    std::condition_variable activeScreenIdAssignedCV_;
    mutable std::mutex activeScreenIdAssignedMutex_;
    mutable std::mutex registerSensorMutex_;
    bool hasRegisterSensorCallback_ = false;
#endif
    struct FoldScreenStatus {
        bool isConnected;
        bool isPowerOn;
    };
    std::unordered_map<uint64_t, FoldScreenStatus> foldScreenIds_; // screenId, FoldScreenStatus
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> screenWhiteList_;
};

sptr<RSScreenManager> CreateOrGetScreenManager();
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_MANAGER
