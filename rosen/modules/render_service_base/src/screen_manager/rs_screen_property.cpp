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
// LCOV_EXCL_START
ScreenId RSScreenProperty::GetScreenId() const
{
    return Get<ScreenPropertyType::ID>();
}

bool RSScreenProperty::IsVirtual() const
{
    return Get<ScreenPropertyType::IS_VIRTUAL>();
}

std::string RSScreenProperty::Name() const
{
    return Get<ScreenPropertyType::NAME>();
}

uint32_t RSScreenProperty::GetWidth() const
{
    auto res = Get<ScreenPropertyType::RENDER_RESOLUTION>();
    return res.first;
}

uint32_t RSScreenProperty::GetHeight() const
{
    auto res = Get<ScreenPropertyType::RENDER_RESOLUTION>();
    return res.second;
}

uint32_t RSScreenProperty::GetPhyWidth() const
{
    auto res = Get<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>();
    return std::get<0>(res);
}

uint32_t RSScreenProperty::GetPhyHeight() const
{
    auto res = Get<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>();
    return std::get<1>(res);
}

uint32_t RSScreenProperty::GetRefreshRate() const
{
    auto res = Get<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>();
    return std::get<2>(res);
}

int32_t RSScreenProperty::GetOffsetX() const
{
    auto res = Get<ScreenPropertyType::OFFSET>();
    return res.first;
}

int32_t RSScreenProperty::GetOffsetY() const
{
    auto res = Get<ScreenPropertyType::OFFSET>();
    return res.second;
}

bool RSScreenProperty::GetIsSamplingOn() const
{
    auto res = Get<ScreenPropertyType::SAMPLING_OPTION>();
    return std::get<0>(res);
}

float RSScreenProperty::GetSamplingTranslateX() const
{
    auto res = Get<ScreenPropertyType::SAMPLING_OPTION>();
    return std::get<1>(res);
}

float RSScreenProperty::GetSamplingTranslateY() const
{
    auto res = Get<ScreenPropertyType::SAMPLING_OPTION>();
    return std::get<2>(res);
}

float RSScreenProperty::GetSamplingScale() const
{
    auto res = Get<ScreenPropertyType::SAMPLING_OPTION>();
    return std::get<3>(res);
}

ScreenColorGamut RSScreenProperty::GetScreenColorGamut() const
{
    return static_cast<ScreenColorGamut>(Get<ScreenPropertyType::COLOR_GAMUT>());
}

ScreenGamutMap RSScreenProperty::GetScreenGamutMap() const
{
    return static_cast<ScreenGamutMap>(Get<ScreenPropertyType::GAMUT_MAP>());
}

ScreenState RSScreenProperty::GetState() const
{
    return static_cast<ScreenState>(Get<ScreenPropertyType::STATE>());
}

ScreenRotation RSScreenProperty::GetScreenCorrection() const
{
    return static_cast<ScreenRotation>(Get<ScreenPropertyType::CORRECTION>());
}

bool RSScreenProperty::GetCanvasRotation() const
{
    return Get<ScreenPropertyType::CANVAS_ROTATION>();
}

bool RSScreenProperty::GetAutoBufferRotation() const
{
    return Get<ScreenPropertyType::AUTO_BUFFER_ROTATION>();
}

RectI RSScreenProperty::GetActiveRect() const
{
    auto res = Get<ScreenPropertyType::ACTIVE_RECT_OPTION>();
    return std::get<0>(res);
}

RectI RSScreenProperty::GetMaskRect() const
{
    auto res = Get<ScreenPropertyType::ACTIVE_RECT_OPTION>();
    return std::get<1>(res);
}

RectI RSScreenProperty::GetReviseRect() const
{
    auto res = Get<ScreenPropertyType::ACTIVE_RECT_OPTION>();
    return std::get<2>(res);
}

uint32_t RSScreenProperty::GetScreenSkipFrameInterval() const
{
    auto res = Get<ScreenPropertyType::SKIP_FRAME_OPTION>();
    return std::get<0>(res);
}

uint32_t RSScreenProperty::GetScreenExpectedRefreshRate() const
{
    auto res = Get<ScreenPropertyType::SKIP_FRAME_OPTION>();
    return std::get<1>(res);
}

SkipFrameStrategy RSScreenProperty::GetScreenSkipFrameStrategy() const
{
    auto res = Get<ScreenPropertyType::SKIP_FRAME_OPTION>();
    return static_cast<SkipFrameStrategy>(std::get<2>(res));
}

GraphicPixelFormat RSScreenProperty::GetPixelFormat() const
{
    return static_cast<GraphicPixelFormat>(Get<ScreenPropertyType::PIXEL_FORMAT>());
}

ScreenHDRFormat RSScreenProperty::GetScreenHDRFormat() const
{
    return static_cast<ScreenHDRFormat>(Get<ScreenPropertyType::HDR_FORMAT>());
}

bool RSScreenProperty::GetEnableVisibleRect() const
{
    auto res = Get<ScreenPropertyType::VISIBLE_RECT_OPTION>();
    return std::get<0>(res);
}

Rect RSScreenProperty::GetVisibleRect() const
{
    auto res = Get<ScreenPropertyType::VISIBLE_RECT_OPTION>();
    return std::get<1>(res);
}

bool RSScreenProperty::IsVisibleRectSupportRotation() const
{
    auto res = Get<ScreenPropertyType::VISIBLE_RECT_OPTION>();
    return std::get<2>(res);
}

std::unordered_set<NodeId> RSScreenProperty::GetWhiteList() const
{
    return Get<ScreenPropertyType::WHITE_LIST>();
}

std::unordered_set<NodeId> RSScreenProperty::GetBlackList() const
{
    return Get<ScreenPropertyType::BLACK_LIST>();
}

std::unordered_set<NodeType> RSScreenProperty::GetTypeBlackList() const
{
    return Get<ScreenPropertyType::TYPE_BLACK_LIST>();
}

std::vector<NodeId> RSScreenProperty::GetSecurityExemptionList() const
{
    return Get<ScreenPropertyType::SECURITY_EXEMPTION_LIST>();
}

std::shared_ptr<Media::PixelMap> RSScreenProperty::GetSecurityMask() const
{
    return Get<ScreenPropertyType::SECURITY_MASK>();
}

bool RSScreenProperty::EnableSkipWindow() const
{
    return Get<ScreenPropertyType::ENABLE_SKIP_WINDOW>();
}

ScreenPowerStatus RSScreenProperty::GetScreenPowerStatus() const
{
    return static_cast<ScreenPowerStatus>(Get<ScreenPropertyType::POWER_STATUS>());
}

RSScreenType RSScreenProperty::GetScreenType() const
{
    return static_cast<RSScreenType>(Get<ScreenPropertyType::SCREEN_TYPE>());
}

ScreenConnectionType RSScreenProperty::GetConnectionType() const
{
    return static_cast<ScreenConnectionType>(Get<ScreenPropertyType::CONNECTION_TYPE>());
}

#ifndef ROSEN_CROSS_PLATFORM
sptr<Surface> RSScreenProperty::GetProducerSurface() const
{
    return Get<ScreenPropertyType::PRODUCER_SURFACE>();
}
#endif

ScreenScaleMode RSScreenProperty::GetScaleMode() const
{
    return static_cast<ScreenScaleMode>(Get<ScreenPropertyType::SCALE_MODE>());
}

VirtualScreenStatus RSScreenProperty::GetVirtualScreenStatus() const
{
    return static_cast<VirtualScreenStatus>(Get<ScreenPropertyType::SCREEN_STATUS>());
}

int32_t RSScreenProperty::GetVirtualSecLayerOption() const
{
    return Get<ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION>();
}

bool RSScreenProperty::IsHardCursorSupport() const
{
    return Get<ScreenPropertyType::IS_HARD_CURSOR_SUPPORT>();
}

std::vector<ScreenColorGamut> RSScreenProperty::GetScreenSupportedColorGamuts() const
{
    return Get<ScreenPropertyType::SUPPORTED_COLOR_GAMUTS>();
}

bool RSScreenProperty::GetDisablePowerOffRenderControl() const
{
    return Get<ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL>();
}

bool RSScreenProperty::IsScreenSwitching() const
{
    return Get<ScreenPropertyType::SCREEN_SWITCH_STATUS>();
}

Gravity RSScreenProperty::GetFrameGravity() const
{
    return static_cast<Gravity>(Get<ScreenPropertyType::SCREEN_FRAME_GRAVITY>());
}

ScreenInfo RSScreenProperty::GetScreenInfo() const
{
    ScreenInfo info;
    info.id = GetScreenId();
    info.width = GetWidth();
    info.height = GetHeight();
    info.phyWidth = GetPhyWidth();
    info.phyHeight = GetPhyHeight();
    info.offsetX = GetOffsetX();
    info.offsetY = GetOffsetY();
    info.isSamplingOn = GetIsSamplingOn();
    info.samplingTranslateX = GetSamplingTranslateX();
    info.samplingTranslateY = GetSamplingTranslateY();
    info.samplingScale = GetSamplingScale();
    info.colorGamut = GetScreenColorGamut();
    info.gamutMap = GetScreenGamutMap();
    info.state = GetState();
    info.skipFrameInterval = GetScreenSkipFrameInterval();
    info.expectedRefreshRate = GetScreenExpectedRefreshRate();
    info.skipFrameStrategy = GetScreenSkipFrameStrategy();
    info.pixelFormat = GetPixelFormat();
    info.hdrFormat = GetScreenHDRFormat();
    info.whiteList = GetWhiteList();
    info.enableVisibleRect = GetEnableVisibleRect();
    info.activeRect = GetActiveRect();
    info.maskRect = GetMaskRect();
    info.reviseRect = GetReviseRect();
    info.powerStatus = GetScreenPowerStatus();
    return info;
}
// LCOV_EXCL_STOP

#define MARSHALL_CASE(ENUM_TYPE)                                                                        \
    case ENUM_TYPE: {                                                                                   \
        using T = PropertyTypeMapper<ENUM_TYPE>::value_type;                                            \
        auto property = static_cast<ScreenProperty<T>*>(prop.GetRefPtr());                              \
        if (!property || !property->Marshalling(data)) {                                                \
            RS_LOGE("%{public}s failed, type: %{public}u", __func__, static_cast<uint32_t>(ENUM_TYPE)); \
            return false;                                                                               \
        }                                                                                               \
        break;                                                                                          \
    }

bool RSScreenProperty::Marshalling(Parcel& data) const
{
    if (!data.WriteUint32(static_cast<uint32_t>(screenProperties_.size()))) {
        RS_LOGE("%{public}s write size failed", __func__);
        return false;
    }
    for (const auto& [type, prop] : screenProperties_) {
        if (!data.WriteUint32(static_cast<uint32_t>(type))) {
            RS_LOGE("%{public}s write type failed, type: %{public}u", __func__, static_cast<uint32_t>(type));
            return false;
        }

        switch (type) {
            MARSHALL_CASE(ScreenPropertyType::ID)
            MARSHALL_CASE(ScreenPropertyType::IS_VIRTUAL)
            MARSHALL_CASE(ScreenPropertyType::NAME)
            MARSHALL_CASE(ScreenPropertyType::RENDER_RESOLUTION)
            MARSHALL_CASE(ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE)
            MARSHALL_CASE(ScreenPropertyType::OFFSET)
            MARSHALL_CASE(ScreenPropertyType::SAMPLING_OPTION)
            MARSHALL_CASE(ScreenPropertyType::COLOR_GAMUT)
            MARSHALL_CASE(ScreenPropertyType::GAMUT_MAP)
            MARSHALL_CASE(ScreenPropertyType::STATE)
            MARSHALL_CASE(ScreenPropertyType::CORRECTION)
            MARSHALL_CASE(ScreenPropertyType::CANVAS_ROTATION)
            MARSHALL_CASE(ScreenPropertyType::AUTO_BUFFER_ROTATION)
            MARSHALL_CASE(ScreenPropertyType::ACTIVE_RECT_OPTION)
            MARSHALL_CASE(ScreenPropertyType::SKIP_FRAME_OPTION)
            MARSHALL_CASE(ScreenPropertyType::PIXEL_FORMAT)
            MARSHALL_CASE(ScreenPropertyType::HDR_FORMAT)
            MARSHALL_CASE(ScreenPropertyType::VISIBLE_RECT_OPTION)
            MARSHALL_CASE(ScreenPropertyType::WHITE_LIST)
            MARSHALL_CASE(ScreenPropertyType::BLACK_LIST)
            MARSHALL_CASE(ScreenPropertyType::TYPE_BLACK_LIST)
            MARSHALL_CASE(ScreenPropertyType::SECURITY_EXEMPTION_LIST)
            MARSHALL_CASE(ScreenPropertyType::SECURITY_MASK)
            MARSHALL_CASE(ScreenPropertyType::ENABLE_SKIP_WINDOW)
            MARSHALL_CASE(ScreenPropertyType::POWER_STATUS)
            MARSHALL_CASE(ScreenPropertyType::SCREEN_TYPE)
            MARSHALL_CASE(ScreenPropertyType::CONNECTION_TYPE)
            MARSHALL_CASE(ScreenPropertyType::PRODUCER_SURFACE)
            MARSHALL_CASE(ScreenPropertyType::SCALE_MODE)
            MARSHALL_CASE(ScreenPropertyType::SCREEN_STATUS)
            MARSHALL_CASE(ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION)
            MARSHALL_CASE(ScreenPropertyType::IS_HARD_CURSOR_SUPPORT)
            MARSHALL_CASE(ScreenPropertyType::SUPPORTED_COLOR_GAMUTS)
            MARSHALL_CASE(ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL)
            MARSHALL_CASE(ScreenPropertyType::SCREEN_SWITCH_STATUS)
            MARSHALL_CASE(ScreenPropertyType::SCREEN_FRAME_GRAVITY)
            default:
                RS_LOGW("%{public}s invalid type: %{public}u", __func__, static_cast<uint32_t>(type));
        }
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

#define UNMARSHALL_CASE(ENUM_TYPE)                                                                      \
    case ENUM_TYPE: {                                                                                   \
        using T = PropertyTypeMapper<ENUM_TYPE>::value_type;                                            \
        auto property = sptr<ScreenProperty<T>>(ScreenProperty<T>::Unmarshalling(data));                \
        if (!property) {                                                                                \
            RS_LOGE("%{public}s failed, type: %{public}u", __func__, static_cast<uint32_t>(ENUM_TYPE)); \
            return false;                                                                               \
        }                                                                                               \
        screenProperties_.insert_or_assign(ENUM_TYPE, property);                                        \
        break;                                                                                          \
    }

bool RSScreenProperty::UnmarshallingData(Parcel& data)
{
    uint32_t size = 0;
    if (!data.ReadUint32(size)) {
        RS_LOGE("%{public}s read size failed", __func__);
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        uint32_t type;
        if (!data.ReadUint32(type)) {
            RS_LOGE("%{public}s read type failed", __func__);
            return false;
        }
        switch (static_cast<ScreenPropertyType>(type)) {
            UNMARSHALL_CASE(ScreenPropertyType::ID)
            UNMARSHALL_CASE(ScreenPropertyType::IS_VIRTUAL)
            UNMARSHALL_CASE(ScreenPropertyType::NAME)
            UNMARSHALL_CASE(ScreenPropertyType::RENDER_RESOLUTION)
            UNMARSHALL_CASE(ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE)
            UNMARSHALL_CASE(ScreenPropertyType::OFFSET)
            UNMARSHALL_CASE(ScreenPropertyType::SAMPLING_OPTION)
            UNMARSHALL_CASE(ScreenPropertyType::COLOR_GAMUT)
            UNMARSHALL_CASE(ScreenPropertyType::GAMUT_MAP)
            UNMARSHALL_CASE(ScreenPropertyType::STATE)
            UNMARSHALL_CASE(ScreenPropertyType::CORRECTION)
            UNMARSHALL_CASE(ScreenPropertyType::CANVAS_ROTATION)
            UNMARSHALL_CASE(ScreenPropertyType::AUTO_BUFFER_ROTATION)
            UNMARSHALL_CASE(ScreenPropertyType::ACTIVE_RECT_OPTION)
            UNMARSHALL_CASE(ScreenPropertyType::SKIP_FRAME_OPTION)
            UNMARSHALL_CASE(ScreenPropertyType::PIXEL_FORMAT)
            UNMARSHALL_CASE(ScreenPropertyType::HDR_FORMAT)
            UNMARSHALL_CASE(ScreenPropertyType::VISIBLE_RECT_OPTION)
            UNMARSHALL_CASE(ScreenPropertyType::WHITE_LIST)
            UNMARSHALL_CASE(ScreenPropertyType::BLACK_LIST)
            UNMARSHALL_CASE(ScreenPropertyType::TYPE_BLACK_LIST)
            UNMARSHALL_CASE(ScreenPropertyType::SECURITY_EXEMPTION_LIST)
            UNMARSHALL_CASE(ScreenPropertyType::SECURITY_MASK)
            UNMARSHALL_CASE(ScreenPropertyType::ENABLE_SKIP_WINDOW)
            UNMARSHALL_CASE(ScreenPropertyType::POWER_STATUS)
            UNMARSHALL_CASE(ScreenPropertyType::SCREEN_TYPE)
            UNMARSHALL_CASE(ScreenPropertyType::CONNECTION_TYPE)
            UNMARSHALL_CASE(ScreenPropertyType::PRODUCER_SURFACE)
            UNMARSHALL_CASE(ScreenPropertyType::SCALE_MODE)
            UNMARSHALL_CASE(ScreenPropertyType::SCREEN_STATUS)
            UNMARSHALL_CASE(ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION)
            UNMARSHALL_CASE(ScreenPropertyType::IS_HARD_CURSOR_SUPPORT)
            UNMARSHALL_CASE(ScreenPropertyType::SUPPORTED_COLOR_GAMUTS)
            UNMARSHALL_CASE(ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL)
            UNMARSHALL_CASE(ScreenPropertyType::SCREEN_SWITCH_STATUS)
            UNMARSHALL_CASE(ScreenPropertyType::SCREEN_FRAME_GRAVITY)
            default:
                RS_LOGW("%{public}s invalid type: %{public}u", __func__, static_cast<uint32_t>(type));
        }
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS
