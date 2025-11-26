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
    cloned->id_ = property_->id_;
    cloned->isVirtual_ = property_->isVirtual_;
    cloned->name_ = property_->name_;
    cloned->width_ = property_->width_;
    cloned->height_ = property_->height_;
    cloned->phyWidth_ = property_->phyWidth_ ? property_->phyWidth_ : property_->width_;
    cloned->phyHeight_ = property_->phyHeight_ ? property_->phyHeight_ : property_->height_;
    cloned->refreshRate_ = property_->refreshRate_;
    cloned->offsetX_ = property_->offsetX_;
    cloned->offsetY_ = property_->offsetY_;
    cloned->isSamplingOn_ = property_->isSamplingOn_;
    cloned->samplingTranslateX_ = property_->samplingTranslateX_;
    cloned->samplingTranslateY_ = property_->samplingTranslateY_;
    cloned->samplingScale_ = property_->samplingScale_;
    cloned->colorGamut_ = property_->colorGamut_;
    cloned->gamutMap_ = property_->gamutMap_;
    cloned->state_ = property_->state_;
    cloned->correction_ = property_->correction_;
    cloned->canvasRotation_ = property_->canvasRotation_;
    cloned->autoBufferRotation_ = property_->autoBufferRotation_;
    cloned->activeRect_ = property_->activeRect_;
    cloned->maskRect_ = property_->maskRect_;
    cloned->reviseRect_ = property_->reviseRect_;
    cloned->skipFrameInterval_ = property_->skipFrameInterval_;
    cloned->expectedRefreshRate_ = property_->expectedRefreshRate_;
    cloned->skipFrameStrategy_ = property_->skipFrameStrategy_;
    cloned->pixelFormat_ = property_->pixelFormat_;
    cloned->hdrFormat_ = property_->hdrFormat_;
    cloned->enableVisibleRect_ = property_->enableVisibleRect_;
    cloned->mainScreenVisibleRect_ = property_->mainScreenVisibleRect_;
    cloned->isSupportRotation_ = property_->isSupportRotation_;
    cloned->whiteList_ = property_->whiteList_;
    cloned->blackList_ = property_->blackList_;
    cloned->typeBlackList_ = property_->typeBlackList_;
    cloned->securityExemptionList_ = property_->securityExemptionList_;
    cloned->securityMask_ = property_->securityMask_;
    cloned->skipWindow_ = property_->skipWindow_;
    cloned->powerStatus_ = property_->powerStatus_;
    cloned->screenType_ = property_->screenType_;
    cloned->producerSurface_ = property_->producerSurface_;
    cloned->scaleMode_ = property_->scaleMode_;
    cloned->screenStatus_ = property_->screenStatus_;
    cloned->virtualSecLayerOption_ = property_->virtualSecLayerOption_;
    cloned->isHardCursorSupport_ = property_->isHardCursorSupport_;
    cloned->disablePowerOffRenderControl_ = property_->disablePowerOffRenderControl_;
    cloned->supportedColorGamuts_ = property_->supportedColorGamuts_;
    cloned->screenSwitchStatus_ = property_->screenSwitchStatus_;
    return cloned;
}

void RSScreenThreadSafeProperty::SetId(ScreenId id)
{
    property_->id_ = id;
}

void RSScreenThreadSafeProperty::SetIsVirtual(bool isVirtual)
{
    property_->isVirtual_ = isVirtual;
}

void RSScreenThreadSafeProperty::SetName(const std::string& name)
{
    property_->name_ = name;
}

void RSScreenThreadSafeProperty::SetWidth(uint32_t width)
{
    UniqueLock lock(propertyMutex_);
    property_->width_ = width;
}

void RSScreenThreadSafeProperty::SetHeight(uint32_t height)
{
    UniqueLock lock(propertyMutex_);
    property_->height_ = height;
}

void RSScreenThreadSafeProperty::SetPhyWidth(uint32_t phyWidth)
{
    UniqueLock lock(propertyMutex_);
    property_->phyWidth_ = phyWidth;
}

void RSScreenThreadSafeProperty::SetPhyHeight(uint32_t phyHeight)
{
    UniqueLock lock(propertyMutex_);
    property_->phyHeight_ = phyHeight;
}

void RSScreenThreadSafeProperty::SetRefreshRate(uint32_t refreshRate)
{
    UniqueLock lock(propertyMutex_);
    property_->refreshRate_ = refreshRate;
}

void RSScreenThreadSafeProperty::SetOffsetX(int32_t offsetX)
{
    UniqueLock lock(propertyMutex_);
    property_->offsetX_ = offsetX;
}

void RSScreenThreadSafeProperty::SetOffsetY(int32_t offsetY)
{
    UniqueLock lock(propertyMutex_);
    property_->offsetY_ = offsetY;
}

void RSScreenThreadSafeProperty::SetIsSamplingOn(bool isSamplingOn)
{
    UniqueLock lock(propertyMutex_);
    property_->isSamplingOn_ = isSamplingOn;
}

void RSScreenThreadSafeProperty::SetSamplingTranslateX(float samplingTranslateX)
{
    UniqueLock lock(propertyMutex_);
    property_->samplingTranslateX_ = samplingTranslateX;
}

void RSScreenThreadSafeProperty::SetSamplingTranslateY(float samplingTranslateY)
{
    UniqueLock lock(propertyMutex_);
    property_->samplingTranslateY_ = samplingTranslateY;
}

void RSScreenThreadSafeProperty::SetSamplingScale(float samplingScale)
{
    UniqueLock lock(propertyMutex_);
    property_->samplingScale_ = samplingScale;
}

void RSScreenThreadSafeProperty::SetScreenColorGamut(ScreenColorGamut colorGamut)
{
    UniqueLock lock(propertyMutex_);
    property_->colorGamut_ = colorGamut;
}

void RSScreenThreadSafeProperty::SetScreenGamutMap(ScreenGamutMap gamutMap)
{
    UniqueLock lock(propertyMutex_);
    property_->gamutMap_ = gamutMap;
}

void RSScreenThreadSafeProperty::SetState(ScreenState state)
{
    UniqueLock lock(propertyMutex_);
    property_->state_ = state;
}

void RSScreenThreadSafeProperty::SetScreenCorrection(ScreenRotation screenRotation)
{
    UniqueLock lock(propertyMutex_);
    property_->correction_ = screenRotation;
}

void RSScreenThreadSafeProperty::SetCanvasRotation(bool canvasRotation)
{
    UniqueLock lock(propertyMutex_);
    property_->canvasRotation_ = canvasRotation;
}

void RSScreenThreadSafeProperty::SetAutoBufferRotation(bool isAutoRotation)
{
    UniqueLock lock(propertyMutex_);
    property_->autoBufferRotation_ = isAutoRotation;
}

void RSScreenThreadSafeProperty::SetActiveRect(RectI activeRect)
{
    UniqueLock lock(propertyMutex_);
    property_->activeRect_ = activeRect;
}

void RSScreenThreadSafeProperty::SetMaskRect(RectI maskRect)
{
    UniqueLock lock(propertyMutex_);
    property_->maskRect_ = maskRect;
}

void RSScreenThreadSafeProperty::SetReviseRect(RectI reviseRect)
{
    UniqueLock lock(propertyMutex_);
    property_->reviseRect_ = reviseRect;
}

void RSScreenThreadSafeProperty::SetSkipFrameInterval(uint32_t skipFrameInterval)
{
    UniqueLock lock(propertyMutex_);
    property_->skipFrameInterval_ = skipFrameInterval;
}

void RSScreenThreadSafeProperty::SetExpectedRefreshRate(uint32_t expectedRefreshRate)
{
    UniqueLock lock(propertyMutex_);
    property_->expectedRefreshRate_ = expectedRefreshRate;
}

void RSScreenThreadSafeProperty::SetSkipFrameStrategy(SkipFrameStrategy skipFrameStrategy)
{
    UniqueLock lock(propertyMutex_);
    property_->skipFrameStrategy_ = skipFrameStrategy;
}

void RSScreenThreadSafeProperty::SetPixelFormat(GraphicPixelFormat pixelFormat)
{
    UniqueLock lock(propertyMutex_);
    property_->pixelFormat_ = pixelFormat;
}

void RSScreenThreadSafeProperty::SetScreenHDRFormat(ScreenHDRFormat hdrFormat)
{
    UniqueLock lock(propertyMutex_);
    property_->hdrFormat_ = hdrFormat;
}

void RSScreenThreadSafeProperty::SetEnableVisibleRect(bool enableVisibleRect)
{
    UniqueLock lock(propertyMutex_);
    property_->enableVisibleRect_ = enableVisibleRect;
}

void RSScreenThreadSafeProperty::SetMainScreenVisibleRect(const Rect& mainScreenRect)
{
    UniqueLock lock(propertyMutex_);
    property_->mainScreenVisibleRect_ = mainScreenRect;
}

void RSScreenThreadSafeProperty::SetVisibleRectSupportRotation(bool supportRotation)
{
    UniqueLock lock(propertyMutex_);
    property_->isSupportRotation_ = supportRotation;
}

void RSScreenThreadSafeProperty::SetWhiteList(const std::unordered_set<uint64_t>& whiteList)
{
    UniqueLock lock(propertyMutex_);
    property_->whiteList_ = whiteList;
}

void RSScreenThreadSafeProperty::SetBlackList(const std::unordered_set<uint64_t>& blackList)
{
    UniqueLock lock(propertyMutex_);
    property_->blackList_ = blackList;
}

void RSScreenThreadSafeProperty::AddBlackList(const std::vector<uint64_t>& blackList)
{
    UniqueLock lock(propertyMutex_);
    property_->blackList_.insert(blackList.cbegin(), blackList.cend());
}

void RSScreenThreadSafeProperty::RemoveBlackList(const std::vector<uint64_t>& blackList)
{
    UniqueLock lock(propertyMutex_);
    for (const auto& list : blackList) {
        property_->blackList_.erase(list);
    }
}

void RSScreenThreadSafeProperty::SetTypeBlackList(const std::unordered_set<uint8_t>& typeBlackList)
{
    UniqueLock lock(propertyMutex_);
    property_->typeBlackList_ = typeBlackList;
}

void RSScreenThreadSafeProperty::SetSecurityExemptionList(const std::vector<uint64_t>& securityExemptionList)
{
    UniqueLock lock(propertyMutex_);
    property_->securityExemptionList_ = securityExemptionList;
}

void RSScreenThreadSafeProperty::SetSecurityMask(std::shared_ptr<Media::PixelMap> securityMask)
{
    UniqueLock lock(propertyMutex_);
    property_->securityMask_ = securityMask;
}

void RSScreenThreadSafeProperty::SetCastScreenEnableSkipWindow(bool enable)
{
    UniqueLock lock(propertyMutex_);
    property_->skipWindow_ = enable;
}

void RSScreenThreadSafeProperty::SetPowerStatus(ScreenPowerStatus powerStatus)
{
    UniqueLock lock(propertyMutex_);
    property_->powerStatus_ = powerStatus;
}

void RSScreenThreadSafeProperty::SetScreenType(RSScreenType screenType)
{
    UniqueLock lock(propertyMutex_);
    property_->screenType_ = screenType;
}

void RSScreenThreadSafeProperty::SetProducerSurface(sptr<Surface> producerSurface)
{
    UniqueLock lock(propertyMutex_);
    property_->producerSurface_ = producerSurface;
}

void RSScreenThreadSafeProperty::SetScreenScaleMode(ScreenScaleMode scaleMode)
{
    UniqueLock lock(propertyMutex_);
    property_->scaleMode_ = scaleMode;
}

void RSScreenThreadSafeProperty::SetScreenStatus(VirtualScreenStatus screenStatus)
{
    UniqueLock lock(propertyMutex_);
    property_->screenStatus_ = screenStatus;
}

void RSScreenThreadSafeProperty::SetVirtualSecLayerOption(int32_t virtualSecLayerOption)
{
    UniqueLock lock(propertyMutex_);
    property_->virtualSecLayerOption_ = virtualSecLayerOption;
}

void RSScreenThreadSafeProperty::SetIsHardCursorSupport(bool isHardCursorSupport)
{
    UniqueLock lock(propertyMutex_);
    property_->isHardCursorSupport_ = isHardCursorSupport;
}

void RSScreenThreadSafeProperty::SetSupportedColorGamuts(std::vector<ScreenColorGamut> colorGamuts)
{
    UniqueLock lock(propertyMutex_);
    property_->supportedColorGamuts_ = colorGamuts;
}

void RSScreenThreadSafeProperty::SetDisablePowerOffRenderControl(bool disable)
{
    UniqueLock lock(propertyMutex_);
    property_->disablePowerOffRenderControl_ = disable;
}

void RSScreenThreadSafeProperty::SetScreenSwitchStatus(bool status)
{
    UniqueLock lock(propertyMutex_);
    property_->screenSwitchStatus_ = status;
}

ScreenId RSScreenThreadSafeProperty::GetId() const
{
    return property_->id_;
}

bool RSScreenThreadSafeProperty::IsVirtual() const
{
    return property_->isVirtual_;
}

const std::string& RSScreenThreadSafeProperty::Name() const
{
    return property_->name_;
}

uint32_t RSScreenThreadSafeProperty::GetWidth() const
{
    SharedLock lock(propertyMutex_);
    return property_->width_;
}

uint32_t RSScreenThreadSafeProperty::GetHeight() const
{
    SharedLock lock(propertyMutex_);
    return property_->height_;
}

uint32_t RSScreenThreadSafeProperty::GetPhyWidth() const
{
    SharedLock lock(propertyMutex_);
    return property_->phyWidth_;
}

uint32_t RSScreenThreadSafeProperty::GetPhyHeight() const
{
    SharedLock lock(propertyMutex_);
    return property_->phyHeight_;
}

uint32_t RSScreenThreadSafeProperty::GetRefreshRate() const
{
    SharedLock lock(propertyMutex_);
    return property_->refreshRate_;
}

int32_t RSScreenThreadSafeProperty::GetOffsetX() const
{
    SharedLock lock(propertyMutex_);
    return property_->offsetX_;
}

int32_t RSScreenThreadSafeProperty::GetOffsetY() const
{
    SharedLock lock(propertyMutex_);
    return property_->offsetY_;
}

bool RSScreenThreadSafeProperty::GetIsSamplingOn() const
{
    SharedLock lock(propertyMutex_);
    return property_->isSamplingOn_;
}

float RSScreenThreadSafeProperty::GetSamplingTranslateX() const
{
    SharedLock lock(propertyMutex_);
    return property_->samplingTranslateX_;
}

float RSScreenThreadSafeProperty::GetSamplingTranslateY() const
{
    SharedLock lock(propertyMutex_);
    return property_->samplingTranslateY_;
}

float RSScreenThreadSafeProperty::GetSamplingScale() const
{
    SharedLock lock(propertyMutex_);
    return property_->samplingScale_;
}

ScreenColorGamut RSScreenThreadSafeProperty::GetScreenColorGamut() const
{
    SharedLock lock(propertyMutex_);
    return property_->colorGamut_;
}

ScreenGamutMap RSScreenThreadSafeProperty::GetScreenGamutMap() const
{
    SharedLock lock(propertyMutex_);
    return property_->gamutMap_;
}

ScreenState RSScreenThreadSafeProperty::GetState() const
{
    SharedLock lock(propertyMutex_);
    return property_->state_;
}

ScreenRotation RSScreenThreadSafeProperty::GetScreenCorrection() const
{
    SharedLock lock(propertyMutex_);
    return property_->correction_;
}

bool RSScreenThreadSafeProperty::GetCanvasRotation() const
{
    SharedLock lock(propertyMutex_);
    return property_->canvasRotation_;
}

bool RSScreenThreadSafeProperty::GetAutoBufferRotation() const
{
    SharedLock lock(propertyMutex_);
    return property_->autoBufferRotation_;
}

RectI RSScreenThreadSafeProperty::GetActiveRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->activeRect_;
}

RectI RSScreenThreadSafeProperty::GetMaskRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->maskRect_;
}

RectI RSScreenThreadSafeProperty::GetReviseRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->reviseRect_;
}

uint32_t RSScreenThreadSafeProperty::GetSkipFrameInterval() const
{
    SharedLock lock(propertyMutex_);
    return property_->skipFrameInterval_;
}

uint32_t RSScreenThreadSafeProperty::GetExpectedRefreshRate() const
{
    SharedLock lock(propertyMutex_);
    return property_->expectedRefreshRate_;
}

SkipFrameStrategy RSScreenThreadSafeProperty::GetSkipFrameStrategy() const
{
    SharedLock lock(propertyMutex_);
    return property_->skipFrameStrategy_;
}

GraphicPixelFormat RSScreenThreadSafeProperty::GetPixelFormat() const
{
    SharedLock lock(propertyMutex_);
    return property_->pixelFormat_;
}

ScreenHDRFormat RSScreenThreadSafeProperty::GetScreenHDRFormat() const
{
    SharedLock lock(propertyMutex_);
    return property_->hdrFormat_;
}

bool RSScreenThreadSafeProperty::GetEnableVisibleRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->enableVisibleRect_;
}

Rect RSScreenThreadSafeProperty::GetMainScreenVisibleRect() const
{
    SharedLock lock(propertyMutex_);
    return property_->mainScreenVisibleRect_;
}

bool RSScreenThreadSafeProperty::GetVisibleRectSupportRotation() const
{
    SharedLock lock(propertyMutex_);
    return property_->isSupportRotation_;
}

std::unordered_set<uint64_t> RSScreenThreadSafeProperty::GetWhiteList() const
{
    SharedLock lock(propertyMutex_);
    return property_->whiteList_;
}

std::unordered_set<uint64_t> RSScreenThreadSafeProperty::GetBlackList() const
{
    SharedLock lock(propertyMutex_);
    return property_->blackList_;
}

std::unordered_set<uint8_t> RSScreenThreadSafeProperty::GetTypeBlackList() const
{
    SharedLock lock(propertyMutex_);
    return property_->typeBlackList_;
}

std::vector<uint64_t> RSScreenThreadSafeProperty::GetSecurityExemptionList() const
{
    SharedLock lock(propertyMutex_);
    return property_->securityExemptionList_;
}

std::shared_ptr<Media::PixelMap> RSScreenThreadSafeProperty::GetSecurityMask() const
{
    SharedLock lock(propertyMutex_);
    return property_->securityMask_;
}

bool RSScreenThreadSafeProperty::GetCastScreenEnableSkipWindow() const
{
    SharedLock lock(propertyMutex_);
    return property_->skipWindow_;
}

ScreenPowerStatus RSScreenThreadSafeProperty::GetPowerStatus() const
{
    SharedLock lock(propertyMutex_);
    return property_->powerStatus_;
}

RSScreenType RSScreenThreadSafeProperty::GetScreenType() const
{
    SharedLock lock(propertyMutex_);
    return property_->screenType_;
}

sptr<Surface> RSScreenThreadSafeProperty::GetProducerSurface() const
{
    SharedLock lock(propertyMutex_);
    return property_->producerSurface_;
}

ScreenScaleMode RSScreenThreadSafeProperty::GetScreenScaleMode() const
{
    SharedLock lock(propertyMutex_);
    return property_->scaleMode_;
}

VirtualScreenStatus RSScreenThreadSafeProperty::GetScreenStatus() const
{
    SharedLock lock(propertyMutex_);
    return property_->screenStatus_;
}

int32_t RSScreenThreadSafeProperty::GetVirtualSecLayerOption() const
{
    SharedLock lock(propertyMutex_);
    return property_->virtualSecLayerOption_;
}

bool RSScreenThreadSafeProperty::GetIsHardCursorSupport() const
{
    SharedLock lock(propertyMutex_);
    return property_->isHardCursorSupport_;
}

std::vector<ScreenColorGamut> RSScreenThreadSafeProperty::GetSupportedColorGamuts() const
{
    SharedLock lock(propertyMutex_);
    return property_->supportedColorGamuts_;
}

ScreenInfo RSScreenThreadSafeProperty::GetScreenInfo() const
{
    SharedLock lock(propertyMutex_);
    return property_->GetScreenInfo();
}

} // namespace Rosen
} // namespace OHOS
