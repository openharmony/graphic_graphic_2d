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
enum class ScreenPropertyType : uint32_t {
    ID = 0,
    IS_VIRTUAL,
    NAME,
    RENDER_RESOLUTION,
    PHYSICAL_RESOLUTION_REFRESHRATE,
    OFFSET,
    SAMPLING_OPTION,
    COLOR_GAMUT,
    GAMUT_MAP,
    STATE,
    CORRECTION,
    CANVAS_ROTATION,
    AUTO_BUFFER_ROTATION,
    ACTIVE_RECT_OPTION,
    SKIP_FRAME_OPTION,
    PIXEL_FORMAT,
    HDR_FORMAT,
    VISIBLE_RECT_OPTION,
    WHITE_LIST,
    BLACK_LIST,
    TYPE_BLACK_LIST,
    SECURITY_EXEMPTION_LIST,
    SECURITY_MASK,
    ENABLE_SKIP_WINDOW,
    POWER_STATUS,
    SCREEN_TYPE,
    CONNECTION_TYPE,
    PRODUCER_SURFACE,
    SCALE_MODE,
    SCREEN_STATUS,
    VIRTUAL_SEC_LAYER_OPTION,
    IS_HARD_CURSOR_SUPPORT,
    SUPPORTED_COLOR_GAMUTS,
    DISABLE_POWER_OFF_RENDER_CONTROL,
    SCREEN_SWITCH_STATUS,
    SCREEN_FRAME_GRAVITY,
};

template<ScreenPropertyType T>
struct PropertyTypeMapper;

#define DECLARE_PROPERTY_TYPE(enum_type, val_type, default_val)     \
    template<>                                                      \
    struct PropertyTypeMapper<enum_type> {                          \
        using value_type = val_type;                                \
        static value_type default_value() { return default_val; }   \
    }

DECLARE_PROPERTY_TYPE(ScreenPropertyType::ID, ScreenId, INVALID_SCREEN_ID);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::IS_VIRTUAL, bool, false);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::NAME, std::string, "");
using resolutionValType = std::pair<uint32_t, uint32_t>;
DECLARE_PROPERTY_TYPE(ScreenPropertyType::RENDER_RESOLUTION, resolutionValType, resolutionValType(0u, 0u));
using phyResolutionValType = std::tuple<uint32_t, uint32_t, uint32_t>;
DECLARE_PROPERTY_TYPE(ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE,
    phyResolutionValType, phyResolutionValType(0u, 0u, 0u));
using offsetValType = std::pair<int32_t, int32_t>;
DECLARE_PROPERTY_TYPE(ScreenPropertyType::OFFSET, offsetValType, offsetValType(0, 0));
using samplingValType = std::tuple<bool, float, float, float>;
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SAMPLING_OPTION, samplingValType, samplingValType(false, 0.f, 0.f, 1.f));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::COLOR_GAMUT, uint32_t,
    static_cast<uint32_t>(ScreenColorGamut::COLOR_GAMUT_SRGB));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::GAMUT_MAP, uint32_t,
    static_cast<uint32_t>(ScreenGamutMap::GAMUT_MAP_CONSTANT));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::STATE, uint8_t, static_cast<uint8_t>(ScreenState::UNKNOWN));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::CORRECTION, uint32_t, static_cast<uint32_t>(ScreenRotation::ROTATION_0));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::CANVAS_ROTATION, bool, false);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::AUTO_BUFFER_ROTATION, bool, false);
using activeRectValType = std::tuple<RectI, RectI, RectI>;
DECLARE_PROPERTY_TYPE(ScreenPropertyType::ACTIVE_RECT_OPTION,
    activeRectValType, activeRectValType(RectI(), RectI(), RectI()));
using skipFrameValType = std::tuple<uint32_t, uint32_t, uint8_t>;
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SKIP_FRAME_OPTION, skipFrameValType, skipFrameValType(
    DEFAULT_SKIP_FRAME_INTERVAL, INVALID_EXPECTED_REFRESH_RATE, static_cast<uint8_t>(SKIP_FRAME_BY_INTERVAL)));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::PIXEL_FORMAT,
    int32_t, static_cast<int32_t>(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::HDR_FORMAT,
    uint32_t, static_cast<uint32_t>(ScreenHDRFormat::NOT_SUPPORT_HDR));
using visibleRectValType = std::tuple<bool, Rect, bool>;
DECLARE_PROPERTY_TYPE(ScreenPropertyType::VISIBLE_RECT_OPTION,
    visibleRectValType, visibleRectValType(false, Rect(), false));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::WHITE_LIST, std::unordered_set<NodeId>, std::unordered_set<NodeId>());
DECLARE_PROPERTY_TYPE(ScreenPropertyType::BLACK_LIST, std::unordered_set<NodeId>, std::unordered_set<NodeId>());
DECLARE_PROPERTY_TYPE(ScreenPropertyType::TYPE_BLACK_LIST,
    std::unordered_set<NodeType>, std::unordered_set<NodeType>());
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SECURITY_EXEMPTION_LIST, std::vector<NodeId>, std::vector<NodeId>());
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SECURITY_MASK, std::shared_ptr<Media::PixelMap>, nullptr);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::ENABLE_SKIP_WINDOW, bool, false);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::POWER_STATUS,
    uint32_t, static_cast<uint32_t>(ScreenPowerStatus::INVALID_POWER_STATUS));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SCREEN_TYPE,
    uint32_t, static_cast<uint32_t>(RSScreenType::UNKNOWN_TYPE_SCREEN));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::CONNECTION_TYPE,
    uint32_t, static_cast<uint32_t>(ScreenConnectionType::INVALID_DISPLAY_CONNECTION_TYPE));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::PRODUCER_SURFACE, sptr<Surface>, nullptr);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SCALE_MODE, uint32_t, static_cast<uint32_t>(ScreenScaleMode::UNISCALE_MODE));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SCREEN_STATUS, uint32_t, static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
DECLARE_PROPERTY_TYPE(ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION, int32_t, 0);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::IS_HARD_CURSOR_SUPPORT, bool, false);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SUPPORTED_COLOR_GAMUTS,
    std::vector<ScreenColorGamut>, std::vector<ScreenColorGamut>());
DECLARE_PROPERTY_TYPE(ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL, bool, false);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SCREEN_SWITCH_STATUS, bool, false);
DECLARE_PROPERTY_TYPE(ScreenPropertyType::SCREEN_FRAME_GRAVITY, int32_t, static_cast<int32_t>(Gravity::DEFAULT));

class ScreenPropertyBase : public Parcelable {
public:
    ScreenPropertyBase() = default;
    virtual ~ScreenPropertyBase() = default;
};

template<typename T>
class ScreenProperty : public ScreenPropertyBase {
public:
    ScreenProperty() = default;
    ScreenProperty(const T& value) : value_(value) {}
    ~ScreenProperty() override = default;

    T Get() { return value_; }
    void Set(const T& value) { value_ = value; }

    bool Marshalling(Parcel& data) const override
    {
        return RSMarshallingHelper::Marshalling(data, value_);
    }
    static ScreenProperty<T>* Unmarshalling(Parcel& data)
    {
        auto property = new ScreenProperty<T>();
        if (RSMarshallingHelper::Unmarshalling(data, property->value_)) {
            return property;
        }
        delete property;
        return nullptr;
    }

private:
    T value_;
};

class RSB_EXPORT RSScreenProperty : public Parcelable {
public:
    RSScreenProperty() = default;
    virtual ~RSScreenProperty() = default;

    bool Marshalling(Parcel& data) const override;
    static RSScreenProperty* Unmarshalling(Parcel& data);

    bool UnmarshallingData(Parcel& data);

    ScreenId GetScreenId() const;
    bool IsVirtual() const;
    std::string Name() const;
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
    std::unordered_set<NodeId> GetWhiteList() const;
    std::unordered_set<NodeId> GetBlackList() const;
    std::unordered_set<NodeType> GetTypeBlackList() const;
    std::vector<NodeId> GetSecurityExemptionList() const;
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
    std::vector<ScreenColorGamut> GetScreenSupportedColorGamuts() const;
    bool GetDisablePowerOffRenderControl() const;
    bool IsScreenSwitching() const;
    Gravity GetFrameGravity() const;

    ScreenInfo GetScreenInfo() const;

    float GetRogWidthRatio() const
    {
        return (GetWidth() == 0) ? 1.f : static_cast<float>(GetPhyWidth()) / GetWidth();
    }

    float GetRogHeightRatio() const
    {
        return (GetHeight() == 0) ? 1.f : static_cast<float>(GetPhyHeight()) / GetHeight();
    }

    void Set(ScreenPropertyType type, const sptr<ScreenPropertyBase>& property)
    {
        screenProperties_.insert_or_assign(type, property);
    }

private:
    template<ScreenPropertyType propType>
    sptr<ScreenPropertyBase> Set(const typename PropertyTypeMapper<propType>::value_type& val) {
        using T = typename PropertyTypeMapper<propType>::value_type;
        if (ROSEN_EQ(Get<propType>(), val)) {
            return nullptr;
        }
        auto screenProperty = sptr<ScreenProperty<T>>::MakeSptr(val);
        screenProperties_.insert_or_assign(propType, screenProperty);
        return screenProperty;
    }

    template<ScreenPropertyType propType>
    typename PropertyTypeMapper<propType>::value_type Get() const {
        auto it = screenProperties_.find(propType);
        if (it == screenProperties_.end()) {
            return PropertyTypeMapper<propType>::default_value();
        }
        using T = typename PropertyTypeMapper<propType>::value_type;
        auto property = static_cast<ScreenProperty<T>*>(it->second.GetRefPtr());
        return property->Get();
    }

    std::map<ScreenPropertyType, sptr<ScreenPropertyBase>> screenProperties_;

    friend class RSScreenThreadSafeProperty;
};
}  // namespace OHOS::Rosen
#endif  // RENDER_SERVICE_BASE_SCREEN_MANAGER_RS_SCREEN_PROPERTY_H