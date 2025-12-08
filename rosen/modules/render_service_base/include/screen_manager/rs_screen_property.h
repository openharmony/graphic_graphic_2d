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

#ifndef RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SCREEN_PROPERTY_H
#define RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SCREEN_PROPERTY_H

#include <unordered_set>

#include "common/rs_rect.h"
#include "rs_screen_info.h"
#include "property/rs_properties_def.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface.h"
#endif

namespace OHOS::Rosen {
class RSB_EXPORT RSScreenProperty : public Parcelable {
public:
    RSScreenProperty();
    ~RSScreenProperty();

    bool Marshalling(Parcel& data) const override;
    static RSScreenProperty* Unmarshalling(Parcel& data);

    bool UnmarshallingData(Parcel& data);

    ScreenId GetScreenId() const;
    bool IsVirtual() const;
    const std::string& Name() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetPhyWidth() const;
    uint32_t GetPhyHeight() const;
    uint32_t GetRefreshRate() const;
    int32_t GetOffsetX() const;
    int32_t GetOffsetY() const;
    bool GetIsSamplingOn() const;
    float GetSamplingTranslateX() const;
    float GetSamplingTranslateY() const;
    float GetSamplingScale() const;
    ScreenColorGamut GetScreenColorGamut() const;
    ScreenGamutMap GetScreenGamutMap() const;
    ScreenState GetState() const;
    ScreenRotation GetScreenCorrection() const;
    bool GetCanvasRotation() const;
    bool GetAutoBufferRotation() const;
    RectI GetActiveRect() const;
    RectI GetMaskRect() const;
    RectI GetReviseRect() const;
    uint32_t GetScreenSkipFrameInterval() const;
    uint32_t GetScreenExpectedRefreshRate() const;
    SkipFrameStrategy GetScreenSkipFrameStrategy() const;
    GraphicPixelFormat GetPixelFormat() const;
    ScreenHDRFormat GetScreenHDRFormat() const;
    bool GetEnableVisibleRect() const;
    Rect GetVisibleRect() const;
    bool IsVisibleRectSupportRotation() const;
    const std::unordered_set<NodeId>& GetWhiteList() const;
    const std::unordered_set<NodeId>& GetBlackList() const;
    const std::unordered_set<NodeType>& GetTypeBlackList() const;
    const std::vector<NodeId>& GetSecurityExemptionList() const;
    std::shared_ptr<Media::PixelMap> GetSecurityMask() const;
    bool EnableSkipWindow() const;
    ScreenPowerStatus GetScreenPowerStatus() const;
    RSScreenType GetScreenType() const;
    ScreenConnectionType GetConnectionType() const;
#ifndef ROSEN_CROSS_PLATFORM
    sptr<Surface> GetProducerSurface() const;
#endif
    ScreenScaleMode GetScaleMode() const;
    VirtualScreenStatus GetVirtualScreenStatus() const;
    int32_t GetVirtualSecLayerOption() const;
    bool IsHardCursorSupport() const;
    const std::vector<ScreenColorGamut>& GetScreenSupportedColorGamuts() const;
    bool GetDisablePowerOffRenderControl() const;
    ScreenPowerStatus GetPowerStatus() const;
    bool IsScreenSwitching() const;

    ScreenInfo GetScreenInfo() const;

private:
    ScreenId id_ = INVALID_SCREEN_ID;
    bool isVirtual_ = false;
    std::string name_;
    uint32_t width_ = 0; // render resolution
    uint32_t height_ = 0;
    uint32_t phyWidth_ = 0; // physical screen resolution
    uint32_t phyHeight_ = 0;
    uint32_t refreshRate_ = 0;
    int32_t offsetX_ = 0;
    int32_t offsetY_ = 0;
    bool isSamplingOn_ = false;
    float samplingTranslateX_ = 0.f;
    float samplingTranslateY_ = 0.f;
    float samplingScale_ = 1.f;
    ScreenColorGamut colorGamut_ = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ScreenGamutMap gamutMap_ = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ScreenState state_ = ScreenState::UNKNOWN;
    ScreenRotation correction_ = ScreenRotation::ROTATION_0;
    bool canvasRotation_ = false;
    bool autoBufferRotation_ = false;
    RectI activeRect_;
    RectI maskRect_;
    RectI reviseRect_;
    uint32_t skipFrameInterval_ = DEFAULT_SKIP_FRAME_INTERVAL; // skip frame interval for change screen refresh rate
    uint32_t expectedRefreshRate_ = INVALID_EXPECTED_REFRESH_RATE;
    SkipFrameStrategy skipFrameStrategy_ = SKIP_FRAME_BY_INTERVAL;
    GraphicPixelFormat pixelFormat_ = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    ScreenHDRFormat hdrFormat_ = ScreenHDRFormat::NOT_SUPPORT_HDR;
    bool enableVisibleRect_ = false;
    Rect mainScreenVisibleRect_ = {};
    bool isSupportRotation_ = false;
    std::unordered_set<NodeId> whiteList_ = {};
    std::unordered_set<NodeId> blackList_ = {};
    std::unordered_set<NodeType> typeBlackList_ = {};
    std::vector<NodeId> securityExemptionList_;
    std::shared_ptr<Media::PixelMap> securityMask_;
    bool skipWindow_ = false;
    ScreenPowerStatus powerStatus_ = ScreenPowerStatus::INVALID_POWER_STATUS;
    RSScreenType screenType_ = RSScreenType::UNKNOWN_TYPE_SCREEN;
    ScreenConnectionType connectionType_ = ScreenConnectionType::INVALID_DISPLAY_CONNECTION_TYPE;

#ifndef ROSEN_CROSS_PLATFORM
    sptr<Surface> producerSurface_ = nullptr; // has value if the screen is virtual
#endif

    ScreenScaleMode scaleMode_ = ScreenScaleMode::UNISCALE_MODE; // just for virtual screen to use
    VirtualScreenStatus screenStatus_ = VIRTUAL_SCREEN_PLAY;
    int32_t virtualSecLayerOption_ = 0;
    bool isHardCursorSupport_ = false;
    bool disablePowerOffRenderControl_ = false;
    std::vector<ScreenColorGamut> supportedColorGamuts_;
    bool screenSwitchStatus_ = false;

    friend class RSScreenManager;
    friend class RSScreenThreadSafeProperty;
};
}  // namespace OHOS::Rosen
#endif  // RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SCREEN_PROPERTY_H