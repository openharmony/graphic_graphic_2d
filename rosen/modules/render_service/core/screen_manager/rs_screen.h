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
#include <optional>
#include <unordered_set>

#include <hdi_display_type.h>
#include <hdi_output.h>
#include <hdi_screen.h>
#include <surface_type.h>

#include <common/rs_rect.h>
#include <screen_manager/screen_types.h>
#include "rs_screen_thread_safe_property.h"

namespace OHOS {
namespace Rosen {
struct VirtualScreenConfigs {
    ScreenId id = INVALID_SCREEN_ID;
    ScreenId associatedScreenId = INVALID_SCREEN_ID;
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
    explicit RSScreen(std::shared_ptr<HdiOutput> output);
    explicit RSScreen(const VirtualScreenConfigs& configs);
    ~RSScreen() = default;

    RSScreen(const RSScreen&) = delete;
    RSScreen& operator=(const RSScreen&) = delete;

    ScreenId Id() const;
    const std::string& Name() const;
    ScreenId GetAssociatedScreenId() const;

    bool IsVirtual() const;
    RSScreenType GetScreenType() const;
    ScreenConnectionType GetConnectionType() const;

    int32_t SetResolution(uint32_t width, uint32_t height);
    void SetRogResolution(uint32_t width, uint32_t height);
    int32_t GetRogResolution(uint32_t& width, uint32_t& height);
    // render resolution
    uint32_t Width() const;
    uint32_t Height() const;
    // physical screen resolution
    uint32_t PhyWidth() const;
    uint32_t PhyHeight() const;
    bool IsSamplingOn() const;
    float GetSamplingTranslateX() const;
    float GetSamplingTranslateY() const;
    float GetSamplingScale() const;

    void DisplayDump(int32_t screenIndex, std::string& dumpString);
    void SurfaceDump(int32_t screenIndex, std::string& dumpString);
    void DumpCurrentFrameLayers();
    void FpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg);
    void ClearFpsDump(int32_t screenIndex, std::string& dumpString, std::string& arg);
    void HitchsDump(int32_t screenIndex, std::string& dumpString, std::string& arg);

    void SetScreenSkipFrameInterval(uint32_t skipFrameInterval);
    uint32_t GetScreenSkipFrameInterval() const;
    void SetScreenExpectedRefreshRate(uint32_t expectedRefreshRate);
    uint32_t GetScreenExpectedRefreshRate() const;
    SkipFrameStrategy GetScreenSkipFrameStrategy() const;
    void SetScreenVsyncEnabled(bool enabled) const;

    // physical screen
    std::shared_ptr<HdiOutput> GetOutput() const;

    uint32_t SetActiveMode(uint32_t modeId);
    std::optional<GraphicDisplayModeInfo> GetActiveMode() const;
    int32_t GetActiveModePosByModeId(int32_t modeId) const;
    const std::vector<GraphicDisplayModeInfo>& GetSupportedModes() const;
    uint32_t GetActiveRefreshRate() const;

    const GraphicDisplayCapability& GetCapability() const;
    int32_t GetDisplayIdentificationData(uint8_t& outPort, std::vector<uint8_t>& edidData) const;

    void SetScreenCorrection(ScreenRotation screenRotation);
    ScreenRotation GetScreenCorrection() const;

    int32_t SetPowerStatus(uint32_t powerStatus);
    ScreenPowerStatus GetPowerStatus();

    int32_t SetDualScreenState(DualScreenStatus status);

    PanelPowerStatus GetPanelPowerStatus() const;
    void SetScreenBacklight(uint32_t level);
    int32_t GetScreenBacklight() const;
    int32_t SetScreenConstraint(uint64_t frameId, uint64_t timestamp, ScreenConstraintType type);

    int32_t SetPixelFormat(GraphicPixelFormat pixelFormat);
    int32_t GetPixelFormat(GraphicPixelFormat& pixelFormat) const;

    int32_t SetScreenColorGamut(int32_t modeIdx);
    int32_t GetScreenColorGamut(ScreenColorGamut& mode) const;
    int32_t GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& mode) const;
    int32_t SetScreenGamutMap(ScreenGamutMap mode);
    int32_t GetScreenGamutMap(ScreenGamutMap& mode) const;

    int32_t SetScreenHDRFormat(int32_t modeIdx);
    int32_t GetScreenHDRFormat(ScreenHDRFormat& hdrFormat) const;
    int32_t GetScreenSupportedHDRFormats(std::vector<ScreenHDRFormat>& hdrFormats) const;
    const GraphicHDRCapability& GetHDRCapability();
    int32_t GetScreenSupportedMetaDataKeys(std::vector<ScreenHDRMetadataKey>& keys) const;

    int32_t SetScreenColorSpace(GraphicCM_ColorSpaceType colorSpace);
    int32_t GetScreenColorSpace(GraphicCM_ColorSpaceType& colorSpace) const;
    int32_t GetScreenSupportedColorSpaces(std::vector<GraphicCM_ColorSpaceType>& colorSpaces) const;

    uint32_t SetScreenActiveRect(const GraphicIRect& activeRect);
    RectI GetActiveRect() const;
    RectI GetMaskRect() const;
    RectI GetReviseRect() const;
    void SetHasProtectedLayer(bool hasProtectedLayer);
    bool GetHasProtectedLayer();
    int32_t SetScreenLinearMatrix(const std::vector<float>& matrix);

    // virtual screen
    void SetProducerSurface(sptr<Surface> producerSurface);
    sptr<Surface> GetProducerSurface() const;

    void ResizeVirtualScreen(uint32_t width, uint32_t height);

    bool SetVirtualMirrorScreenCanvasRotation(bool canvasRotation);
    bool GetCanvasRotation() const;

    int32_t SetVirtualScreenAutoRotation(bool isAutoRotation);
    bool GetVirtualScreenAutoRotation() const;

    bool SetVirtualMirrorScreenScaleMode(ScreenScaleMode scaleMode);
    ScreenScaleMode GetScaleMode() const;

    bool SetVirtualScreenStatus(VirtualScreenStatus screenStatus);
    VirtualScreenStatus GetVirtualScreenStatus() const;

    void SetCastScreenEnableSkipWindow(bool enable);
    bool GetCastScreenEnableSkipWindow();
    void SetBlackList(const std::unordered_set<uint64_t>& blackList);
    void SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList);
    void AddBlackList(const std::vector<uint64_t>& blackList);
    void RemoveBlackList(const std::vector<uint64_t>& blackList);
    std::unordered_set<uint64_t> GetBlackList() const;
    std::unordered_set<uint8_t> GetTypeBlackList() const;
    std::unordered_set<uint64_t> GetWhiteList() const;

    void SetSecurityExemptionList(const std::vector<uint64_t>& securityExemptionList);
    const std::vector<uint64_t> GetSecurityExemptionList() const;

    int32_t SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask);
    std::shared_ptr<Media::PixelMap> GetSecurityMask() const;

    void SetEnableVisibleRect(bool enable);
    bool GetEnableVisibleRect() const;
    void SetMainScreenVisibleRect(const Rect& mainScreenRect);
    Rect GetMainScreenVisibleRect() const;
    bool GetVisibleRectSupportRotation() const;
    void SetVisibleRectSupportRotation(bool supportRotation);

    void SetScreenOffset(int32_t offsetX, int32_t offsetY);
    int32_t GetOffsetX() const;
    int32_t GetOffsetY() const;

    bool GetAndResetPSurfaceChange();
    void SetPSurfaceChange(bool pSurfaceChange);
    bool GetAndResetVirtualScreenPlay();

    void SetOnPropertyChangedCallback(std::function<void(const sptr<RSScreenProperty>&)> callback);
    sptr<RSScreenProperty> GetProperty() const;
    ScreenInfo GetScreenInfo() const;
private:
    // create hdiScreen and get some information from drivers.
    void PhysicalScreenInit() noexcept;
    void ScreenCapabilityInit() noexcept;
    void VirtualScreenInit() noexcept;
    void InitDisplayPropertyForHardCursor();
    void WriteHisyseventEpsLcdInfo(GraphicDisplayModeInfo& activeMode);
    bool CalculateMaskRectAndReviseRect(const GraphicIRect& activeRect, GraphicIRect& reviseRect);

    void ModeInfoDump(std::string& dumpString);
    void CapabilityDump(std::string& dumpString);
    void PropDump(std::string& dumpString);
    void PowerStatusDump(std::string& dumpString);
    void CapabilityTypeDump(GraphicInterfaceType capabilityType, std::string& dumpString);
    void ScreenTypeDump(std::string& dumpString);

    ScreenId associatedScreenId_ = INVALID_SCREEN_ID;
    std::atomic<bool> isRogResolution_ = false;

    std::shared_ptr<HdiOutput> hdiOutput_ = nullptr; // has value if the screen is physical
    std::unique_ptr<HdiScreen> hdiScreen_ = nullptr; // has value if the screen is physical
    std::vector<GraphicDisplayModeInfo> supportedModes_;
    GraphicDisplayCapability capability_ = {"test1", GRAPHIC_DISP_INTF_HDMI, 1921, 1081, 0, 0, true, 0};
    GraphicHDRCapability hdrCapability_;

    std::vector<ScreenColorGamut> supportedVirtualColorGamuts_ = {
        COLOR_GAMUT_SRGB,
        COLOR_GAMUT_DCI_P3,
        COLOR_GAMUT_ADOBE_RGB,
        COLOR_GAMUT_DISPLAY_P3,
        COLOR_GAMUT_BT2100_HLG };
    std::vector<ScreenColorGamut> supportedPhysicalColorGamuts_;
    std::atomic<int32_t> currentVirtualColorGamutIdx_ = 0;
    std::atomic<int32_t> currentPhysicalColorGamutIdx_ = 0;
    std::atomic<int32_t> currentVirtualHDRFormatIdx_ = 0;
    std::atomic<int32_t> currentPhysicalHDRFormatIdx_ = 0;
    std::vector<ScreenHDRFormat> supportedVirtualHDRFormats_ = {
        NOT_SUPPORT_HDR };
    std::vector<ScreenHDRFormat> supportedPhysicalHDRFormats_;

    static std::map<GraphicColorGamut, GraphicCM_ColorSpaceType> RS_TO_COMMON_COLOR_SPACE_TYPE_MAP;
    static std::map<GraphicCM_ColorSpaceType, GraphicColorGamut> COMMON_COLOR_SPACE_TYPE_TO_RS_MAP;
    static std::map<GraphicHDRFormat, ScreenHDRFormat> HDI_HDR_FORMAT_TO_RS_MAP;
    static std::map<ScreenHDRFormat, GraphicHDRFormat> RS_TO_HDI_HDR_FORMAT_MAP;

    std::atomic<int32_t> backlightLevel_ = INVALID_BACKLIGHT_VALUE;
    std::atomic<bool> hasProtectedLayer_ = false;

    std::vector<float> linearMatrix_ = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    std::atomic<bool> hasLogBackLightAfterPowerStatusChanged_ = false;

    // status for full screen dirty region update
    std::atomic<bool> pSurfaceChange_ = false;
    std::atomic<bool> virtualScreenPlay_ = false;

    std::function<void(const sptr<RSScreenProperty>&)> onPropertyChange_;
    RSScreenThreadSafeProperty property_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN
