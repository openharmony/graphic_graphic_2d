/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RS_SCREEN
#define RS_SCREEN

#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_set>

#include <hdi_display_type.h>
#include <hdi_output.h>
#include <hdi_screen.h>
#include <surface_type.h>

#include <common/rs_rect.h>
#include <screen_manager/screen_types.h>

namespace OHOS {
namespace Rosen {
struct VirtualScreenConfigs {
    ScreenId id = INVALID_SCREEN_ID;
    ScreenId mirrorId = INVALID_SCREEN_ID;
    std::string name;
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<Surface> surface = nullptr;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
    int32_t flags = 0; // reserve flag.
    std::unordered_set<uint64_t> whiteList = {};
};

class RSScreen {
public:
    RSScreen() = default;
    virtual ~RSScreen() noexcept = default;

    virtual ScreenId Id() const = 0;
    virtual const std::string& Name() const = 0;
    virtual void SetMirror(ScreenId mirroredId) = 0;
    virtual ScreenId MirroredId() const = 0;

    virtual bool IsEnable() const = 0;
    virtual bool IsVirtual() const = 0;
    virtual const RSScreenType& GetScreenType() const = 0;

    virtual int32_t SetResolution(uint32_t width, uint32_t height) = 0;
    virtual void SetRogResolution(uint32_t width, uint32_t height) = 0;
    // render resolution
    virtual uint32_t Width() const = 0;
    virtual uint32_t Height() const = 0;
    // physical screen resolution
    virtual uint32_t PhyWidth() const = 0;
    virtual uint32_t PhyHeight() const = 0;
    virtual bool IsSamplingOn() const = 0;
    virtual float GetSamplingTranslateX() const = 0;
    virtual float GetSamplingTranslateY() const = 0;
    virtual float GetSamplingScale() const = 0;

    virtual void DisplayDump(int32_t screenIndex, std::string& dumpString) = 0;
    virtual void SurfaceDump(int32_t screenIndex, std::string& dumpString) = 0;
    virtual void DumpCurrentFrameLayers() = 0;
    virtual void FpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg) = 0;
    virtual void ClearFpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg) = 0;
    virtual void HitchsDump(int32_t screenIndex, std::string& dumpString, std::string& arg) = 0;

    virtual void SetScreenSkipFrameInterval(uint32_t skipFrameInterval) = 0;
    virtual uint32_t GetScreenSkipFrameInterval() const = 0;
    virtual void SetScreenExpectedRefreshRate(uint32_t expectedRefreshRate) = 0;
    virtual uint32_t GetScreenExpectedRefreshRate() const = 0;
    virtual SkipFrameStrategy GetScreenSkipFrameStrategy() const = 0;
    virtual void SetEqualVsyncPeriod(bool isEqualVsyncPeriod) = 0;
    virtual bool GetEqualVsyncPeriod() const = 0;
    virtual void SetScreenVsyncEnabled(bool enabled) const = 0;

    // physical screen
    virtual std::shared_ptr<HdiOutput> GetOutput() const = 0;

    virtual uint32_t SetActiveMode(uint32_t modeId) = 0;
    virtual std::optional<GraphicDisplayModeInfo> GetActiveMode() const = 0;
    virtual int32_t GetActiveModePosByModeId(int32_t modeId) const = 0;
    virtual const std::vector<GraphicDisplayModeInfo>& GetSupportedModes() const = 0;

    virtual const GraphicDisplayCapability& GetCapability() const = 0;
    virtual int32_t GetDisplayIdentificationData(uint8_t& outPort, std::vector<uint8_t>& edidData) const = 0;

    virtual void SetScreenCorrection(ScreenRotation screenRotation) = 0;
    virtual ScreenRotation GetScreenCorrection() const = 0;

    virtual int32_t SetPowerStatus(uint32_t powerStatus) = 0;
    virtual uint32_t GetPowerStatus() = 0;

    virtual void SetScreenBacklight(uint32_t level) = 0;
    virtual int32_t GetScreenBacklight() const = 0;
    virtual int32_t SetScreenConstraint(uint64_t frameId, uint64_t timestamp, ScreenConstraintType type) = 0;

    virtual int32_t SetPixelFormat(GraphicPixelFormat pixelFormat) = 0;
    virtual int32_t GetPixelFormat(GraphicPixelFormat& pixelFormat) const = 0;

    virtual int32_t SetScreenColorGamut(int32_t modeIdx) = 0;
    virtual int32_t GetScreenColorGamut(ScreenColorGamut& mode) const = 0;
    virtual int32_t GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& mode) const = 0;
    virtual int32_t SetScreenGamutMap(ScreenGamutMap mode) = 0;
    virtual int32_t GetScreenGamutMap(ScreenGamutMap& mode) const = 0;

    virtual int32_t SetScreenHDRFormat(int32_t modeIdx) = 0;
    virtual int32_t GetScreenHDRFormat(ScreenHDRFormat& hdrFormat) const = 0;
    virtual int32_t GetScreenSupportedHDRFormats(std::vector<ScreenHDRFormat>& hdrFormats) const = 0;
    virtual const GraphicHDRCapability& GetHDRCapability() = 0;
    virtual int32_t GetScreenSupportedMetaDataKeys(std::vector<ScreenHDRMetadataKey>& keys) const = 0;

    virtual int32_t SetScreenColorSpace(GraphicCM_ColorSpaceType colorSpace) = 0;
    virtual int32_t GetScreenColorSpace(GraphicCM_ColorSpaceType& colorSpace) const = 0;
    virtual int32_t GetScreenSupportedColorSpaces(std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const = 0;

    virtual uint32_t SetScreenActiveRect(const GraphicIRect& activeRect) = 0;
    virtual RectI GetActiveRect() const = 0;
    virtual RectI GetMaskRect() const = 0;
    virtual RectI GetReviseRect() const = 0;
    virtual void SetHasProtectedLayer(bool hasProtectedLayer) = 0;
    virtual bool GetHasProtectedLayer() = 0;
    virtual int32_t SetScreenLinearMatrix(const std::vector<float>& matrix) = 0;
    virtual bool GetDisplayPropertyForHardCursor() = 0;

    // virtual screen
    virtual void SetProducerSurface(sptr<Surface> producerSurface) = 0;
    virtual sptr<Surface> GetProducerSurface() const = 0;
    virtual bool GetAndResetVirtualSurfaceUpdateFlag() = 0;

    virtual void ResizeVirtualScreen(uint32_t width, uint32_t height) = 0;

    virtual bool SetVirtualMirrorScreenCanvasRotation(bool canvasRotation) = 0;
    virtual bool GetCanvasRotation() const = 0;

    virtual bool SetVirtualMirrorScreenScaleMode(ScreenScaleMode scaleMode) = 0;
    virtual ScreenScaleMode GetScaleMode() const = 0;

    virtual bool SetVirtualScreenStatus(VirtualScreenStatus screenStatus) = 0;
    virtual VirtualScreenStatus GetVirtualScreenStatus() const = 0;

    virtual void SetCastScreenEnableSkipWindow(bool enable) = 0;
    virtual bool GetCastScreenEnableSkipWindow() = 0;
    virtual void SetBlackList(const std::unordered_set<uint64_t>& blackList) = 0;
    virtual void SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList) = 0;
    virtual void AddBlackList(const std::vector<uint64_t>& blackList) = 0;
    virtual void RemoveBlackList(const std::vector<uint64_t>& blackList) = 0;
    virtual const std::unordered_set<uint64_t> GetBlackList() const = 0;
    virtual const std::unordered_set<uint8_t> GetTypeBlackList() const = 0;
    virtual const std::unordered_set<uint64_t>& GetWhiteList() const = 0;

    virtual void SetSecurityExemptionList(const std::vector<uint64_t>& securityExemptionList) = 0;
    virtual const std::vector<uint64_t> GetSecurityExemptionList() const = 0;

    virtual int32_t SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask) = 0;
    virtual std::shared_ptr<Media::PixelMap> GetSecurityMask() const = 0;

    virtual void SetEnableVisibleRect(bool enable) = 0;
    virtual bool GetEnableVisibleRect() const = 0;
    virtual void SetMainScreenVisibleRect(const Rect& mainScreenRect) = 0;
    virtual Rect GetMainScreenVisibleRect() const = 0;
    virtual bool GetVisibleRectSupportRotation() const = 0;
    virtual void SetVisibleRectSupportRotation(bool supportRotation) = 0;
    virtual int32_t GetVirtualSecLayerOption() const = 0;
};

namespace impl {
class RSScreen : public OHOS::Rosen::RSScreen {
public:
    RSScreen(ScreenId id, bool isVirtual, std::shared_ptr<HdiOutput> output, sptr<Surface> surface);
    RSScreen(const VirtualScreenConfigs& configs);
    ~RSScreen() override = default;

    RSScreen(const RSScreen&) = delete;
    RSScreen& operator=(const RSScreen&) = delete;

    ScreenId Id() const override;
    const std::string& Name() const override;
    void SetMirror(ScreenId mirroredId) override;
    ScreenId MirroredId() const override;

    bool IsEnable() const override;
    bool IsVirtual() const override;
    const RSScreenType& GetScreenType() const override;

    int32_t SetResolution(uint32_t width, uint32_t height) override;
    void SetRogResolution(uint32_t width, uint32_t height) override;
    // render resolution
    uint32_t Width() const override;
    uint32_t Height() const override;
    // physical screen resolution
    uint32_t PhyWidth() const override;
    uint32_t PhyHeight() const override;
    bool IsSamplingOn() const override;
    float GetSamplingTranslateX() const override;
    float GetSamplingTranslateY() const override;
    float GetSamplingScale() const override;

    void DisplayDump(int32_t screenIndex, std::string& dumpString) override;
    void SurfaceDump(int32_t screenIndex, std::string& dumpString) override;
    void DumpCurrentFrameLayers() override;
    void FpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg) override;
    void ClearFpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg) override;
    void HitchsDump(int32_t screenIndex, std::string& dumpString, std::string& arg) override;

    void SetScreenSkipFrameInterval(uint32_t skipFrameInterval) override;
    uint32_t GetScreenSkipFrameInterval() const override;
    void SetScreenExpectedRefreshRate(uint32_t expectedRefreshRate) override;
    uint32_t GetScreenExpectedRefreshRate() const override;
    SkipFrameStrategy GetScreenSkipFrameStrategy() const override;
    void SetEqualVsyncPeriod(bool isEqualVsyncPeriod) override;
    bool GetEqualVsyncPeriod() const override;
    void SetScreenVsyncEnabled(bool enabled) const override;

    // physical screen
    std::shared_ptr<HdiOutput> GetOutput() const override;

    uint32_t SetActiveMode(uint32_t modeId) override;
    std::optional<GraphicDisplayModeInfo> GetActiveMode() const override;
    int32_t GetActiveModePosByModeId(int32_t modeId) const override;
    const std::vector<GraphicDisplayModeInfo>& GetSupportedModes() const override;

    const GraphicDisplayCapability& GetCapability() const override;
    int32_t GetDisplayIdentificationData(uint8_t& outPort, std::vector<uint8_t>& edidData) const override;

    void SetScreenCorrection(ScreenRotation screenRotation) override;
    ScreenRotation GetScreenCorrection() const override;

    int32_t SetPowerStatus(uint32_t powerStatus) override;
    uint32_t GetPowerStatus() override;

    void SetScreenBacklight(uint32_t level) override;
    int32_t GetScreenBacklight() const override;
    int32_t SetScreenConstraint(uint64_t frameId, uint64_t timestamp, ScreenConstraintType type) override;

    int32_t SetPixelFormat(GraphicPixelFormat pixelFormat) override;
    int32_t GetPixelFormat(GraphicPixelFormat& pixelFormat) const override;

    int32_t SetScreenColorGamut(int32_t modeIdx) override;
    int32_t GetScreenColorGamut(ScreenColorGamut& mode) const override;
    int32_t GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& mode) const override;
    int32_t SetScreenGamutMap(ScreenGamutMap mode) override;
    int32_t GetScreenGamutMap(ScreenGamutMap& mode) const override;

    int32_t SetScreenHDRFormat(int32_t modeIdx) override;
    int32_t GetScreenHDRFormat(ScreenHDRFormat& hdrFormat) const override;
    int32_t GetScreenSupportedHDRFormats(std::vector<ScreenHDRFormat>& hdrFormats) const override;
    const GraphicHDRCapability& GetHDRCapability() override;
    int32_t GetScreenSupportedMetaDataKeys(std::vector<ScreenHDRMetadataKey>& keys) const override;

    int32_t SetScreenColorSpace(GraphicCM_ColorSpaceType colorSpace) override;
    int32_t GetScreenColorSpace(GraphicCM_ColorSpaceType& colorSpace) const override;
    int32_t GetScreenSupportedColorSpaces(std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const override;

    uint32_t SetScreenActiveRect(const GraphicIRect& activeRect) override;
    RectI GetActiveRect() const override;
    RectI GetMaskRect() const override;
    RectI GetReviseRect() const override;
    void SetHasProtectedLayer(bool hasProtectedLayer) override;
    bool GetHasProtectedLayer() override;
    int32_t SetScreenLinearMatrix(const std::vector<float>& matrix) override;
    bool GetDisplayPropertyForHardCursor() override;

    // virtual screen
    void SetProducerSurface(sptr<Surface> producerSurface) override;
    sptr<Surface> GetProducerSurface() const override;
    bool GetAndResetVirtualSurfaceUpdateFlag() override;

    void ResizeVirtualScreen(uint32_t width, uint32_t height) override;

    bool SetVirtualMirrorScreenCanvasRotation(bool canvasRotation) override;
    bool GetCanvasRotation() const override;

    bool SetVirtualMirrorScreenScaleMode(ScreenScaleMode scaleMode) override;
    ScreenScaleMode GetScaleMode() const override;

    bool SetVirtualScreenStatus(VirtualScreenStatus screenStatus) override;
    VirtualScreenStatus GetVirtualScreenStatus() const override;

    void SetCastScreenEnableSkipWindow(bool enable) override;
    bool GetCastScreenEnableSkipWindow() override;
    void SetBlackList(const std::unordered_set<uint64_t>& blackList) override;
    void SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList) override;
    void AddBlackList(const std::vector<uint64_t>& blackList) override;
    void RemoveBlackList(const std::vector<uint64_t>& blackList) override;
    const std::unordered_set<uint64_t> GetBlackList() const override;
    const std::unordered_set<uint8_t> GetTypeBlackList() const override;
    const std::unordered_set<uint64_t>& GetWhiteList() const override;

    void SetSecurityExemptionList(const std::vector<uint64_t>& securityExemptionList) override;
    const std::vector<uint64_t> GetSecurityExemptionList() const override;

    int32_t SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask) override;
    std::shared_ptr<Media::PixelMap> GetSecurityMask() const override;

    void SetEnableVisibleRect(bool enable) override;
    bool GetEnableVisibleRect() const override;
    void SetMainScreenVisibleRect(const Rect& mainScreenRect) override;
    Rect GetMainScreenVisibleRect() const override;
    bool GetVisibleRectSupportRotation() const override;
    void SetVisibleRectSupportRotation(bool supportRotation) override;
    int32_t GetVirtualSecLayerOption() const override;

private:
    // create hdiScreen and get some information from drivers.
    void PhysicalScreenInit() noexcept;
    void ScreenCapabilityInit() noexcept;
    void VirtualScreenInit() noexcept;
    void WriteHisyseventEpsLcdInfo(GraphicDisplayModeInfo& activeMode);
    bool CalculateMaskRectAndReviseRect(const GraphicIRect& activeRect, GraphicIRect& reviseRect);

    void ModeInfoDump(std::string& dumpString);
    void CapabilityDump(std::string& dumpString);
    void PropDump(std::string& dumpString);
    void PowerStatusDump(std::string& dumpString);
    void CapabilityTypeDump(GraphicInterfaceType capabilityType, std::string& dumpString);
    void ScreenTypeDump(std::string& dumpString);

    // ScreenId for this screen.
    ScreenId id_ = INVALID_SCREEN_ID;
    // If this screen is the mirror of other screen, this member would be a valid id.
    ScreenId mirroredId_ = INVALID_SCREEN_ID;

    std::string name_;

    mutable std::shared_mutex screenMutex_;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t phyWidth_ = 0;
    uint32_t phyHeight_ = 0;
    bool isSamplingOn_ = false;
    float samplingTranslateX_ = 0.f;
    float samplingTranslateY_ = 0.f;
    float samplingScale_ = 1.f;
    int32_t screenBacklightLevel_ = INVALID_BACKLIGHT_VALUE;
    RectI activeRect_ = {};
    RectI maskRect_ = {};
    RectI reviseRect_ = {};

    std::atomic<VirtualScreenStatus> screenStatus_ = VIRTUAL_SCREEN_PLAY;

    bool isVirtual_ = true;
    std::atomic<bool> isVirtualSurfaceUpdateFlag_ = false;
    int32_t virtualSecLayerOption_ = 0;
    std::shared_ptr<HdiOutput> hdiOutput_ = nullptr; // has value if the screen is physical
    std::unique_ptr<HdiScreen> hdiScreen_ = nullptr; // has value if the screen is physical
    std::vector<GraphicDisplayModeInfo> supportedModes_;
    GraphicDisplayCapability capability_ = {"test1", GRAPHIC_DISP_INTF_HDMI, 1921, 1081, 0, 0, true, 0};
    GraphicHDRCapability hdrCapability_;

    mutable std::mutex producerSurfaceMutex_;
    sptr<Surface> producerSurface_ = nullptr;  // has value if the screen is virtual
    ScreenPowerStatus powerStatus_ = ScreenPowerStatus::INVALID_POWER_STATUS;
    std::atomic<GraphicPixelFormat> pixelFormat_;

    std::vector<ScreenColorGamut> supportedVirtualColorGamuts_ = {
        COLOR_GAMUT_SRGB,
        COLOR_GAMUT_DCI_P3,
        COLOR_GAMUT_ADOBE_RGB,
        COLOR_GAMUT_DISPLAY_P3 };
    std::vector<ScreenColorGamut> supportedPhysicalColorGamuts_;
    std::atomic<int32_t> currentVirtualColorGamutIdx_ = 0;
    std::atomic<int32_t> currentPhysicalColorGamutIdx_ = 0;
    std::atomic<ScreenGamutMap> currentVirtualGamutMap_ = GAMUT_MAP_CONSTANT;
    std::atomic<int32_t> currentVirtualHDRFormatIdx_ = 0;
    std::atomic<int32_t> currentPhysicalHDRFormatIdx_ = 0;
    std::vector<ScreenHDRFormat> supportedVirtualHDRFormats_ = {
        NOT_SUPPORT_HDR };
    std::vector<ScreenHDRFormat> supportedPhysicalHDRFormats_;
    RSScreenType screenType_ = RSScreenType::UNKNOWN_TYPE_SCREEN;

    mutable std::shared_mutex skipFrameMutex_;
    uint32_t skipFrameInterval_ = DEFAULT_SKIP_FRAME_INTERVAL;
    uint32_t expectedRefreshRate_ = INVALID_EXPECTED_REFRESH_RATE;
    SkipFrameStrategy skipFrameStrategy_ = SKIP_FRAME_BY_INTERVAL;
    bool isEqualVsyncPeriod_ = true;

    std::atomic<ScreenRotation> screenRotation_ = ScreenRotation::ROTATION_0;
    std::atomic<bool> canvasRotation_ = false; // just for virtual screen to use
    std::atomic<ScreenScaleMode> scaleMode_ = ScreenScaleMode::UNISCALE_MODE; // just for virtual screen to use
    static std::map<GraphicColorGamut, GraphicCM_ColorSpaceType> RS_TO_COMMON_COLOR_SPACE_TYPE_MAP;
    static std::map<GraphicCM_ColorSpaceType, GraphicColorGamut> COMMON_COLOR_SPACE_TYPE_TO_RS_MAP;
    static std::map<GraphicHDRFormat, ScreenHDRFormat> HDI_HDR_FORMAT_TO_RS_MAP;
    static std::map<ScreenHDRFormat, GraphicHDRFormat> RS_TO_HDI_HDR_FORMAT_MAP;
    std::unordered_set<uint64_t> whiteList_ = {};

    mutable std::mutex blackListMutex_;
    std::unordered_set<uint64_t> blackList_ = {};

    mutable std::mutex typeBlackListMutex_;
    std::unordered_set<uint8_t> typeBlackList_ = {};

    mutable std::mutex securityExemptionMutex_;
    std::vector<uint64_t> securityExemptionList_ = {};

    mutable std::mutex securityMaskMutex_;
    std::shared_ptr<Media::PixelMap> securityMask_ = nullptr;

    mutable std::mutex visibleRectMutex_;
    Rect mainScreenVisibleRect_ = {};

    std::atomic<bool> enableVisibleRect_ = false;
    std::atomic<bool> skipWindow_ = false;

    std::once_flag hardCursorSupportedFlag_;
    bool isHardCursorSupport_ = false;

    std::atomic<bool> isSupportRotation_ = false;
    std::atomic<bool> hasProtectedLayer_ = false;

    std::vector<float> linearMatrix_ = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    std::atomic<bool> hasLogBackLightAfterPowerStatusChanged_ = false;
};
} // namespace impl
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN
