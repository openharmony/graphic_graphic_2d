/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef RS_SCREEN_THREAD_SAFE_PROPERTY_H
#define RS_SCREEN_THREAD_SAFE_PROPERTY_H

#include <shared_mutex>
#include <screen_manager/rs_screen_property.h>

namespace OHOS {
namespace Rosen {
class RSScreenThreadSafeProperty {
public:
    RSScreenThreadSafeProperty();
    ~RSScreenThreadSafeProperty();

    sptr<RSScreenProperty> Clone() const;
    void SetId(ScreenId id);
    void SetName(const std::string& name);
    void SetIsVirtual(bool isVirtual);
    void SetWidth(uint32_t width);
    void SetHeight(uint32_t height);
    void SetPhyWidth(uint32_t phyWidth);
    void SetPhyHeight(uint32_t phyHeight);
    void SetRefreshRate(uint32_t refreshRate);
    void SetOffsetX(int32_t offsetX);
    void SetOffsetY(int32_t offsetY);
    void SetIsSamplingOn(bool isSamplingOn);
    void SetSamplingTranslateX(float samplingTranslateX);
    void SetSamplingTranslateY(float samplingTranslateY);
    void SetSamplingScale(float samplingScale);
    void SetScreenColorGamut(ScreenColorGamut colorGamut);
    void SetScreenGamutMap(ScreenGamutMap gamutMap);
    void SetState(ScreenState state);
    void SetScreenCorrection(ScreenRotation screenRotation);
    void SetCanvasRotation(bool canvasRotation);
    void SetAutoBufferRotation(bool isAutoRotation);
    void SetActiveRect(RectI activeRect);
    void SetMaskRect(RectI maskRect);
    void SetReviseRect(RectI reviseRect);
    void SetSkipFrameInterval(uint32_t skipFrameInterval);
    void SetExpectedRefreshRate(uint32_t expectedRefreshRate);
    void SetSkipFrameStrategy(SkipFrameStrategy skipFrameStrategy);
    void SetPixelFormat(GraphicPixelFormat pixelFormat);
    void SetScreenHDRFormat(ScreenHDRFormat hdrFormat);
    void SetEnableVisibleRect(bool enableVisibleRect);
    void SetMainScreenVisibleRect(const Rect& mainScreenRect);
    void SetVisibleRectSupportRotation(bool supportRotation);
    void SetWhiteList(const std::unordered_set<uint64_t>& whiteList);
    void SetBlackList(const std::unordered_set<uint64_t>& blackList);
    void AddBlackList(const std::vector<uint64_t>& blackList);
    void RemoveBlackList(const std::vector<uint64_t>& blackList);
    void SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList);
    void SetSecurityExemptionList(const std::vector<uint64_t>& securityExemptionList);
    void SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask);
    void SetCastScreenEnableSkipWindow(bool enable);
    void SetPowerStatus(ScreenPowerStatus powerStatus);
    void SetScreenType(RSScreenType screenType);
    void SetProducerSurface(sptr<Surface> producerSurface);
    void SetScreenScaleMode(ScreenScaleMode scaleMode);
    void SetScreenStatus(VirtualScreenStatus screenStatus);
    void SetVirtualSecLayerOption(int32_t virtualSecLayerOption);
    void SetIsHardCursorSupport(bool isHardCursorSupport);
    void SetSupportedColorGamuts(std::vector<ScreenColorGamut> colorGamuts);
    void SetDisablePowerOffRenderControl(bool disable);
    void SetScreenSwitchStatus(bool status);

    ScreenId GetId() const;
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
    uint32_t GetSkipFrameInterval() const;
    uint32_t GetExpectedRefreshRate() const;
    SkipFrameStrategy GetSkipFrameStrategy() const;
    GraphicPixelFormat GetPixelFormat() const;
    ScreenHDRFormat GetScreenHDRFormat() const;
    bool GetEnableVisibleRect() const;
    Rect GetMainScreenVisibleRect() const;
    bool GetVisibleRectSupportRotation() const;
    std::unordered_set<uint64_t> GetWhiteList() const;
    std::unordered_set<uint64_t> GetBlackList() const;
    std::unordered_set<uint8_t> GetTypeBlackList() const;
    std::vector<uint64_t> GetSecurityExemptionList() const;
    std::shared_ptr<Media::PixelMap> GetSecurityMask() const;
    bool GetCastScreenEnableSkipWindow() const;
    ScreenPowerStatus GetPowerStatus() const;
    RSScreenType GetScreenType() const;
    sptr<Surface> GetProducerSurface() const;
    ScreenScaleMode GetScreenScaleMode() const;
    VirtualScreenStatus GetScreenStatus() const;
    int32_t GetVirtualSecLayerOption() const;
    bool GetIsHardCursorSupport() const;
    std::vector<ScreenColorGamut> GetSupportedColorGamuts() const;

    ScreenInfo GetScreenInfo() const;

private:
    using SharedLock = std::shared_lock<std::shared_mutex>;
    using UniqueLock = std::unique_lock<std::shared_mutex>;

    mutable std::shared_mutex propertyMutex_;
    sptr<RSScreenProperty> property_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_SCREEN_THREAD_SAFE_PROPERTY_H
