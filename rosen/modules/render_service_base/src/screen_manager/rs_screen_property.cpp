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
#include "screen_manager/rs_screen_property.h"
#include "platform/common/rs_log.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
RSScreenProperty::RSScreenProperty() {}
RSScreenProperty::~RSScreenProperty() {}

ScreenId RSScreenProperty::GetScreenId() const
{
    return id_;
}

bool RSScreenProperty::IsVirtual() const
{
    return isVirtual_;
}

const std::string& RSScreenProperty::Name() const
{
    return name_;
}

uint32_t RSScreenProperty::GetWidth() const
{
    return width_;
}

uint32_t RSScreenProperty::GetHeight() const
{
    return height_;
}

uint32_t RSScreenProperty::GetPhyWidth() const
{
    return phyWidth_;
}

uint32_t RSScreenProperty::GetPhyHeight() const
{
    return phyHeight_;
}

uint32_t RSScreenProperty::GetRefreshRate() const
{
    return refreshRate_;
}

int32_t RSScreenProperty::GetOffsetX() const
{
    return offsetX_;
}

int32_t RSScreenProperty::GetOffsetY() const
{
    return offsetY_;
}

bool RSScreenProperty::GetIsSamplingOn() const
{
    return isSamplingOn_;
}

float RSScreenProperty::GetSamplingTranslateX() const
{
    return samplingTranslateX_;
}

float RSScreenProperty::GetSamplingTranslateY() const
{
    return samplingTranslateY_;
}

float RSScreenProperty::GetSamplingScale() const
{
    return samplingScale_;
}

ScreenColorGamut RSScreenProperty::GetScreenColorGamut() const
{
    return colorGamut_;
}

ScreenGamutMap RSScreenProperty::GetScreenGamutMap() const
{
    return gamutMap_;
}

ScreenState RSScreenProperty::GetState() const
{
    return state_;
}

ScreenRotation RSScreenProperty::GetScreenCorrection() const
{
    return correction_;
}

bool RSScreenProperty::GetCanvasRotation() const
{
    return canvasRotation_;
}

bool RSScreenProperty::GetAutoBufferRotation() const
{
    return autoBufferRotation_;
}

RectI RSScreenProperty::GetActiveRect() const
{
    return activeRect_;
}

RectI RSScreenProperty::GetMaskRect() const
{
    return maskRect_;
}

RectI RSScreenProperty::GetReviseRect() const
{
    return reviseRect_;
}

uint32_t RSScreenProperty::GetScreenSkipFrameInterval() const
{
    return skipFrameInterval_;
}

uint32_t RSScreenProperty::GetScreenExpectedRefreshRate() const
{
    return expectedRefreshRate_;
}

SkipFrameStrategy RSScreenProperty::GetScreenSkipFrameStrategy() const
{
    return skipFrameStrategy_;
}

GraphicPixelFormat RSScreenProperty::GetPixelFormat() const
{
    return pixelFormat_;
}

ScreenHDRFormat RSScreenProperty::GetScreenHDRFormat() const
{
    return hdrFormat_;
}

bool RSScreenProperty::GetEnableVisibleRect() const
{
    return enableVisibleRect_;
}

Rect RSScreenProperty::GetVisibleRect() const
{
    return mainScreenVisibleRect_;
}

bool RSScreenProperty::IsVisibleRectSupportRotation() const
{
    return isSupportRotation_;
}

const std::unordered_set<NodeId>& RSScreenProperty::GetWhiteList() const
{
    return whiteList_;
}

const std::unordered_set<NodeId>& RSScreenProperty::GetBlackList() const
{
    return blackList_;
}

const std::unordered_set<NodeType>& RSScreenProperty::GetTypeBlackList() const
{
    return typeBlackList_;
}

const std::vector<NodeId>& RSScreenProperty::GetSecurityExemptionList() const
{
    return securityExemptionList_;
}

std::shared_ptr<Media::PixelMap> RSScreenProperty::GetSecurityMask() const
{
    return securityMask_;
}

bool RSScreenProperty::EnableSkipWindow() const
{
    return skipWindow_;
}

ScreenPowerStatus RSScreenProperty::GetScreenPowerStatus() const
{
    return powerStatus_;
}

RSScreenType RSScreenProperty::GetScreenType() const
{
    return screenType_;
}

ScreenConnectionType RSScreenProperty::GetConnectionType() const
{
    return connectionType_;
}

#ifndef ROSEN_CROSS_PLATFORM
sptr<Surface> RSScreenProperty::GetProducerSurface() const
{
    return producerSurface_;
}
#endif

ScreenScaleMode RSScreenProperty::GetScaleMode() const
{
    return scaleMode_;
}

VirtualScreenStatus RSScreenProperty::GetVirtualScreenStatus() const
{
    return screenStatus_;
}

int32_t RSScreenProperty::GetVirtualSecLayerOption() const
{
    return virtualSecLayerOption_;
}

bool RSScreenProperty::IsHardCursorSupport() const
{
    return isHardCursorSupport_;
}

const std::vector<ScreenColorGamut>& RSScreenProperty::GetScreenSupportedColorGamuts() const
{
    return supportedColorGamuts_;
}

bool RSScreenProperty::GetDisablePowerOffRenderControl() const
{
    return disablePowerOffRenderControl_;
}

ScreenPowerStatus RSScreenProperty::GetPowerStatus() const
{
    return powerStatus_;
}

bool RSScreenProperty::IsScreenSwitching() const
{
    return screenSwitchStatus_;
}

ScreenInfo RSScreenProperty::GetScreenInfo() const
{
    ScreenInfo info;
    info.id = id_;
    info.width = width_;
    info.height = height_;
    info.phyWidth = phyWidth_;
    info.phyHeight = phyHeight_;
    info.offsetX = offsetX_;
    info.offsetY = offsetY_;
    info.isSamplingOn = isSamplingOn_;
    info.samplingTranslateX = samplingTranslateX_;
    info.samplingTranslateY = samplingTranslateY_;
    info.samplingScale = samplingScale_;
    info.colorGamut = colorGamut_;
    info.gamutMap = gamutMap_;
    info.state = state_;
    info.skipFrameInterval = skipFrameInterval_;
    info.expectedRefreshRate = expectedRefreshRate_;
    info.skipFrameStrategy = skipFrameStrategy_;
    info.pixelFormat = pixelFormat_;
    info.hdrFormat = hdrFormat_;
    info.whiteList = whiteList_;
    info.enableVisibleRect = enableVisibleRect_;
    info.activeRect = activeRect_;
    info.maskRect = maskRect_;
    info.reviseRect = reviseRect_;
    info.powerStatus = powerStatus_;
    return info;
}

bool RSScreenProperty::Marshalling(Parcel& data) const
{
    if (!data.WriteUint64(id_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint64 id err.");
        return false;
    }
    if (!data.WriteBool(isVirtual_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool isVirtual err.");
        return false;
    }
    if (!data.WriteString(name_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteString name err.");
        return false;
    }
    if (!data.WriteUint32(width_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 width err.");
        return false;
    }
    if (!data.WriteUint32(height_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 height err.");
        return false;
    }
    if (!data.WriteUint32(phyWidth_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 phyWidth err.");
        return false;
    }
    if (!data.WriteUint32(phyHeight_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 phyHeight err.");
        return false;
    }
    if (!data.WriteUint32(refreshRate_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 refreshRate err.");
        return false;
    }
    if (!data.WriteInt32(offsetX_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteInt32 offsetX err.");
        return false;
    }
    if (!data.WriteInt32(offsetY_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteInt32 offsetY err.");
        return false;
    }
    if (!data.WriteBool(isSamplingOn_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool isSamplingOn err.");
        return false;
    }
    if (!data.WriteFloat(samplingTranslateX_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteFloat samplingTranslateX err.");
        return false;
    }
    if (!data.WriteFloat(samplingTranslateY_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteFloat samplingTranslateY err.");
        return false;
    }
    if (!data.WriteFloat(samplingScale_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteFloat samplingScale err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(colorGamut_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 colorGamut err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(gamutMap_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 gamutMap err.");
        return false;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(state_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint8 state err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(correction_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 correction err.");
        return false;
    }
    if (!data.WriteBool(canvasRotation_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool canvasRotation err.");
        return false;
    }
    if (!data.WriteBool(autoBufferRotation_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool autoBufferRotation err.");
        return false;
    }
#ifdef ROSEN_OHOS
    if (!activeRect_.Marshalling(data)) {
        ROSEN_LOGE("WriteScreenProperty: Marshalling activeRect err.");
        return false;
    }
    if (!maskRect_.Marshalling(data)) {
        ROSEN_LOGE("WriteScreenProperty: Marshalling maskRect err.");
        return false;
    }
    if (!reviseRect_.Marshalling(data)) {
        ROSEN_LOGE("WriteScreenProperty: Marshalling reviseRect err.");
        return false;
    }
#endif
    if (!data.WriteUint32(skipFrameInterval_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 skipFrameInterval err.");
        return false;
    }
    if (!data.WriteUint32(expectedRefreshRate_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 expectedRefreshRate err.");
        return false;
    }
    if (!data.WriteInt32(static_cast<int32_t>(skipFrameStrategy_))) {
        ROSEN_LOGE("WriteScreenProperty: Write skipFrameStrategy err.");
        return false;
    }
    if (!data.WriteInt32(static_cast<int32_t>(pixelFormat_))) {
        ROSEN_LOGE("WriteScreenProperty: Write pixelFormat err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(hdrFormat_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 hdrFormat err.");
        return false;
    }
    if (!data.WriteBool(enableVisibleRect_)) {
        ROSEN_LOGE("WriteScreenProperty: Write enableVisibleRect err.");
        return false;
    }
    if (!data.WriteInt32(mainScreenVisibleRect_.x) || !data.WriteInt32(mainScreenVisibleRect_.y) ||
        !data.WriteInt32(mainScreenVisibleRect_.w) || !data.WriteInt32(mainScreenVisibleRect_.h)) {
        ROSEN_LOGE("WriteScreenProperty: Write mainScreenVisibleRect err.");
        return false;
    }
    if (!data.WriteBool(isSupportRotation_)) {
        ROSEN_LOGE("WriteScreenProperty: Write isSupportRotation err.");
        return false;
    }

    std::vector<uint64_t> whiteListVec;
    whiteListVec.assign(whiteList_.begin(), whiteList_.end());
    if (!data.WriteUInt64Vector(whiteListVec)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUInt64Vector whiteList err.");
        return false;
    }

    std::vector<uint64_t> blackListVec;
    blackListVec.assign(blackList_.begin(), blackList_.end());
    if (!data.WriteUInt64Vector(blackListVec)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUInt64Vector blackList err.");
        return false;
    }

    std::vector<uint8_t> typeBlackListVec;
    typeBlackListVec.assign(typeBlackList_.begin(), typeBlackList_.end());
    if (!data.WriteUInt8Vector(typeBlackListVec)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUInt8Vector typeBlackList err.");
        return false;
    }

    if (!data.WriteUInt64Vector(securityExemptionList_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUInt64Vector securityExemptionList err.");
        return false;
    }

    if (securityMask_) {
        if (!data.WriteBool(true) || !data.WriteParcelable(securityMask_.get())) {
            ROSEN_LOGE("WriteScreenProperty: WriteBool[true] OR WriteParcelable[securityMask.get()] err.");
            return false;
        }
    } else {
        if (!data.WriteBool(false)) {
            ROSEN_LOGE("WriteScreenProperty: WriteBool [false] err.");
            return false;
        }
    }

    if (!data.WriteBool(skipWindow_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool skipWindow err.");
        return false;
    }
    if (!data.WriteUint32(powerStatus_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool powerStatus err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(scaleMode_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool scaleMode err.");
        return false;
    }

#ifndef ROSEN_CROSS_PLATFORM
    if (!producerSurface_ || !producerSurface_->GetProducer()) {
        data.WriteBool(false);
    } else {
        data.WriteBool(true);
        auto producer = producerSurface_->GetProducer();
        if (!data.WriteRemoteObject(producer->AsObject())) {
            ROSEN_LOGE("WriteScreenProperty: WriteRemoteObject producer->AsObject() err.");
            return false;
        }
    }
#endif

    if (!data.WriteUint32(static_cast<uint32_t>(screenStatus_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 screenStatus err.");
        return false;
    }
    if (!data.WriteInt32(virtualSecLayerOption_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteInt32 virtualSecLayerOption err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(screenType_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 screenType err.");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(connectionType_))) {
        ROSEN_LOGE("WriteScreenProperty: WriteUint32 connectionType_ err.");
        return false;
    }
    if (!data.WriteBool(isHardCursorSupport_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool isHardCursorSupport err.");
        return false;
    }

    std::vector<uint32_t> supportedColorGamuts;
    supportedColorGamuts.assign(supportedColorGamuts_.begin(), supportedColorGamuts_.end());
    if (!data.WriteUInt32Vector(supportedColorGamuts)) {
        ROSEN_LOGE("WriteScreenProperty: WriteUInt32Vector supportedColorGamut err.");
        return false;
    }

    if (!data.WriteBool(disablePowerOffRenderControl_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool disablePowerOffRenderControl err.");
        return false;
    }
    if (!data.WriteBool(screenSwitchStatus_)) {
        ROSEN_LOGE("WriteScreenProperty: WriteBool screenSwitchStatus err.");
        return false;
    }
    return true;
}

RSScreenProperty* RSScreenProperty::Unmarshalling(Parcel& data)
{
    auto screenProperty = new RSScreenProperty();
    if (screenProperty->UnmarshallingData(data)) {
        return screenProperty;
    }
    delete screenProperty;
    return nullptr;
}

bool RSScreenProperty::UnmarshallingData(Parcel& data)
{
    if (!data.ReadUint64(id_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint64 id err.");
        return false;
    }
    if (!data.ReadBool(isVirtual_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool isVirtual err.");
        return false;
    }
    if (!data.ReadString(name_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadString name err.");
        return false;
    }
    if (!data.ReadUint32(width_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 width err.");
        return false;
    }
    if (!data.ReadUint32(height_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 height err.");
        return false;
    }
    if (!data.ReadUint32(phyWidth_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 phyWidth err.");
        return false;
    }
    if (!data.ReadUint32(phyHeight_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 phyHeight err.");
        return false;
    }
    if (!data.ReadUint32(refreshRate_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 refreshRate err.");
        return false;
    }
    if (!data.ReadInt32(offsetX_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadInt32 offsetX err.");
        return false;
    }
    if (!data.ReadInt32(offsetY_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadInt32 offsetY err.");
        return false;
    }
    if (!data.ReadBool(isSamplingOn_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool isSamplingOn err.");
        return false;
    }
    if (!data.ReadFloat(samplingTranslateX_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadFloat samplingTranslateX err.");
        return false;
    }
    if (!data.ReadFloat(samplingTranslateY_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadFloat samplingTranslateY err.");
        return false;
    }
    if (!data.ReadFloat(samplingScale_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadFloat samplingScale err.");
        return false;
    }
    uint32_t colorGamut = 0;
    if (!data.ReadUint32(colorGamut)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 colorGamut err.");
        return false;
    }
    colorGamut_ = static_cast<ScreenColorGamut>(colorGamut);
    uint32_t gamutMap = 0;
    if (!data.ReadUint32(gamutMap)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 gamutMap err.");
        return false;
    }
    gamutMap_ = static_cast<ScreenGamutMap>(gamutMap);
    uint8_t state = 0;
    if (!data.ReadUint8(state)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint8 state err.");
        return false;
    }
    state_ = static_cast<ScreenState>(state);
    uint32_t correction = 0;
    if (!data.ReadUint32(correction)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 rotation err.");
        return false;
    }
    correction_ = static_cast<ScreenRotation>(correction);

    if (!data.ReadBool(canvasRotation_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool canvasRotation err.");
        return false;
    }
    if (!data.ReadBool(autoBufferRotation_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool autoBufferRotation err.");
        return false;
    }

    // Rects
#ifdef ROSEN_OHOS
    if (!RSMarshallingHelper::Unmarshalling(data, activeRect_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: Marshalling activeRect err.");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(data, maskRect_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: Marshalling maskRect err.");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(data, reviseRect_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: Marshalling reviseRect err.");
        return false;
    }
#endif

    if (!data.ReadUint32(skipFrameInterval_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 skipFrameInterval err.");
        return false;
    }
    if (!data.ReadUint32(expectedRefreshRate_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 expectedRefreshRate err.");
        return false;
    }
    int32_t skipFrameStrategy = 0;
    if (!data.ReadInt32(skipFrameStrategy)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadInt32 skipFrameStrategy err.");
        return false;
    }
    skipFrameStrategy_ = static_cast<SkipFrameStrategy>(skipFrameStrategy);
    int32_t pixelFormat = 0;
    if (!data.ReadInt32(pixelFormat)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadInt32 pixelFormat err.");
        return false;
    }
    pixelFormat_ = static_cast<GraphicPixelFormat>(pixelFormat);
    uint32_t hdrFormat = 0;
    if (!data.ReadUint32(hdrFormat)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 hdrFormat err.");
        return false;
    }
    hdrFormat_ = static_cast<ScreenHDRFormat>(hdrFormat);
    if (!data.ReadBool(enableVisibleRect_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool enableVisibleRect err.");
        return false;
    }

    int32_t x = -1;
    int32_t y = -1;
    int32_t w = -1;
    int32_t h = -1;
    if (!data.ReadInt32(x) || !data.ReadInt32(y) ||
        !data.ReadInt32(w) || !data.ReadInt32(h)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool visibleRect err.");
        return false;
    }
    mainScreenVisibleRect_ = {x, y, w, h};
    if (!data.ReadBool(isSupportRotation_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool isSupportRotation err.");
        return false;
    }

    std::vector<uint64_t> whiteListVec;
    if (!data.ReadUInt64Vector(&whiteListVec)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUInt64Vector whiteList err.");
        return false;
    }
    whiteList_ = std::unordered_set<uint64_t>(whiteListVec.begin(), whiteListVec.end());

    std::vector<NodeId> blackListVec;
    if (!data.ReadUInt64Vector(&blackListVec)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUInt64Vector blackList err.");
        return false;
    }
    blackList_ = std::unordered_set<NodeId>(blackListVec.begin(), blackListVec.end());

    std::vector<uint8_t> typeBlackListVec;
    if (!data.ReadUInt8Vector(&typeBlackListVec)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUInt8Vector typeBlackList err.");
        return false;
    }
    typeBlackList_ = std::unordered_set<NodeType>(typeBlackListVec.begin(), typeBlackListVec.end());

    if (!data.ReadUInt64Vector(&securityExemptionList_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUInt64Vector securityExemptionList err.");
        return false;
    }

    bool enable{false};
    if (!data.ReadBool(enable)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool securityMask err.");
        return false;
    }
    if (enable) {
        securityMask_ = std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
    }

    if (!data.ReadBool(skipWindow_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool skipWindow err.");
        return false;
    }

    uint32_t powerStatus = 0;
    if (!data.ReadUint32(powerStatus)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 powerStatus err.");
        return false;
    }
    powerStatus_ = static_cast<ScreenPowerStatus>(powerStatus);

    uint32_t scaleMode = 0;
    if (!data.ReadUint32(scaleMode)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 scaleMode err.");
        return false;
    }
    scaleMode_ = static_cast<ScreenScaleMode>(scaleMode);

#ifndef ROSEN_CROSS_PLATFORM
    bool hasSurface = false;
    if (!data.ReadBool(hasSurface)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool hasSurface err.");
        return false;
    }
    if (hasSurface) {
        auto remoteObject = static_cast<MessageParcel*>(&data)->ReadRemoteObject();
        if (remoteObject != nullptr) {
            auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
            producerSurface_ = Surface::CreateSurfaceAsProducer(bufferProducer);
        }
    } else {
        producerSurface_ = nullptr;
    }
#endif

    uint32_t screenStatus = 0;
    if (!data.ReadUint32(screenStatus)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 screenStatus err.");
        return false;
    }
    screenStatus_ = static_cast<VirtualScreenStatus>(screenStatus);

    if (!data.ReadInt32(virtualSecLayerOption_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadInt32 virtualSecLayerOption err.");
        return false;
    }
    uint32_t screenType = 0;
    if (!data.ReadUint32(screenType)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 screenType err.");
        return false;
    }
    screenType_ = static_cast<RSScreenType>(screenType);
    uint32_t connectionType = 0;
    if (!data.ReadUint32(connectionType)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUint32 connectionType err.");
        return false;
    }
    connectionType_ = static_cast<ScreenConnectionType>(connectionType);
    if (!data.ReadBool(isHardCursorSupport_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool isHardCursorSupport err.");
        return false;
    }

    std::vector<uint32_t> supportedColorGamuts;
    if (!data.ReadUInt32Vector(&supportedColorGamuts)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadUInt32Vector supportedColorGamuts err.");
        return false;
    }
    supportedColorGamuts_.resize(supportedColorGamuts.size());
    std::transform(supportedColorGamuts.begin(), supportedColorGamuts.end(), supportedColorGamuts_.begin(),
        [](auto val) { return static_cast<ScreenColorGamut>(val); });

    if (!data.ReadBool(disablePowerOffRenderControl_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool disablePowerOffRenderControl err.");
        return false;
    }
    if (!data.ReadBool(screenSwitchStatus_)) {
        ROSEN_LOGE("RSScreenProperty::Unmarshalling: ReadBool screenSwitchStatus err.");
        return false;
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS
