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

constexpr uint32_t LAYER_COMPOSITION_CAPACITY = 12;

constexpr uint32_t DEFAULT_SKIP_FRAME_INTERVAL = 1;

constexpr int32_t SCREEN_ROTATION_NUM = 4;

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

enum class ScreenScaleMode : uint32_t {
    FILL_MODE = 0,
    UNISCALE_MODE,
    INVALID_MODE,
};

typedef enum : uint32_t {
    BUILT_IN_TYPE_SCREEN = 0,
    EXTERNAL_TYPE_SCREEN,
    VIRTUAL_TYPE_SCREEN,
    UNKNOWN_TYPE_SCREEN,
} RSScreenType;

typedef enum : uint32_t {
    POWER_STATUS_ON = 0,
    POWER_STATUS_STANDBY,
    POWER_STATUS_SUSPEND,
    POWER_STATUS_OFF,
    POWER_STATUS_OFF_FAKE,
    POWER_STATUS_ON_ADVANCED,
    POWER_STATUS_OFF_ADVANCED,
    POWER_STATUS_BUTT,
    INVALID_POWER_STATUS,
} ScreenPowerStatus;

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
} ScreenInterfaceType;

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
    NOT_SUPPORT_HDR = 0,
    VIDEO_HLG,
    VIDEO_HDR10,
    VIDEO_HDR_VIVID,
    IMAGE_HDR_VIVID_DUAL,
    IMAGE_HDR_VIVID_SINGLE,
    IMAGE_HDR_ISO_DUAL,
    IMAGE_HDR_ISO_SINGLE,
} ScreenHDRFormat;

typedef enum : uint32_t {
    MATAKEY_RED_PRIMARY_X = 0,
    MATAKEY_RED_PRIMARY_Y = 1,
    MATAKEY_GREEN_PRIMARY_X = 2,
    MATAKEY_GREEN_PRIMARY_Y = 3,
    MATAKEY_BLUE_PRIMARY_X = 4,
    MATAKEY_BLUE_PRIMARY_Y = 5,
    MATAKEY_WHITE_PRIMARY_X = 6,
    MATAKEY_WHITE_PRIMARY_Y = 7,
    MATAKEY_MAX_LUMINANCE = 8,
    MATAKEY_MIN_LUMINANCE = 9,
    MATAKEY_MAX_CONTENT_LIGHT_LEVEL = 10,
    MATAKEY_MAX_FRAME_AVERAGE_LIGHT_LEVEL = 11,
    MATAKEY_HDR10_PLUS = 12,
    MATAKEY_HDR_VIVID = 13,
} ScreenHDRMetadataKey;

/*
 * @brief Defines the screen HDR metadata.
 */
typedef struct {
    ScreenHDRMetadataKey key;
    float value;
} ScreenHDRMetaData;

typedef enum : uint32_t {
    SUCCESS = 0,
    SCREEN_NOT_FOUND,
    RS_CONNECTION_ERROR,
    SURFACE_NOT_UNIQUE,
    RENDER_SERVICE_NULL,
    INVALID_ARGUMENTS,
    WRITE_PARCEL_ERR,
    HDI_ERROR,
    SCREEN_MANAGER_NULL,
} StatusCode;

/*
 * @brief Enumerates screen color key types supported by hardware acceleration.
 */
typedef enum {
    SCREEN_CKEY_NONE = 0,      /**< No color key */
    SCREEN_CKEY_SRC,           /**< Source color key */
    SCREEN_CKEY_DST,           /**< Destination color key */
    SCREEN_CKEY_BUTT           /**< Null operation */
} ScreenColorKey;

/*
 * @brief Enumerates screen mirror types supported by hardware acceleration.
 */
typedef enum {
    SCREEN_MIRROR_NONE = 0,      /**< No mirror */
    SCREEN_MIRROR_LR,            /**< Left and right mirrors */
    SCREEN_MIRROR_TB,            /**< Top and bottom mirrors */
    SCREEN_MIRROR_BUTT           /**< Null operation */
} ScreenMirrorType;

/*
 * @brief Enumerates the color data spaces.
 */
typedef enum {
    SCREEN_COLOR_DATA_SPACE_UNKNOWN = 0,
    SCREEN_GAMUT_BT601 = 0x00000001,
    SCREEN_GAMUT_BT709 = 0x00000002,
    SCREEN_GAMUT_DCI_P3 = 0x00000003,
    SCREEN_GAMUT_SRGB = 0x00000004,
    SCREEN_GAMUT_ADOBE_RGB = 0x00000005,
    SCREEN_GAMUT_DISPLAY_P3 = 0x00000006,
    SCREEN_GAMUT_BT2020 = 0x00000007,
    SCREEN_GAMUT_BT2100_PQ = 0x00000008,
    SCREEN_GAMUT_BT2100_HLG = 0x00000009,
    SCREEN_GAMUT_DISPLAY_BT2020 = 0x0000000a,
    SCREEN_TRANSFORM_FUNC_UNSPECIFIED = 0x00000100,
    SCREEN_TRANSFORM_FUNC_LINEAR = 0x00000200,
    SCREEN_TRANSFORM_FUNC_SRGB = 0x00000300,
    SCREEN_TRANSFORM_FUNC_SMPTE_170M = 0x00000400,
    SCREEN_TRANSFORM_FUNC_GM2_2 = 0x00000500,
    SCREEN_TRANSFORM_FUNC_GM2_6 = 0x00000600,
    SCREEN_TRANSFORM_FUNC_GM2_8 = 0x00000700,
    SCREEN_TRANSFORM_FUNC_ST2084 = 0x00000800,
    SCREEN_TRANSFORM_FUNC_HLG = 0x00000900,
    SCREEN_PRECISION_UNSPECIFIED = 0x00010000,
    SCREEN_PRECISION_FULL = 0x00020000,
    SCREEN_PRESION_LIMITED = 0x00030000,
    SCREEN_PRESION_EXTENDED = 0x00040000,
    SCREEN_BT601_SMPTE170M_FULL = SCREEN_GAMUT_BT601 | SCREEN_TRANSFORM_FUNC_SMPTE_170M | SCREEN_PRECISION_FULL,
    SCREEN_BT601_SMPTE170M_LIMITED = SCREEN_GAMUT_BT601 | SCREEN_TRANSFORM_FUNC_SMPTE_170M | SCREEN_PRESION_LIMITED,
    SCREEN_BT709_LINEAR_FULL = SCREEN_GAMUT_BT709 | SCREEN_TRANSFORM_FUNC_LINEAR | SCREEN_PRECISION_FULL,
    SCREEN_BT709_LINEAR_EXTENDED = SCREEN_GAMUT_BT709 | SCREEN_TRANSFORM_FUNC_LINEAR | SCREEN_PRESION_EXTENDED,
    SCREEN_BT709_SRGB_FULL = SCREEN_GAMUT_BT709 | SCREEN_TRANSFORM_FUNC_SRGB | SCREEN_PRECISION_FULL,
    SCREEN_BT709_SRGB_EXTENDED = SCREEN_GAMUT_BT709 | SCREEN_TRANSFORM_FUNC_SRGB | SCREEN_PRESION_EXTENDED,
    SCREEN_BT709_SMPTE170M_LIMITED = SCREEN_GAMUT_BT709 | SCREEN_TRANSFORM_FUNC_SMPTE_170M | SCREEN_PRESION_LIMITED,
    SCREEN_DCI_P3_LINEAR_FULL = SCREEN_GAMUT_DCI_P3 | SCREEN_TRANSFORM_FUNC_LINEAR | SCREEN_PRECISION_FULL,
    SCREEN_DCI_P3_GAMMA26_FULL = SCREEN_GAMUT_DCI_P3 | SCREEN_TRANSFORM_FUNC_GM2_6 | SCREEN_PRECISION_FULL,
    SCREEN_DISPLAY_P3_LINEAR_FULL = SCREEN_GAMUT_DISPLAY_P3 | SCREEN_TRANSFORM_FUNC_LINEAR | SCREEN_PRECISION_FULL,
    SCREEN_DCI_P3_SRGB_FULL = SCREEN_GAMUT_DCI_P3 | SCREEN_TRANSFORM_FUNC_SRGB | SCREEN_PRECISION_FULL,
    SCREEN_ADOBE_RGB_GAMMA22_FULL = SCREEN_GAMUT_ADOBE_RGB | SCREEN_TRANSFORM_FUNC_GM2_2 | SCREEN_PRECISION_FULL,
    SCREEN_BT2020_LINEAR_FULL = SCREEN_GAMUT_BT2020 | SCREEN_TRANSFORM_FUNC_LINEAR | SCREEN_PRECISION_FULL,
    SCREEN_BT2020_SRGB_FULL = SCREEN_GAMUT_BT2020 | SCREEN_TRANSFORM_FUNC_SRGB | SCREEN_PRECISION_FULL,
    SCREEN_BT2020_SMPTE170M_FULL = SCREEN_GAMUT_BT2020 | SCREEN_TRANSFORM_FUNC_SMPTE_170M | SCREEN_PRECISION_FULL,
    SCREEN_BT2020_ST2084_FULL = SCREEN_GAMUT_BT2020 | SCREEN_TRANSFORM_FUNC_ST2084 | SCREEN_PRECISION_FULL,
    SCREEN_BT2020_HLG_FULL = SCREEN_GAMUT_BT2020 | SCREEN_TRANSFORM_FUNC_HLG | SCREEN_PRECISION_FULL,
    SCREEN_BT2020_ST2084_LIMITED = SCREEN_GAMUT_BT2020 | SCREEN_TRANSFORM_FUNC_ST2084 | SCREEN_PRESION_LIMITED,
} ScreenColorDataSpace;

// get the underlying type of an enum value.
template<typename EnumType>
inline constexpr typename std::underlying_type<EnumType>::type ECast(EnumType t)
{
    return static_cast<typename std::underlying_type<EnumType>::type>(t);
}
} // namespace Rosen
} // namespace OHOS

#endif // RS_BASE_SCREEN_TYPES
