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

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <future>

#include <hdi_backend.h>
#include <ipc_callbacks/screen_change_callback.h>
#include <refbase.h>
#include <screen_manager/rs_screen_props.h>
#include <screen_manager/rs_screen_mode_info.h>
#include <screen_manager/rs_screen_capability.h>
#include <screen_manager/rs_screen_data.h>
#include <screen_manager/rs_screen_hdr_capability.h>
#include <screen_manager/screen_types.h>
#include <screen_manager/rs_virtual_screen_resolution.h>
#include <screen_manager/rs_screen_info.h>
#include <surface.h>
#include <surface_type.h>
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
#include "sensor_agent.h"
#include "sensor_agent_type.h"
#endif
#include "rs_screen.h"

namespace OHOS {
namespace Rosen {
class RSScreenManager : public RefBase {
public:
    RSScreenManager() = default;
    virtual ~RSScreenManager() noexcept = default;

    virtual bool Init() noexcept = 0;

    // get default/primary screen id.
    virtual ScreenId GetDefaultScreenId() const = 0;

    virtual std::vector<ScreenId> GetAllScreenIds() const = 0;

    virtual void SetDefaultScreenId(ScreenId id) = 0;

    virtual uint32_t GetCurrentVirtualScreenNum() = 0;

    virtual void SetScreenMirror(ScreenId id, ScreenId toMirror) = 0;

    virtual ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int flags = 0,
        std::vector<uint64_t> whiteList = {}) = 0;

    virtual int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) = 0;

    virtual int32_t AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) = 0;

    virtual int32_t RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) = 0;

    virtual int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<uint64_t>& securityExemptionList) = 0;

    virtual const std::vector<uint64_t> GetVirtualScreenSecurityExemptionList(ScreenId id) const = 0;

    virtual int32_t SetScreenSecurityMask(ScreenId id,
        std::shared_ptr<Media::PixelMap> securityMask) = 0;

    virtual std::shared_ptr<Media::PixelMap> GetScreenSecurityMask(ScreenId id) const = 0;

    virtual int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
        bool supportRotation = false) = 0;

    virtual Rect GetMirrorScreenVisibleRect(ScreenId id) const = 0;

    virtual int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable) = 0;

    virtual const std::unordered_set<uint64_t> GetVirtualScreenBlackList(ScreenId id) const = 0;

    virtual std::unordered_set<uint64_t> GetAllBlackList() const = 0;

    virtual std::unordered_set<uint64_t> GetAllWhiteList() = 0;

    virtual int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) = 0;

    virtual bool GetAndResetVirtualSurfaceUpdateFlag(ScreenId id) const = 0;

    virtual void RemoveVirtualScreen(ScreenId id) = 0;

    virtual uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId) = 0;

    virtual uint32_t SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect) = 0;

    virtual int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) = 0;

    virtual bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation) = 0;

    virtual bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode ScaleMode) = 0;

    virtual int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation) = 0;

    virtual void GetVirtualScreenResolution(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const = 0;

    virtual void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const = 0;

    virtual std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const = 0;

    virtual RSScreenCapability GetScreenCapability(ScreenId id) const = 0;

    virtual ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const = 0;

    virtual ScreenRotation GetScreenCorrection(ScreenId id) const = 0;

    virtual RSScreenData GetScreenData(ScreenId id) const = 0;

    virtual ScreenInfo QueryDefaultScreenInfo() const = 0;

    virtual ScreenInfo QueryScreenInfo(ScreenId id) const = 0;

    // Can only be called after QueryScreenState and the state is ScreenState::SOFTWARE_OUTPUT_ENABLE;
    virtual sptr<Surface> GetProducerSurface(ScreenId id) const = 0;

    virtual bool GetCanvasRotation(ScreenId id) const = 0;

    virtual ScreenScaleMode GetScaleMode(ScreenId id) const = 0;

    // Can only be called after QueryScreenState and the state is ScreenState::HDI_OUTPUT_ENABLE;
    virtual std::shared_ptr<HdiOutput> GetOutput(ScreenId id) const = 0;

    virtual int32_t AddScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback) = 0;

    virtual void RemoveScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback) = 0;

    virtual void ProcessScreenHotPlugEvents() = 0;

    virtual bool TrySimpleProcessHotPlugEvents() = 0;

    virtual void DisplayDump(std::string& dumpString) = 0;

    virtual void SurfaceDump(std::string& dumpString) = 0;

    virtual void FpsDump(std::string& dumpString, std::string& arg) = 0;

    virtual void ClearFpsDump(std::string& dumpString, std::string& arg) = 0;

    virtual void HitchsDump(std::string& dumpString, std::string& arg) = 0;

    virtual int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual int32_t GetScreenBacklight(ScreenId id) const = 0;

    virtual void SetScreenBacklight(ScreenId id, uint32_t level) = 0;

    virtual int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const = 0;

    virtual int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const = 0;

    virtual int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const = 0;

    virtual int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) = 0;

    virtual int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) = 0;

    virtual int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) const = 0;

    virtual int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const = 0;

    virtual int32_t GetScreenType(ScreenId id, RSScreenType& type) const = 0;

    virtual int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) = 0;

    virtual int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const = 0;

    virtual int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat) = 0;

    virtual int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const = 0;

    virtual int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const = 0;

    virtual int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx) = 0;

    virtual int32_t GetScreenSupportedColorSpaces(
        ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const = 0;

    virtual int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const = 0;

    virtual uint32_t GetActualScreensNum() const = 0;

    virtual ScreenInfo GetActualScreenMaxResolution() const = 0;

    virtual int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace) = 0;

    virtual ScreenId GetActiveScreenId() = 0;

    virtual int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate) = 0;

    virtual void SetEqualVsyncPeriod(ScreenId id, bool isEqualVsyncPeriod) = 0;

    /* only used for mock tests */
    virtual void MockHdiScreenConnected(std::unique_ptr<impl::RSScreen>& rsScreen) = 0;

    virtual bool IsAllScreensPowerOff() const = 0;

    // used to skip render frame or render only one frame when screen power is off.
    virtual void MarkPowerOffNeedProcessOneFrame() = 0;

    virtual void ResetPowerOffNeedProcessOneFrame() = 0;

    virtual bool GetPowerOffNeedProcessOneFrame() const = 0;

    virtual bool IsScreenPowerOff(ScreenId id) const = 0;

    virtual void DisablePowerOffRenderControl(ScreenId id) = 0;

    virtual int GetDisableRenderControlScreensCount() const = 0;

    virtual std::unordered_map<ScreenId, std::unordered_set<uint64_t>> GetScreenWhiteList() const = 0;

    virtual int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
        std::vector<uint8_t>& edidData) const = 0;

#ifdef USE_VIDEO_PROCESSING_ENGINE
    virtual float GetScreenBrightnessNits(ScreenId id) const = 0;
#endif

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    virtual void HandlePostureData(const SensorEvent * const event) = 0;
#endif
    virtual void PostForceRefreshTask() = 0;

    virtual void RemoveForceRefreshTask() = 0;

    virtual void ClearFrameBufferIfNeed() = 0;

    virtual int32_t SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type) = 0;

    virtual void GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const = 0;

    virtual bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus) = 0;
    virtual VirtualScreenStatus GetVirtualScreenStatus(ScreenId id) const = 0;

    virtual bool GetDisplayPropertyForHardCursor(uint32_t screenId) = 0;

    virtual bool IsScreenPoweringOn() const = 0;

    virtual bool IsVisibleRectSupportRotation(ScreenId id) const = 0;

    virtual void SetScreenHasProtectedLayer(ScreenId id, bool hasProtectedLayer) = 0;

    virtual void SetScreenSwitchStatus(bool flag) = 0;

    virtual bool IsScreenSwitching() const = 0;

    virtual int32_t SetScreenLinearMatrix(ScreenId id, const std::vector<float>& matrix) = 0;
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

    // noncopyable
    RSScreenManager(const RSScreenManager &) = delete;
    RSScreenManager &operator=(const RSScreenManager &) = delete;

    bool Init() noexcept override;

    ScreenId GetDefaultScreenId() const override;

    std::vector<ScreenId> GetAllScreenIds() const override;

    void SetDefaultScreenId(ScreenId id) override;

    void SetScreenMirror(ScreenId id, ScreenId toMirror) override;

    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId,
        int32_t flags,
        std::vector<uint64_t> whiteList) override;

    int32_t SetVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) override;

    int32_t AddVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) override;

    int32_t RemoveVirtualScreenBlackList(ScreenId id, const std::vector<uint64_t>& blackList) override;

    int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<uint64_t>& securityExemptionList) override;

    const std::vector<uint64_t> GetVirtualScreenSecurityExemptionList(ScreenId id) const override;

    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false) override;

    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask) override;

    std::shared_ptr<Media::PixelMap> GetScreenSecurityMask(ScreenId id) const override;

    Rect GetMirrorScreenVisibleRect(ScreenId id) const override;

    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable) override;

    const std::unordered_set<uint64_t> GetVirtualScreenBlackList(ScreenId id) const override;

    std::unordered_set<uint64_t> GetAllBlackList() const override;

    std::unordered_set<uint64_t> GetAllWhiteList() override;

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) override;

    bool GetAndResetVirtualSurfaceUpdateFlag(ScreenId id) const override;

    void RemoveVirtualScreen(ScreenId id) override;

    uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    uint32_t SetScreenActiveRect(ScreenId id, const GraphicIRect& activeRect) override;

    int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation) override;

    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode ScaleMode) override;

    void GetVirtualScreenResolution(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const override;

    void GetScreenActiveMode(ScreenId id, RSScreenModeInfo& screenModeInfo) const override;

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) const override;

    RSScreenCapability GetScreenCapability(ScreenId id) const override;

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) const override;

    ScreenRotation GetScreenCorrection(ScreenId id) const override;

    RSScreenData GetScreenData(ScreenId id) const  override;

    ScreenInfo QueryDefaultScreenInfo() const override;

    ScreenInfo QueryScreenInfo(ScreenId id) const override;

    sptr<Surface> GetProducerSurface(ScreenId id) const override;

    bool GetCanvasRotation(ScreenId id) const override;

    ScreenScaleMode GetScaleMode(ScreenId id) const override;

    std::shared_ptr<HdiOutput> GetOutput(ScreenId id) const override;

    int32_t AddScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback) override;

    void RemoveScreenChangeCallback(const sptr<RSIScreenChangeCallback> &callback) override;

    void ProcessScreenHotPlugEvents() override;

    bool TrySimpleProcessHotPlugEvents() override;

    void DisplayDump(std::string& dumpString) override;

    void SurfaceDump(std::string& dumpString) override;

    void FpsDump(std::string& dumpString, std::string& arg) override;

    void ClearFpsDump(std::string& dumpString, std::string& arg) override;

    void HitchsDump(std::string& dumpString, std::string& arg) override;

    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height) override;

    int32_t GetScreenBacklight(ScreenId id) const override;

    void SetScreenBacklight(ScreenId id, uint32_t level) override;

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) const override;

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const override;

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) const override;

    uint32_t GetActualScreensNum() const override;

    ScreenInfo GetActualScreenMaxResolution() const override;

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) override;

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) override;

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation) override;

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) const override;

    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const override;

    int32_t GetScreenType(ScreenId id, RSScreenType& type) const override;

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) override;

    int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) const override;

    int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat) override;

    int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const override;

    int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) const override;

    int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx) override;

    int32_t GetScreenSupportedColorSpaces(
        ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const override;

    int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const override;

    int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace) override;

    ScreenId GetActiveScreenId() override;

    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate) override;

    void SetEqualVsyncPeriod(ScreenId id, bool isEqualVsyncPeriod) override;

    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
        std::vector<uint8_t>& edidData) const override;

    /* only used for mock tests */
    void MockHdiScreenConnected(std::unique_ptr<impl::RSScreen>& rsScreen) override
    {
        if (rsScreen == nullptr) {
            return;
        }
        screens_[rsScreen->Id()] = std::move(rsScreen);
    }

    bool IsAllScreensPowerOff() const override;

    // used to skip render frame or render only one frame when screen power is off.
    void MarkPowerOffNeedProcessOneFrame() override;

    void ResetPowerOffNeedProcessOneFrame() override;

    bool GetPowerOffNeedProcessOneFrame() const override;

    bool IsScreenPowerOff(ScreenId id) const override;

    void DisablePowerOffRenderControl(ScreenId id) override;

    int GetDisableRenderControlScreensCount() const override;

#ifdef USE_VIDEO_PROCESSING_ENGINE
    float GetScreenBrightnessNits(ScreenId id) const override;
#endif

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    void HandlePostureData(const SensorEvent * const event) override;
#endif
    void PostForceRefreshTask() override;

    void RemoveForceRefreshTask() override;

    void ClearFrameBufferIfNeed() override;

    int32_t SetScreenConstraint(ScreenId id, uint64_t timestamp, ScreenConstraintType type) override;

    void GetDefaultScreenActiveMode(RSScreenModeInfo& screenModeInfo) const override;

    bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus) override;
    VirtualScreenStatus GetVirtualScreenStatus(ScreenId id) const override;

    static void ReleaseScreenDmaBuffer(uint64_t screenId);

    bool GetDisplayPropertyForHardCursor(uint32_t screenId) override;

    uint32_t GetCurrentVirtualScreenNum() override
    {
        return currentVirtualScreenNum_;
    }

    bool IsScreenPoweringOn() const override
    {
        return isScreenPoweringOn_;
    }

    void SetScreenHasProtectedLayer(ScreenId id, bool hasProtectedLayer) override;

    bool IsVisibleRectSupportRotation(ScreenId id) const override;

    void SetScreenSwitchStatus(bool flag) override;

    bool IsScreenSwitching() const override;

    int32_t SetScreenLinearMatrix(ScreenId id, const std::vector<float>& matrix) override;
    // Get all whiteList and their screenId
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> GetScreenWhiteList() const override;

private:
    RSScreenManager();
    ~RSScreenManager() noexcept override;

    static void OnHotPlug(std::shared_ptr<HdiOutput> &output, bool connected, void *data);
    void OnHotPlugEvent(std::shared_ptr<HdiOutput> &output, bool connected);
    static void OnRefresh(ScreenId id, void *data);
    void OnRefreshEvent(ScreenId id);
    static void OnHwcDead(void *data);
    void OnHwcDeadEvent();
    static void OnScreenVBlankIdle(uint32_t devId, uint64_t ns, void *data);
    void OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns);
    void CleanAndReinit();
    void ProcessScreenConnected(std::shared_ptr<HdiOutput> &output);
    void AddScreenToHgm(std::shared_ptr<HdiOutput> &output);
    void ProcessScreenDisConnected(std::shared_ptr<HdiOutput> &output);
    void RemoveScreenFromHgm(std::shared_ptr<HdiOutput> &output);
    void ProcessPendingConnections();
    void HandleDefaultScreenDisConnected();
    void ForceRefreshOneFrame() const;

    void GetVirtualScreenResolutionLocked(ScreenId id, RSVirtualScreenResolution& virtualScreenResolution) const;
    RSScreenCapability GetScreenCapabilityLocked(ScreenId id) const;
    ScreenRotation GetScreenCorrectionLocked(ScreenId id) const;

    void RemoveVirtualScreenLocked(ScreenId id);
    ScreenId GenerateVirtualScreenIdLocked();
    void ReuseVirtualScreenIdLocked(ScreenId id);

    int32_t GetScreenSupportedColorGamutsLocked(ScreenId id, std::vector<ScreenColorGamut>& mode) const;
    int32_t GetScreenSupportedMetaDataKeysLocked(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) const;
    int32_t GetScreenColorGamutLocked(ScreenId id, ScreenColorGamut& mode) const;
    int32_t GetScreenGamutMapLocked(ScreenId id, ScreenGamutMap& mode) const;
    int32_t GetScreenHDRCapabilityLocked(ScreenId id, RSScreenHDRCapability& screenHdrCapability) const;
    int32_t GetScreenTypeLocked(ScreenId id, RSScreenType& type) const;
    int32_t GetPixelFormatLocked(ScreenId id, GraphicPixelFormat& pixelFormat) const;
    int32_t SetPixelFormatLocked(ScreenId id, GraphicPixelFormat pixelFormat);
    int32_t GetScreenSupportedHDRFormatsLocked(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) const;
    int32_t GetScreenHDRFormatLocked(ScreenId id, ScreenHDRFormat& hdrFormat) const;
    int32_t SetScreenHDRFormatLocked(ScreenId id, int32_t modeIdx);
    int32_t GetScreenSupportedColorSpacesLocked(ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const;
    int32_t GetScreenColorSpaceLocked(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) const;
    ScreenInfo QueryScreenInfoLocked(ScreenId id) const;
    bool GetCastScreenEnableSkipWindow(ScreenId id) const;

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    void RegisterSensorCallback();
    void UnRegisterSensorCallback();
    void HandleSensorData(float angle);
    FoldState TransferAngleToScreenState(float angle);
#endif

    void RegSetScreenVsyncEnabledCallbackForMainThread(ScreenId vsyncEnabledScreenId);
    void RegSetScreenVsyncEnabledCallbackForHardwareThread(ScreenId vsyncEnabledScreenId);
    void UpdateVsyncEnabledScreenId(ScreenId screenId);

    void TriggerCallbacks(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason = ScreenChangeReason::DEFAULT) const;
    std::shared_ptr<OHOS::Rosen::RSScreen> GetScreen(ScreenId id) const;
    void UpdateFoldScreenConnectStatusLocked(ScreenId screenId, bool connected);
    uint64_t JudgeVSyncEnabledScreenWhileHotPlug(ScreenId screenId, bool connected);
    uint64_t JudgeVSyncEnabledScreenWhilePowerStatusChanged(ScreenId screenId, ScreenPowerStatus status);

    mutable std::mutex mutex_;
    mutable std::mutex renderControlMutex_;
    HdiBackend *composer_ = nullptr;
    std::atomic<ScreenId> defaultScreenId_ = INVALID_SCREEN_ID;
    std::map<ScreenId, std::shared_ptr<OHOS::Rosen::RSScreen>> screens_;
    std::queue<ScreenId> freeVirtualScreenIds_;
    uint32_t virtualScreenCount_ = 0;
    uint32_t currentVirtualScreenNum_ = 0;

    mutable std::shared_mutex screenChangeCallbackMutex_;
    std::vector<sptr<RSIScreenChangeCallback>> screenChangeCallbacks_;
    std::atomic<bool> mipiCheckInFirstHotPlugEvent_ = false;
    std::atomic<bool> isHwcDead_ = false;

    mutable std::mutex hotPlugAndConnectMutex_;
    std::map<ScreenId, ScreenHotPlugEvent> pendingHotPlugEvents_;
    std::vector<ScreenId> pendingConnectedIds_;

    mutable std::shared_mutex powerStatusMutex_;
    std::unordered_map<ScreenId, uint32_t> screenPowerStatus_;

    mutable std::shared_mutex backLightAndCorrectionMutex_;
    std::unordered_map<ScreenId, uint32_t> screenBacklight_;
    std::unordered_map<ScreenId, ScreenRotation> screenCorrection_;

    std::unordered_set<uint64_t> castScreenBlackList_ = {};

    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::RSScreenManager> instance_;

    uint64_t frameId_ = 0;
    std::atomic<bool> powerOffNeedProcessOneFrame_ = false;
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
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> screenWhiteList_;
};
} // namespace impl
} // namespace Rosen
} // namespace OHOS
#endif // RS_SCREEN_MANAGER
