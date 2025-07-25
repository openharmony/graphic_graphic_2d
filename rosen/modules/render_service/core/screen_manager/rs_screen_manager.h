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

#include <hdi_backend.h>
#include <ipc_callbacks/screen_change_callback.h>
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
#include <screen_manager/rs_screen_info.h>

namespace OHOS {
namespace Rosen {
struct LoadOptParamsForScreen {
    LoadOptParamsForHdiBackend loadOptParamsForHdiBackend;
};

class RSScreen;
class RSIScreenNodeListener;
class RSScreenManager : public RefBase {
public:
    RSScreenManager() = default;
    virtual ~RSScreenManager() noexcept = default;

    virtual bool Init() noexcept = 0;
    virtual void ProcessScreenHotPlugEvents() = 0;
    virtual bool TrySimpleProcessHotPlugEvents() = 0;
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    virtual void HandlePostureData(const SensorEvent* const event) = 0;
#endif

    virtual void SetDefaultScreenId(ScreenId id) = 0;
    virtual ScreenId GetDefaultScreenId() const = 0;
    virtual ScreenId GetActiveScreenId() = 0;
    virtual std::vector<ScreenId> GetAllScreenIds() const = 0;

    virtual ScreenInfo QueryScreenInfo(ScreenId id) const = 0;
    virtual ScreenInfo QueryDefaultScreenInfo() const = 0;

    virtual int32_t GetScreenType(ScreenId id, RSScreenType& type) const = 0;
    virtual ScreenInfo GetActualScreenMaxResolution() const = 0;

    virtual int32_t AddScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback) = 0;
    virtual void RemoveScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback) = 0;
    virtual void RegisterScreenNodeListener(std::shared_ptr<RSIScreenNodeListener> listener) = 0;

    virtual void DisplayDump(std::string& dumpString) = 0;
    virtual void SurfaceDump(std::string& dumpString) = 0;
    virtual void DumpCurrentFrameLayers() = 0;
    virtual void FpsDump(std::string& dumpString, std::string& arg) = 0;
    virtual void ClearFpsDump(std::string& dumpString, std::string& arg) = 0;
    virtual void HitchsDump(std::string& dumpString, std::string& arg) = 0;

    virtual int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) = 0;
    virtual void SetEqualVsyncPeriod(ScreenId id, bool isEqualVsyncPeriod) = 0;
    virtual void PostForceRefreshTask() = 0;
    virtual void RemoveForceRefreshTask() = 0;

    virtual void ClearFrameBufferIfNeed() = 0;
    virtual void ReleaseScreenDmaBuffer(ScreenId id) = 0;

    /* only used for mock tests */
    virtual void MockHdiScreenConnected(std::shared_ptr<RSScreen> rsScreen) = 0;

    // physical screen
    virtual std::shared_ptr<HdiOutput> GetOutput(ScreenId id) const = 0;

    virtual uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId) = 0;
    virtual void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const = 0;
    virtual void GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const = 0;
    virtual std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const = 0;

    virtual RSScreenCapability GetScreenCapability(ScreenId id) const = 0;
    virtual RSScreenData GetScreenData(ScreenId id) const  = 0;
    virtual int32_t GetDisplayIdentificationData(
        ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) const = 0;

    virtual int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation) = 0;
    virtual ScreenRotation GetScreenCorrection(ScreenId id) const = 0;

    virtual int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;
    virtual int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) = 0;
    virtual ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const = 0;
    virtual void WaitScreenPowerStatusTask() = 0;
    virtual bool IsScreenPoweringOn() const = 0;
    virtual bool IsScreenPoweringOff(ScreenId id) const = 0;
    virtual bool IsScreenPowerOff(ScreenId id) const = 0;
    virtual bool IsAllScreensPowerOff() const = 0;
    virtual void DisablePowerOffRenderControl(ScreenId id) = 0;
    virtual int GetDisableRenderControlScreensCount() const = 0;
    // used to skip render frame or render only one frame when screen power is off.
    virtual void MarkPowerOffNeedProcessOneFrame() = 0;
    virtual void ResetPowerOffNeedProcessOneFrame() = 0;
    virtual bool GetPowerOffNeedProcessOneFrame() const = 0;

    virtual void SetScreenBacklight(ScreenId id, uint32_t level) = 0;
    virtual int32_t GetScreenBacklight(ScreenId id) const = 0;
    virtual int32_t SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type) = 0;

    virtual int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat) = 0;
    virtual int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const = 0;

    virtual int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) = 0;
    virtual int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const = 0;
    virtual int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const = 0;
    virtual int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) = 0;
    virtual int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) const = 0;

    virtual int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx) = 0;
    virtual int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const = 0;
    virtual int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const = 0;
    virtual int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const = 0;
    virtual int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const = 0;

    virtual int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace) = 0;
    virtual int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const = 0;
    virtual int32_t GetScreenSupportedColorSpaces(
        ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const = 0;

    virtual void SetScreenSwitchStatus(bool flag) = 0;
    virtual bool IsScreenSwitching() const = 0;

    virtual uint32_t SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect) = 0;
    virtual void SetScreenHasProtectedLayer(ScreenId id, bool hasProtectedLayer) = 0;
    virtual int32_t SetScreenLinearMatrix(ScreenId id, const std::vector<float>& matrix) = 0;
    virtual bool GetDisplayPropertyForHardCursor(uint32_t screenId) = 0;

    // virtual screen
    virtual ScreenId CreateVirtualScreen(const std::string& name, uint32_t width, uint32_t height,
        sptr<Surface> surface, ScreenId mirrorId = 0, int32_t flags = 0, std::vector<uint64_t> whiteList = {}) = 0;
    virtual void RemoveVirtualScreen(ScreenId id) = 0;
    virtual uint32_t GetCurrentVirtualScreenNum() = 0;

    virtual int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) = 0;
    virtual sptr<Surface> GetProducerSurface(ScreenId id) const = 0;
    virtual bool GetAndResetVirtualSurfaceUpdateFlag(ScreenId id) const = 0;

    virtual int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height) = 0;
    virtual int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;
    virtual void GetVirtualScreenResolution(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const = 0;

    virtual bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation) = 0;
    virtual bool GetCanvasRotation(ScreenId id) const = 0;

    virtual int32_t SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation) = 0;
    virtual bool GetVirtualScreenAutoRotation(ScreenId id) const = 0;

    virtual bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode ScaleMode) = 0;
    virtual ScreenScaleMode GetScaleMode(ScreenId id) const = 0;

    virtual bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus) = 0;
    virtual VirtualScreenStatus GetVirtualScreenStatus(ScreenId id) const = 0;

    virtual int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable) = 0;
    virtual int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) = 0;
    virtual int32_t SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList) = 0;
    virtual int32_t AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) = 0;
    virtual int32_t RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) = 0;
    virtual const std::unordered_set<uint64_t> GetVirtualScreenBlackList(ScreenId id) const = 0;
    virtual const std::unordered_set<uint8_t> GetVirtualScreenTypeBlackList(ScreenId id) const = 0;
    virtual std::unordered_set<uint64_t> GetAllBlackList() const = 0;
    virtual std::unordered_set<uint64_t> GetAllWhiteList() = 0;
    virtual std::unordered_set<uint64_t> GetBlackListVirtualScreenByNode(uint64_t nodeId) = 0;

    virtual int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<uint64_t>& securityExemptionList) = 0;
    virtual const std::vector<uint64_t> GetVirtualScreenSecurityExemptionList(ScreenId id) const = 0;

    virtual int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask) = 0;
    virtual std::shared_ptr<Media::PixelMap> GetScreenSecurityMask(ScreenId id) const = 0;

    virtual int32_t SetMirrorScreenVisibleRect(
        ScreenId id, const Rect& mainScreenRect, bool supportRotation = false) = 0;
    virtual Rect GetMirrorScreenVisibleRect(ScreenId id) const = 0;
    virtual bool IsVisibleRectSupportRotation(ScreenId id) = 0;
    virtual int32_t GetVirtualScreenSecLayerOption(ScreenId id) const = 0;

    virtual int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate) = 0;
    
    virtual void SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY) = 0;

    virtual std::unordered_map<ScreenId, std::unordered_set<uint64_t>> GetScreenWhiteList() const = 0;

    virtual void InitLoadOptParams(LoadOptParamsForScreen& loadOptParamsForScreen) = 0;

    virtual bool CheckPSurfaceChanged(ScreenId id) = 0;
};

sptr<RSScreenManager> CreateOrGetScreenManager();

namespace impl {
struct ScreenHotPlugEvent {
    std::shared_ptr<HdiOutput> output;
    bool connected = false;
};

enum class FoldState : uint32_t {
    UNKNOW,
    FOLDED,
    EXPAND
};

class RSScreenManager : public OHOS::Rosen::RSScreenManager {
public:
    static sptr<OHOS::Rosen::RSScreenManager> GetInstance() noexcept;

    bool Init() noexcept override;
    void ProcessScreenHotPlugEvents() override;
    bool TrySimpleProcessHotPlugEvents() override;
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    void HandlePostureData(const SensorEvent* const event) override;
#endif

    void SetDefaultScreenId(ScreenId id) override;
    ScreenId GetDefaultScreenId() const override;
    ScreenId GetActiveScreenId() override;
    std::vector<ScreenId> GetAllScreenIds() const override;

    ScreenInfo QueryScreenInfo(ScreenId id) const override;
    ScreenInfo QueryDefaultScreenInfo() const override;

    int32_t GetScreenType(ScreenId id, RSScreenType& type) const override;
    ScreenInfo GetActualScreenMaxResolution() const override;

    int32_t AddScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback) override;
    void RemoveScreenChangeCallback(const sptr<RSIScreenChangeCallback>& callback) override;
    void RegisterScreenNodeListener(std::shared_ptr<RSIScreenNodeListener> listener) override;

    void DisplayDump(std::string& dumpString) override;
    void SurfaceDump(std::string& dumpString) override;
    void DumpCurrentFrameLayers() override;
    void FpsDump(std::string& dumpString, std::string& arg) override;
    void ClearFpsDump(std::string& dumpString, std::string& arg) override;
    void HitchsDump(std::string& dumpString, std::string& arg) override;

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) override;
    void SetEqualVsyncPeriod(ScreenId id, bool isEqualVsyncPeriod) override;
    void PostForceRefreshTask() override;
    void RemoveForceRefreshTask() override;

    void ClearFrameBufferIfNeed() override;
    void ReleaseScreenDmaBuffer(ScreenId screenId) override;

    /* only used for mock tests */
    void MockHdiScreenConnected(std::shared_ptr<OHOS::Rosen::RSScreen> rsScreen) override;

    // physical screen
    std::shared_ptr<HdiOutput> GetOutput(ScreenId id) const override;

    uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId) override;
    void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const override;
    void GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const override;
    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const override;

    RSScreenCapability GetScreenCapability(ScreenId id) const override;
    RSScreenData GetScreenData(ScreenId id) const  override;
    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) const override;

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation) override;
    ScreenRotation GetScreenCorrection(ScreenId id) const override;

    int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;
    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;
    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const override;
    void WaitScreenPowerStatusTask() override;
    bool IsScreenPoweringOn() const override;
    bool IsScreenPoweringOff(ScreenId id) const override;
    bool IsScreenPowerOff(ScreenId id) const override;
    bool IsAllScreensPowerOff() const override;
    void DisablePowerOffRenderControl(ScreenId id) override;
    int GetDisableRenderControlScreensCount() const override;
    // used to skip render frame or render only one frame when screen power is off.
    void MarkPowerOffNeedProcessOneFrame() override;
    void ResetPowerOffNeedProcessOneFrame() override;
    bool GetPowerOffNeedProcessOneFrame() const override;

    void SetScreenBacklight(ScreenId id, uint32_t level) override;
    int32_t GetScreenBacklight(ScreenId id) const override;
    int32_t SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type) override;

    int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat) override;
    int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const override;

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) override;
    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const override;
    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const override;
    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) override;
    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) const override;

    int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx) override;
    int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const override;
    int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const override;
    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const override;
    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const override;

    int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace) override;
    int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const override;
    int32_t GetScreenSupportedColorSpaces(
        ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const override;

    void SetScreenSwitchStatus(bool flag) override;
    bool IsScreenSwitching() const override;

    uint32_t SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect) override;
    void SetScreenHasProtectedLayer(ScreenId id, bool hasProtectedLayer) override;
    int32_t SetScreenLinearMatrix(ScreenId id, const std::vector<float>& matrix) override;
    bool GetDisplayPropertyForHardCursor(uint32_t screenId) override;

    // virtual screen
    ScreenId CreateVirtualScreen(const std::string& name, uint32_t width, uint32_t height, sptr<Surface> surface,
        ScreenId mirrorId, int32_t flags, std::vector<uint64_t> whiteList) override;
    void RemoveVirtualScreen(ScreenId id) override;
    uint32_t GetCurrentVirtualScreenNum() override;

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) override;
    sptr<Surface> GetProducerSurface(ScreenId id) const override;
    bool GetAndResetVirtualSurfaceUpdateFlag(ScreenId id) const override;

    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height) override;
    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;
    void GetVirtualScreenResolution(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const override;

    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation) override;
    bool GetCanvasRotation(ScreenId id) const override;

    int32_t SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation) override;
    bool GetVirtualScreenAutoRotation(ScreenId id) const override;

    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode ScaleMode) override;
    ScreenScaleMode GetScaleMode(ScreenId id) const override;

    bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus) override;
    VirtualScreenStatus GetVirtualScreenStatus(ScreenId id) const override;

    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable) override;
    int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) override;
    int32_t SetVirtualScreenTypeBlackList(ScreenId id, const std::vector<uint8_t>& typeBlackList) override;
    int32_t AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) override;
    int32_t RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) override;
    const std::unordered_set<uint64_t> GetVirtualScreenBlackList(ScreenId id) const override;
    const std::unordered_set<uint8_t> GetVirtualScreenTypeBlackList(ScreenId id) const override;
    std::unordered_set<uint64_t> GetAllBlackList() const override;
    std::unordered_set<uint64_t> GetAllWhiteList() override;
    std::unordered_set<uint64_t> GetBlackListVirtualScreenByNode(uint64_t nodeId) override;

    int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<uint64_t>& securityExemptionList) override;
    const std::vector<uint64_t> GetVirtualScreenSecurityExemptionList(ScreenId id) const override;

    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask) override;
    std::shared_ptr<Media::PixelMap> GetScreenSecurityMask(ScreenId id) const override;

    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false) override;
    Rect GetMirrorScreenVisibleRect(ScreenId id) const override;
    bool IsVisibleRectSupportRotation(ScreenId id) override;
    int32_t GetVirtualScreenSecLayerOption(ScreenId id) const override;

    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate) override;
    void SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY) override;

    // Get all whiteList and their screenId
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> GetScreenWhiteList() const override;

    void InitLoadOptParams(LoadOptParamsForScreen& loadOptParamsForScreen) override;
    bool CheckPSurfaceChanged(ScreenId id) override;
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

    // physical screen
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
    void RegisterSensorCallback();
    void UnRegisterSensorCallback();
    void HandleSensorData(float angle);
    FoldState TransferAngleToScreenState(float angle);
#endif

    std::shared_ptr<OHOS::Rosen::RSScreen> GetScreen(ScreenId id) const;
    void TriggerCallbacks(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason = ScreenChangeReason::DEFAULT) const;
    void NotifyScreenNodeChange(ScreenId id, bool connected) const;

    // virtual screen
    ScreenId GenerateVirtualScreenId();
    void ForceRefreshOneFrame() const;

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
    std::shared_ptr<RSIScreenNodeListener> screenNodeListener_;

    std::atomic<bool> mipiCheckInFirstHotPlugEvent_ = false;
    std::atomic<bool> isHwcDead_ = false;

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

    mutable std::mutex typeBlackListMutex_;
    std::unordered_set<uint8_t> castScreenTypeBlackList_ = {};

    uint64_t frameId_ = 0; // only used by SetScreenConstraint, called in hardware thread per frame

    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::RSScreenManager> instance_;

    std::atomic<bool> powerOffNeedProcessOneFrame_ = false;

    mutable std::mutex renderControlMutex_;
    std::unordered_set<ScreenId> disableRenderControlScreens_ = {};

    bool isScreenPoweringOn_ = false;
    std::atomic<bool> isScreenSwitching_ = false;

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    SensorUser user;
    bool isFoldScreenFlag_ = false;
    ScreenId innerScreenId_ = 0;
    ScreenId externalScreenId_ = INVALID_SCREEN_ID;
    ScreenId activeScreenId_ = 0;
    bool isFirstTimeToGetActiveScreenId_ = true;
    bool isPostureSensorDataHandled_ = false;
    std::condition_variable activeScreenIdAssignedCV_;
    mutable std::mutex activeScreenIdAssignedMutex_;
#endif
    struct FoldScreenStatus {
        bool isConnected;
        bool isPowerOn;
    };
    std::unordered_map<uint64_t, FoldScreenStatus> foldScreenIds_; // screenId, FoldScreenStatus

    mutable std::mutex whiteListMutex_;
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> screenWhiteList_;

    LoadOptParamsForScreen loadOptParamsForScreen_ = {};
};
} // namespace impl
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_MANAGER
