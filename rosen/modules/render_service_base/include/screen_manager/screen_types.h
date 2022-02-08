/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RS_BASE_SCREEN_TYPES
#define RS_BASE_SCREEN_TYPES

#include <cstdint>
#include <type_traits>

namespace OHOS {
namespace Rosen {
// Logical ScreenId for both physical and virtual screen.
using ScreenId = uint64_t;

// Screen Physical Id provided by hdi-backend.
using ScreenPhysicalId = uint32_t;

constexpr ScreenId INVALID_SCREEN_ID = ~(static_cast<ScreenId>(0));

constexpr int32_t INVALID_BACKLIGHT_VALUE = -1;

inline constexpr ScreenId ToScreenId(ScreenPhysicalId physicalId)
{
    return static_cast<ScreenId>(physicalId);
}

inline constexpr ScreenPhysicalId ToScreenPhysicalId(ScreenId id)
{
    return static_cast<ScreenPhysicalId>(id);
}

enum class ScreenEvent : uint8_t {
    CONNECTED,
    DISCONNECTED,
    UNKNOWN,
};

enum class ScreenRotation : uint32_t {
    ROTATION_0 = 0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
    INVALID_SCREEN_ROTATION,
};

typedef enum : uint32_t {
    POWER_STATUS_ON = 0,
    POWER_STATUS_STANDBY,
    POWER_STATUS_SUSPEND,
    POWER_STATUS_OFF,
    POWER_STATUS_BUTT,
    INVALID_POWER_STATUS,
}ScreenPowerStatus;

typedef enum : uint32_t {
    DISP_INTF_HDMI = 0,
    DISP_INTF_LCD,
    DISP_INTF_BT1120,
    DISP_INTF_BT656,
    DISP_INTF_YPBPR,
    DISP_INTF_RGB,
    DISP_INTF_CVBS,
    DISP_INTF_SVIDEO,
    DISP_INTF_VGA,
    DISP_INTF_MIPI,
    DISP_INTF_PANEL,
    DISP_INTF_BUTT,
    DISP_INVALID,
}ScreenInterfaceType;

typedef enum : uint32_t {
    COLOR_GAMUT_INVALID = UINT32_MAX,
    COLOR_GAMUT_NATIVE = 0,
    COLOR_GAMUT_STANDARD_BT601,
    COLOR_GAMUT_STANDARD_BT709,
    COLOR_GAMUT_DCI_P3,
    COLOR_GAMUT_SRGB,
    COLOR_GAMUT_ADOBE_RGB,
    COLOR_GAMUT_DISPLAY_P3,
    COLOR_GAMUT_BT2020,
    COLOR_GAMUT_BT2100_PQ,
    COLOR_GAMUT_BT2100_HLG,
    COLOR_GAMUT_DISPLAY_BT2020,
} ScreenColorGamut;

typedef enum : uint32_t {
    GAMUT_MAP_CONSTANT = 0,
    GAMUT_MAP_EXTENSION,
    GAMUT_MAP_HDR_CONSTANT,
    GAMUT_MAP_HDR_EXTENSION,
} ScreenGamutMap;

typedef enum : uint32_t {
    SUCCESS = 0,
    SCREEN_NOT_FOUND,
    RS_CONNECTION_ERROR,
    SURFACE_NOT_UNIQUE,
    RENDER_SERVICE_NULL,
    INVALID_AUGMENTS,
    WRITE_PARCEL_ERR,
} StatusCode;

// get the underlying type of an enum value.
template<typename EnumType>
inline constexpr typename std::underlying_type<EnumType>::type ECast(EnumType t)
{
    return static_cast<typename std::underlying_type<EnumType>::type>(t);
}
} // namespace Rosen
} // namespace OHOS

#endif // RS_BASE_SCREEN_TYPES
