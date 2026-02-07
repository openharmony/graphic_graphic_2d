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
#include "rs_screen_thread_safe_property.h"

#include "platform/common/rs_log.h"
#include <pixel_map.h>

namespace OHOS {
namespace Rosen {
RSScreenThreadSafeProperty::RSScreenThreadSafeProperty() : property_(sptr<RSScreenProperty>::MakeSptr()) {}

RSScreenThreadSafeProperty::~RSScreenThreadSafeProperty() {}

sptr<RSScreenProperty> RSScreenThreadSafeProperty::Clone() const
{
    auto cloned = sptr<RSScreenProperty>::MakeSptr();
    UniqueLock lock(propertyMutex_);
    cloned->screenProperties_ = property_->screenProperties_;
    return cloned;
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetId(ScreenId id)
{
    auto prop = property_->Set<ScreenPropertyType::ID>(id);
    return { ScreenPropertyType::ID, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetIsVirtual(bool isVirtual)
{
    auto prop = property_->Set<ScreenPropertyType::IS_VIRTUAL>(isVirtual);
    return { ScreenPropertyType::IS_VIRTUAL, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetName(const std::string& name)
{
    auto prop = property_->Set<ScreenPropertyType::NAME>(name);
    return { ScreenPropertyType::NAME, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetResolution(std::pair<uint32_t, uint32_t> resolution)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::RENDER_RESOLUTION>(resolution);
    return { ScreenPropertyType::RENDER_RESOLUTION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetPhysicalModeParams(
    uint32_t phyWidth, uint32_t phyHeight, uint32_t refreshRate)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>(
        std::make_tuple(phyWidth, phyHeight, refreshRate));
    return { ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetOffset(int32_t offsetX, int32_t offsetY)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::OFFSET>(std::make_pair(offsetX, offsetY));
    return { ScreenPropertyType::OFFSET, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetSamplingOption(
    bool isSamplingOn, float translateX, float translateY, float samplingScale)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SAMPLING_OPTION>(
        std::make_tuple(isSamplingOn, translateX, translateY, samplingScale));
    return { ScreenPropertyType::SAMPLING_OPTION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenColorGamut(ScreenColorGamut colorGamut)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::COLOR_GAMUT>(static_cast<uint32_t>(colorGamut));
    return { ScreenPropertyType::COLOR_GAMUT, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenGamutMap(ScreenGamutMap gamutMap)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::GAMUT_MAP>(static_cast<uint32_t>(gamutMap));
    return { ScreenPropertyType::GAMUT_MAP, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetState(ScreenState state)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::STATE>(static_cast<uint8_t>(state));
    return { ScreenPropertyType::STATE, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenCorrection(ScreenRotation correction)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::CORRECTION>(static_cast<uint32_t>(correction));
    return { ScreenPropertyType::CORRECTION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetCanvasRotation(bool canvasRotation)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::CANVAS_ROTATION>(canvasRotation);
    return { ScreenPropertyType::CANVAS_ROTATION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetAutoBufferRotation(bool isAutoRotation)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::AUTO_BUFFER_ROTATION>(isAutoRotation);
    return { ScreenPropertyType::AUTO_BUFFER_ROTATION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetActiveRectOption(
    RectI activeRect, RectI maskRect, RectI reviseRect)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::ACTIVE_RECT_OPTION>(
        std::make_tuple(activeRect, maskRect, reviseRect));
    return { ScreenPropertyType::ACTIVE_RECT_OPTION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetSkipFrameOption(
    uint32_t skipFrameInterval, uint32_t expectedRefreshRate, SkipFrameStrategy skipFrameStrategy)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SKIP_FRAME_OPTION>(
        std::make_tuple(skipFrameInterval, expectedRefreshRate, static_cast<uint8_t>(skipFrameStrategy)));
    return { ScreenPropertyType::SKIP_FRAME_OPTION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetPixelFormat(GraphicPixelFormat pixelFormat)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::PIXEL_FORMAT>(static_cast<int32_t>(pixelFormat));
    return { ScreenPropertyType::PIXEL_FORMAT, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenHDRFormat(ScreenHDRFormat hdrFormat)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::HDR_FORMAT>(static_cast<uint32_t>(hdrFormat));
    return { ScreenPropertyType::HDR_FORMAT, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetVisibleRectOption(
    bool enableVisibleRect, const Rect& mainScreenRect, bool supportRotation)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::VISIBLE_RECT_OPTION>(
        std::make_tuple(enableVisibleRect, mainScreenRect, supportRotation));
    return { ScreenPropertyType::VISIBLE_RECT_OPTION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetWhiteList(
    const std::unordered_set<uint64_t>& whiteList)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::WHITE_LIST>(whiteList);
    return { ScreenPropertyType::WHITE_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::AddWhiteList(
    const std::vector<uint64_t>& whiteList)
{
    UniqueLock lock(propertyMutex_);
    auto tmpList = property_->Get<ScreenPropertyType::WHITE_LIST>();
    tmpList.insert(whiteList.cbegin(), whiteList.cend());

    auto prop = property_->Set<ScreenPropertyType::WHITE_LIST>(tmpList);
    return { ScreenPropertyType::WHITE_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::RemoveWhiteList(
    const std::vector<uint64_t>& whiteList)
{
    UniqueLock lock(propertyMutex_);
    auto tmpList = property_->Get<ScreenPropertyType::WHITE_LIST>();
    for (auto id : whiteList) {
        tmpList.erase(id);
    }

    auto prop = property_->Set<ScreenPropertyType::WHITE_LIST>(tmpList);
    return { ScreenPropertyType::WHITE_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetBlackList(
    const std::unordered_set<uint64_t>& blackList)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::BLACK_LIST>(blackList);
    return { ScreenPropertyType::BLACK_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::AddBlackList(
    const std::vector<uint64_t>& blackList)
{
    UniqueLock lock(propertyMutex_);
    auto tmpList = property_->Get<ScreenPropertyType::BLACK_LIST>();
    tmpList.insert(blackList.cbegin(), blackList.cend());

    auto prop = property_->Set<ScreenPropertyType::BLACK_LIST>(tmpList);
    return { ScreenPropertyType::BLACK_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::RemoveBlackList(
    const std::vector<uint64_t>& blackList)
{
    UniqueLock lock(propertyMutex_);
    auto tmpList = property_->Get<ScreenPropertyType::BLACK_LIST>();
    for (auto id : blackList) {
        tmpList.erase(id);
    }
    auto prop = property_->Set<ScreenPropertyType::BLACK_LIST>(tmpList);
    return { ScreenPropertyType::BLACK_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetTypeBlackList(
    const std::unordered_set<uint8_t>& typeBlackList)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::TYPE_BLACK_LIST>(typeBlackList);
    return { ScreenPropertyType::TYPE_BLACK_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetSecurityExemptionList(
    const std::vector<uint64_t>& securityExemptionList)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SECURITY_EXEMPTION_LIST>(securityExemptionList);
    return { ScreenPropertyType::SECURITY_EXEMPTION_LIST, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetSecurityMask(
    std::shared_ptr<Media::PixelMap> securityMask)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SECURITY_MASK>(securityMask);
    return { ScreenPropertyType::SECURITY_MASK, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetCastScreenEnableSkipWindow(bool enable)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(enable);
    return { ScreenPropertyType::ENABLE_SKIP_WINDOW, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetPowerStatus(ScreenPowerStatus powerStatus)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::POWER_STATUS>(static_cast<uint32_t>(powerStatus));
    return { ScreenPropertyType::POWER_STATUS, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenType(RSScreenType screenType)
{
    auto prop = property_->Set<ScreenPropertyType::SCREEN_TYPE>(static_cast<uint32_t>(screenType));
    return { ScreenPropertyType::SCREEN_TYPE, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetConnectionType(ScreenConnectionType connectionType)
{
    auto prop = property_->Set<ScreenPropertyType::CONNECTION_TYPE>(static_cast<uint32_t>(connectionType));
    return { ScreenPropertyType::CONNECTION_TYPE, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetProducerSurface(sptr<Surface> producerSurface)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::PRODUCER_SURFACE>(producerSurface);
    return { ScreenPropertyType::PRODUCER_SURFACE, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenScaleMode(ScreenScaleMode scaleMode)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SCALE_MODE>(static_cast<uint32_t>(scaleMode));
    return { ScreenPropertyType::SCALE_MODE, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenStatus(VirtualScreenStatus screenStatus)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(screenStatus));
    return { ScreenPropertyType::SCREEN_STATUS, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetVirtualSecLayerOption(int32_t virtualSecLayerOption)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION>(virtualSecLayerOption);
    return { ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetIsHardCursorSupport(bool isHardCursorSupport)
{
    auto prop = property_->Set<ScreenPropertyType::IS_HARD_CURSOR_SUPPORT>(isHardCursorSupport);
    return { ScreenPropertyType::IS_HARD_CURSOR_SUPPORT, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetSupportedColorGamuts(
    std::vector<ScreenColorGamut> colorGamuts)
{
    auto prop = property_->Set<ScreenPropertyType::SUPPORTED_COLOR_GAMUTS>(colorGamuts);
    return { ScreenPropertyType::SUPPORTED_COLOR_GAMUTS, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetDisablePowerOffRenderControl(bool disable)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL>(disable);
    return { ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetScreenSwitchStatus(bool status)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SCREEN_SWITCH_STATUS>(status);
    return { ScreenPropertyType::SCREEN_SWITCH_STATUS, prop };
}

RSScreenThreadSafeProperty::ResType RSScreenThreadSafeProperty::SetFrameGravity(int32_t gravity)
{
    UniqueLock lock(propertyMutex_);
    auto prop = property_->Set<ScreenPropertyType::SCREEN_FRAME_GRAVITY>(gravity);
    return { ScreenPropertyType::SCREEN_FRAME_GRAVITY, prop };
}

ScreenId RSScreenThreadSafeProperty::GetId() const
{
    return property_->GetScreenId();
}

bool RSScreenThreadSafeProperty::IsVirtual() const
{
    return property_->IsVirtual();
}

std::string RSScreenThreadSafeProperty::Name() const
{
    return property_->Name();
}

uint32_t RSScreenThreadSafeProperty::GetWidth() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetWidth();
}

uint32_t RSScreenThreadSafeProperty::GetHeight() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetHeight();
}

uint32_t RSScreenThreadSafeProperty::GetPhyWidth() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetPhyWidth();
}

uint32_t RSScreenThreadSafeProperty::GetPhyHeight() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetPhyHeight();
}

uint32_t RSScreenThreadSafeProperty::GetRefreshRate() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetRefreshRate();
}

int32_t RSScreenThreadSafeProperty::GetOffsetX() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetOffsetX();
}

int32_t RSScreenThreadSafeProperty::GetOffsetY() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetOffsetY();
}

bool RSScreenThreadSafeProperty::GetIsSamplingOn() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetIsSamplingOn();
}

float RSScreenThreadSafeProperty::GetSamplingTranslateX() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetSamplingTranslateX();
}

float RSScreenThreadSafeProperty::GetSamplingTranslateY() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetSamplingTranslateY();
}

float RSScreenThreadSafeProperty::GetSamplingScale() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetSamplingScale();
}

ScreenColorGamut RSScreenThreadSafeProperty::GetScreenColorGamut() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenColorGamut();
}

ScreenGamutMap RSScreenThreadSafeProperty::GetScreenGamutMap() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenGamutMap();
}

ScreenState RSScreenThreadSafeProperty::GetState() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetState();
}

ScreenRotation RSScreenThreadSafeProperty::GetScreenCorrection() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenCorrection();
}

bool RSScreenThreadSafeProperty::GetCanvasRotation() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetCanvasRotation();
}

bool RSScreenThreadSafeProperty::GetAutoBufferRotation() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetAutoBufferRotation();
}

RectI RSScreenThreadSafeProperty::GetActiveRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetActiveRect();
}

RectI RSScreenThreadSafeProperty::GetMaskRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetMaskRect();
}

RectI RSScreenThreadSafeProperty::GetReviseRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetReviseRect();
}

uint32_t RSScreenThreadSafeProperty::GetSkipFrameInterval() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenSkipFrameInterval();
}

uint32_t RSScreenThreadSafeProperty::GetExpectedRefreshRate() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenExpectedRefreshRate();
}

SkipFrameStrategy RSScreenThreadSafeProperty::GetSkipFrameStrategy() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenSkipFrameStrategy();
}

GraphicPixelFormat RSScreenThreadSafeProperty::GetPixelFormat() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetPixelFormat();
}

ScreenHDRFormat RSScreenThreadSafeProperty::GetScreenHDRFormat() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenHDRFormat();
}

bool RSScreenThreadSafeProperty::GetEnableVisibleRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetEnableVisibleRect();
}

Rect RSScreenThreadSafeProperty::GetMainScreenVisibleRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetVisibleRect();
}

bool RSScreenThreadSafeProperty::GetVisibleRectSupportRotation() const
{
    SharedLock lock(propertyMutex_);
    return property_->IsVisibleRectSupportRotation();
}

std::unordered_set<uint64_t> RSScreenThreadSafeProperty::GetWhiteList() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetWhiteList();
}

std::unordered_set<uint64_t> RSScreenThreadSafeProperty::GetBlackList() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetBlackList();
}

std::unordered_set<uint8_t> RSScreenThreadSafeProperty::GetTypeBlackList() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetTypeBlackList();
}

std::vector<uint64_t> RSScreenThreadSafeProperty::GetSecurityExemptionList() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetSecurityExemptionList();
}

std::shared_ptr<Media::PixelMap> RSScreenThreadSafeProperty::GetSecurityMask() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetSecurityMask();
}

bool RSScreenThreadSafeProperty::GetCastScreenEnableSkipWindow() const
{
    SharedLock lock(propertyMutex_);
    return property_->EnableSkipWindow();
}

ScreenPowerStatus RSScreenThreadSafeProperty::GetPowerStatus() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenPowerStatus();
}

RSScreenType RSScreenThreadSafeProperty::GetScreenType() const
{
    return property_->GetScreenType();
}

ScreenConnectionType RSScreenThreadSafeProperty::GetConnectionType() const
{
    return property_->GetConnectionType();
}

sptr<Surface> RSScreenThreadSafeProperty::GetProducerSurface() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetProducerSurface();
}

ScreenScaleMode RSScreenThreadSafeProperty::GetScreenScaleMode() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScaleMode();
}

VirtualScreenStatus RSScreenThreadSafeProperty::GetScreenStatus() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetVirtualScreenStatus();
}

int32_t RSScreenThreadSafeProperty::GetVirtualSecLayerOption() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetVirtualSecLayerOption();
}

bool RSScreenThreadSafeProperty::GetIsHardCursorSupport() const
{
    return property_->IsHardCursorSupport();
}

std::vector<ScreenColorGamut> RSScreenThreadSafeProperty::GetSupportedColorGamuts() const
{
    return property_->GetScreenSupportedColorGamuts();
}

ScreenInfo RSScreenThreadSafeProperty::GetScreenInfo() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenInfo();
}

} // namespace Rosen
} // namespace OHOS
